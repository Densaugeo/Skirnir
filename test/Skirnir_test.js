var Skirnir = require('../Skirnir.js');

var skirnir = new Skirnir();

skirnir.autoConnect('/dev', function(err) {
  if(err) {
    console.log('Error scanning /dev: ' + err);
  }
});

skirnir.on('new', function(e) {
  console.log('Added new serial device: ' + e.device);
});

skirnir.on('message', function(e) {
  console.log('Received buffer:');
  console.log(new Buffer(e.data));
});

skirnir.on('open', function(e) {
  console.log('Opened device ' + e.device + '. Currect connections are:');
  
  for(var i in skirnir.connections) {
    console.log(i);
  }
});

skirnir.on('close', function(e) {
  console.log('Closed device ' + e.device + '. Currect connections are:');
  console.log(skirnir.connections);
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
