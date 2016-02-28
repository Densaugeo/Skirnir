var Skirnir = require('../Skirnir.js');

var serially = new Skirnir();

serially.autoConnect('/dev', function(err) {
  if(err) {
    console.log('Error scanning /dev: ' + err);
  }
});

var last_message = '';

serially.on('new', function(e) {
  console.log('Added new serial device: ' + e.device);
});

serially.on('message', function(e) {
  console.log('Received buffer:');
  console.log(Buffer(e.data));
});

serially.on('open', function(e) {
  console.log('Opened device ' + e.device + '. Currect connections are:');
  for(var i in serially.connections) console.log(i);
});

serially.on('close', function(e) {
  console.log('Closed device ' + e.device + '. Currect connections are:');
  console.log(serially.connections);
});

var getRandomBuffer = function(length) {
  var result = [];
  
  for(var i = 0; i < length; ++i) {
    result.push(Math.floor(Math.random()*256));
  }
  
  return Buffer(result);
}

setInterval(function() {
  var message_bin = getRandomBuffer(45);
  
  for(var i in serially.connections) {
    console.log('Sending buffer:');
    console.log(message_bin);
    serially.connections[i].send(message_bin);
  }
}, 1000);

var cli = require('repl').start({});
