import express from 'express'
var router = express.Router();
import fs from 'fs'

/* GET home page. */
router.get('/', function(req, res, next) {
  res.render('index', { title: 'ESP32 info' });
});


export default router;