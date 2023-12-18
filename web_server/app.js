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

var app = express();

// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'pug');

app.use(logger('dev'));
app.use(express.json());
app.use(express.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));

app.use('/', indexRouter);
app.use('/users', usersRouter);

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

app.post('/clicked', async (req, res) => {
  const ESPdata = await getESPdata();
  res.status(201).send(ESPdata)
});

app.post('/loglevel', async (req, res) => {
  var log_level = { esp_log_level: req.body.esp_log_level }
  res.status(201).send(log_level)
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