import { fileURLToPath } from 'url';
import new_path from 'path';
import { getESPdata } from './database.js';

const __filename = fileURLToPath(import.meta.url);
const __dirname = new_path.dirname(__filename);

import createError from 'http-errors'
import express from 'express'
import path from 'path'
import cookieParser from 'cookie-parser'
import logger from 'morgan'
import indexRouter from './routes/index.js'
import usersRouter from './routes/users.js'

//import multer from 'multer'
import fs from 'fs'
import bodyParser from 'body-parser';
import { spawn } from 'child_process';

// Allow assets directory listings
import serveIndex from 'serve-index'; 

var app = express();

// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'pug');

app.use(logger('dev'));
app.use(express.json());
app.use(express.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));
app.use('/images', serveIndex(path.join(__dirname, '/images')))

app.use(bodyParser.raw({
  type: 'image/jpg',
  limit: '10mb'
}));


app.use('/', indexRouter);
app.use('/users', usersRouter);

app.locals.temperature= "[no data]";
app.locals.pressure= "[no data]";
app.locals.alarm= "[no data]";
app.locals.log_level_esp="4"; //DEBUG - log level
app.locals.cap_level_esp="3"; //1024 bytes - cap level

app.locals.imageNum = 0;

//MySQL ESP all inforamtion
app.get('/ESPdata', async (req, res) => {
  const ESPdata = await getESPdata()
  res.send(ESPdata)
});

app.post('/sensor', async (req, res) => {
  const { temp, pres, alarm } = req.body
  console.log(temp);
  console.log(pres);
  console.log(alarm);
  app.locals.temperature = temp;
  app.locals.pressure = pres;
  app.locals.alarm = alarm;
  res.sendStatus(201);
});

app.post('/camera', async(req, res) => {

  req.pipe(fs.createWriteStream('./images/image_' + app.locals.imageNum + '.jpg' ))
  .on('close', () => {
    console.log('Image downloaded successfully!');
  })
  .on('error', (err) => {
    console.error('Error downloading the image:', err);
  });
  app.locals.imageNum++;
  res.sendStatus(201)
  
});

app.post('/clicked', async (req, res) => {
  const ESPdata = await getESPdata();
  res.status(201).send(ESPdata)
});

app.post('/loglevel', async (req, res) => {
  var log_level = { esp_log_level: req.body.esp_log_level }
  app.locals.log_level_esp = log_level.esp_log_level;
  console.log(app.locals.log_level_esp);
  res.status(201).send(log_level);
});

app.post('/caplevel', async (req, res) => {
  var cap_level = { esp_cap_level: req.body.esp_cap_level }
  app.locals.cap_level_esp = cap_level.esp_cap_level;
  console.log(app.locals.cap_level_esp);
  res.status(201).send(cap_level);
});

app.post('/logEsp', async (req, res) => {

  req.pipe(fs.createWriteStream('./python_decoder/log_from_esp.txt' ))
  .on('close', () => {
    console.log('Esp log downloaded successfully!');
  })
  .on('error', (err) => {
    console.error('Error downloading the esp log:', err);
  });
  res.status(201).send(`L${app.locals.log_level_esp}C${app.locals.cap_level_esp}`);
  const python = spawn('python', ['./python_decoder/script1.py', './python_decoder/log_from_esp.txt', 'log_parsed_esp.csv']);

  python.on('close', (code) => {
    console.log(`child process close all stdio with code ${code}`);
  });

});

app.post('/logEspCam', async (req, res) => {
  
  req.pipe(fs.createWriteStream('./python_decoder/log_from_esp_cam.txt' ))
  .on('close', () => {
    console.log('Esp log cam downloaded successfully!');
  })
  .on('error', (err) => {
    console.error('Error downloading the esp log cam:', err);
  });
  res.status(201).send(`L${app.locals.log_level_esp}C${app.locals.cap_level_esp}`);

  const python = spawn('python', ['./python_decoder/script1.py', './python_decoder/log_from_esp_cam.txt', 'log_parsed_esp_cam.csv']);

  python.on('close', (code) => {
    console.log(`child process close all stdio with code ${code}`);
  });

});

app.post('/getImage' , async (req, res) => {
  const send_image = req.body.name
  console.log(send_image);
  const stream_image = await fs.createReadStream('./images/'+ send_image);
  stream_image.pipe(res);
});

app.get('/image', async (req, res) => {
  
  var file_size = {};
  var file_time = {};
  var files  = fs.readdirSync('./images');
  console.log(files);
  files.forEach(function each(item, index) {
    file_size[index] = (fs.statSync('./images/' + item)).size;
    file_time[index] = (fs.statSync('./images/' + item)).mtime;
  });

  function Imagejson(image_name, image_size, image_time) {
    this.image_name = image_name;
    this.image_size = image_size;
    this.image_time = image_time;
  }
  // create an array
  var imagejson = [];
  // add objects to the array
  files.forEach(function each(item, index) {
    imagejson.push(new Imagejson(item, file_size[index], file_time[index]));
  })

  res.json({
    status: 'success',
    data: JSON.stringify(imagejson)
  });

});

app.post('/temp', async (req, res, next) => {

  const send_image = req.body.name
  console.log(send_image);
  const stream_image = await fs.createReadStream('./images/' + send_image);
  stream_image.pipe(res);
});

// catch 404 and forward to error handler
app.use(function(req, res, next) {
  next(createError(404));
});

// error handler
app.use(function(err, req, res, next) {
  // set locals, only providing error in development
  res.locals.message = err.message;
  res.locals.error = req.app.get('env') === 'development' ? err : {};

  // render the error page
  res.status(err.status || 500);
  res.render('error');
});

export default app