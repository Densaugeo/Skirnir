var Skirnir = require('../Skirnir.js');

var skirnir = new Skirnir({dir: '/dev', autoscan: true, autoadd: true});

function list_keys(object) {
  for(var i in object) {
    console.log(i);
  }
}

skirnir.on('add', function(e) {
  console.log('Added new serial device: ' + e.device + '. Currect devices are:');
  
  list_keys(skirnir.devices);
});

skirnir.on('remove', function(e) {
  console.log('Removed serial device: ' + e.device + '. Currect devices are:');
  
  list_keys(skirnir.devices);
});

skirnir.on('connect', function(e) {
  console.log('Connected device ' + e.device + '. Currect connections are:');
  
  list_keys(skirnir.connections);
});

skirnir.on('disconnect', function(e) {
  console.log('Disconnected device ' + e.device + '. Currect connections are:');
  
  list_keys(skirnir.connections);
});

skirnir.on('message', function(e) {
  console.log('Received buffer:');
  console.log(new Buffer(e.data));
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

setInterval(function() {
  var message_bin = getRandomBuffer(45);
  
  for(var i in skirnir.connections) {
    console.log('Sending buffer:');
    console.log(message_bin);
    skirnir.connections[i].send(message_bin);
  }
}, 1000);

var cli = require('repl').start({});
cli.context.Skirnir = Skirnir;
cli.context.skirnir = skirnir;
