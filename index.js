const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline')

const port = new SerialPort({
  path: 'COM3',
  baudRate: 9600
});

//console.log(port.path);

const parser = port.pipe(new ReadlineParser({ delimiter: '\r\n' }))

port.on('open', function () {
  console.log(`Serial OPEN ${port.path}`);
  setInterval(write, 1000);
});

parser.on('data', function (data) {
  console.log('Data: ' + data);
});

function write() {
  //console.log('Write: ' + data);
  port.write(new Buffer.from(`${new Date().getHours()}${new Date().getMinutes()}\n`), function (err, results) {
    if (err) {
      console.log('Err: ' + err);
      console.log('Results: ' + results);
    }
  });
}