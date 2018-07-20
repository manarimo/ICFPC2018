const express = require('express');
const bodyParser = require('body-parser');
const imageDataUri = require('image-data-uri');
const fs = require('fs');
const app = express();

app.use(bodyParser.json({ limit: '100mb' }));

app.use(function(req, res, next) {
    res.header("Access-Control-Allow-Origin", "*");
    res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
    next();
});

app.post('/', (req, res) => {
    const buf = imageDataUri.decode(req.body.data).dataBuffer;
    fs.writeFile(`raw/${req.body.name}`, buf);
    console.log(`Saved ${req.body.name}`);
    res.send('ok');
});

app.listen(25252);
