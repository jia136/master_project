import { fileURLToPath } from 'url';
import new_path from 'path';
import { getESPdata, getESPonedata, createEspData } from './database.js';

const __filename = fileURLToPath(import.meta.url);
const __dirname = new_path.dirname(__filename);

import createError from 'http-errors'
import express from 'express'
import path from 'path'
import cookieParser from 'cookie-parser'
import logger from 'morgan'
import indexRouter from './routes/index.js'
import usersRouter from './routes/users.js'

import WebSocket from 'ws'
import { WebSocketServer } from 'ws';
//import multer from 'multer'
import fs from 'fs'
import bodyParser from 'body-parser';

// Allow assets directory listings
import serveIndex from 'serve-index'; 
import { request } from 'http';



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

//app.use('/images', express.static('images'));

app.use('/', indexRouter);
app.use('/users', usersRouter);

const wss = new WebSocketServer({ port: 5000 });

app.locals.temperatura = "123"

//MySQL ESP all inforamtion
app.get('/ESPdata', async (req, res) => {
  const ESPdata = await getESPdata()
  res.send(ESPdata)
});

//MySQLESP get one data from database
app.get('/ESPdata/:id', async (req, res) => {
  const id = req.params.id
  const ESPdata = await getESPonedata(id)
  res.send(ESPdata)
});

app.post('/ESPdata', async (req, res) => {
  const { title, value } = req.body
  const sensor_data = await createEspData(title, value)
  res.status(201).send(sensor_data)
});

/*app.post('/temperature', async (req, res) => {
  //var temp_val = { temp: req.body.temp }
  //console.log(temp_val)
  //app.locals.temperatura = JSON.stringify(temp_val)
  //res.status(201).send(temp_val)
  res.sendStatus(201);
});*/

app.post('/test', async(req, res) => {    

  req.pipe(fs.createWriteStream('./images/image.jpg'))
  .on('close', () => {
    console.log('Image downloaded successfully!');
  })
  .on('error', (err) => {
    console.error('Error downloading the image:', err);
  });
  
  res.sendStatus(201)
});

app.get('/test', async (req, res) => {

  res.sendStatus(201)
});

app.post('/clicked', async (req, res) => {
  const ESPdata = await getESPdata();
  res.status(201).send(ESPdata)
});

app.post('/loglevel', async (req, res) => {
  var log_level = { esp_log_level: req.body.esp_log_level }
  res.status(201).send(log_level)
});

wss.on('connection', function connection(ws) {
  console.log('A new client Connected!');
  ws.send('Welcome New Client!');

  ws.on('message', function incoming(message) {
    console.log('received: %s', message);

    wss.clients.forEach(function each(client) {
      if (client !== ws && client.readyState === WebSocket.OPEN) {
        client.send(message);
      }
    });
    
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
  // create an array restaurants
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
  //res.sendFile(stream_image);
  //const stream_image = await fs.createReadStream('./images/image.jpg').pipe(request.put('http://192.168.1.7:3000/image.jpg'));
  //console.log('temp');
  //res.status(200);
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