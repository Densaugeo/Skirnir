var Skirnir = require('../Skirnir.js');
var colors = require('colors');

var skirnir = new Skirnir({dir: '/dev', autoscan: true, autoadd: true, baud: 115200});

function list_keys(object) {
  for(var i in object) {
    console.log(i.green);
  }
  
  console.log();
}

skirnir.on('add', function(e) {
  console.log('Added'.green + ' new serial device: ' + e.device.green + '. Currect devices are:');
  
  list_keys(skirnir.devices);
});

skirnir.on('remove', function(e) {
  console.log('Removed'.red + ' serial device: ' + e.device.green + '. Currect devices are:');
  
  list_keys(skirnir.devices);
});

skirnir.on('connect', function(e) {
  console.log('Connected'.green + ' device ' + e.device.green + '. Currect connections are:');
  
  list_keys(skirnir.connections);
});

skirnir.on('disconnect', function(e) {
  console.log('Disconnected'.red + ' device ' + e.device.green + '. Currect connections are:');
  
  list_keys(skirnir.connections);
});

skirnir.on('message', function(e) {
  console.log('Received '.yellow + String(e.data.length).magenta + ' byte buffer:'.yellow);
  console.log(new Buffer(e.data));
  
  var equal = new Buffer(e.data).equals(message_bin)
  
  console.log('Equal to last buffer sent: ' + (equal ? 'true'.green : 'false'.red) + '\n');
});

skirnir.on('error', function(e) {
  console.log('Error event from ' + e.call + ': ' + e.error);
});

var getRandomBuffer = function(length) {
  var result = [];
  
  for(var i = 0; i < length; ++i) {
    result.push(Math.floor(Math.random()*256));
  }
  
  return new Buffer(result);
}

var message_bin;
var message_size = 45;

setInterval(function() {
  message_bin = getRandomBuffer(message_size);
  
  console.log('Sending '.yellow + String(message_size).magenta + ' byte buffer:'.yellow);
  console.log(message_bin);
  console.log();
  skirnir.broadcast(message_bin);
  
  message_size = message_size === 45 ? 180 : 45;
}, 1500);

var cli = require('repl').start({});
cli.context.Skirnir = Skirnir;
cli.context.skirnir = skirnir;
