var fs = require('fs');
var cp = require('child_process');

var device = process.argv[2];
var baud = process.argv[3];

// Serial settings

// Serial port must have icanon flag
// Turns echo off on serial port - they echo the Arduino's output back to itself
// -icrnl and -onlcr to keep \n as just \n

var serial_settings = ['sane', baud, '-echo', '-echoe', '-echok', '-echoctl', '-echoke', '-icrnl', '-onlcr'];

serial_settings.forEach(function(v) {
  var stty_result = cp.spawnSync('stty', ['--file=' + device, v]);
  
  if(stty_result.error) {
    console.log('Error: could not call stty for ' + device + '. Make sure stty is installed.');
    process.exit(1);
  }
  
  if(stty_result.stderr.length > 0) {
    console.log('Warning: \'stty --file=' + device + ' ' + v + '\' failed: ' + stty_result.stderr.toString());
    
    if(v === 'sane') {
      console.log('Error: cannot proceed without stty sane');
      process.exit(1);
    }
  }
});

var reader = fs.createReadStream(device);
var writer = fs.createWriteStream(device);

var Heartbeat = function() {
  var active = false;
  Object.defineProperty(this, 'active', {
    get: function() {
      return active;
    },
    set: function(v) {
      if(active !== v) {
        active = Boolean(v);
        process.send({type: active ? 'connect' : 'disconnect'}, function(err) {
          if(err) {
            console.log('This may be that random IPC error:');
            console.log(err);
          }
        });
      }
    }
  });
  
  this.timer = {};
}
var heartbeat = new Heartbeat();

process.on('message', function(message) {
  if(message.data.length > 180) {
    throw new Error('Message too large to send, must be 180 bytes or less');
  }
  
  var base64 = new Buffer(message.data).toString('base64');
  var base64_size = message.data.length <= 45 ? 60 : 240;
  
  // Pad to 60 bytes because serial packets must all be the same size
  while(base64.length < base64_size) {
    base64 += 'A';
  }
  
  var prefix = message.data.length <= 45 ? '#' : '&';
  
  writer.write(prefix + base64 + '\n', 'utf-8');
});

reader.on('data', function(data) {
  // Check for heartbeat ping. All heartbeats have length 2
  if(data.length === 2) {
    switch(data.toString()) {
      case '<\n': // Reply with '-' to continue heartbeat
        writer.write('-\n', 'utf-8');
        break;
      case '>\n': // Heartbeat complete
        heartbeat.active = true;
        
        clearTimeout(heartbeat.timer);
        
        heartbeat.timer = setTimeout(function() {
          heartbeat.active = false;
        }, 7000);
        break;
    }
  } else if(heartbeat.active) {
    var serial_packet = data.toString();
    
    // If packet is a valid message packet...
    var expected_length = 0;
    switch(serial_packet[0]) {
      case '#': expected_length = 62 ; break;
      case '&': expected_length = 242; break;
      default: return;
    }
    
    if(serial_packet.length === expected_length && serial_packet[expected_length - 1] === '\n') {
      process.send(new Buffer(serial_packet.substring(1, expected_length - 1), 'base64'), function(err) {
        if(err !== null) {
          console.log('Error trying to send to parent process:');
          console.log(err);
        }
      });
    }
  }
});

reader.on('close', function() {
  console.log('Serial port ' + device + ' closed.');
  process.exit(1);
});
