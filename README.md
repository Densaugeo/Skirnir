# Skirnir

Link Arduino to Nodejs, reliably. Can automatically connect to devices in a given folder and detect new devices. Linux-only, since it manages serial settings through stty.

[![License: LGPL](https://img.shields.io/badge/license-LGPL-blue.svg)](http://www.gnu.org/licenses/lgpl-3.0.en.html)
[![Build Status](https://travis-ci.org/Densaugeo/Skirnir.svg?branch=master)](https://travis-ci.org/Densaugeo/Skirnir)

## Installation

#### Nodejs

Run `npm install git://github.com/Densaugeo/Skirnir.git` or add git://github.com/Densaugeo/Skirnir.git to your package.json dependencies.

#### Arduino

Add contents of src/ to your project folder and `#include "Skirnir.hpp" in your main .cpp file.

## Usage

#### Nodejs

Minimal code to automatically connect to any device found in /dev, send [1, 2, 3], and log any response (messages will be padded with zeroes, because Skirnir packets always carry 45 bytes of data):

~~~
var Skirnir = require('skirnir');

var skirnir = new Skirnir({dir: '/dev', autoscan: true, autoadd: true});

setInterval(function() {
  for(var i in skirnir.connections) {
    skirnir.connections[i].send(new Buffer([1, 2, 3]));
  }
}, 1000);

skirnir.on('message', function(e) {
  console.log(new Buffer(e.data));
});
~~~

#### Arduino

The following minimal code can maintain a connection and echo received packets back to the PC:

~~~
#include "Skirnir.hpp"

unsigned char packet_decoded[45];
unsigned long last_ping = 0;

Skirnir a_skirnir = Skirnir(&Serial);

void setup() {
  Serial.begin(9600);
}

void loop() {
  // Check timer for overflow
  if(millis() < last_ping) {
    last_ping = 0;
  }
  
  // Start heartbeat every 5s
  if(millis() > last_ping + 5000) {
    last_ping = millis();
    
    a_skirnir.heartbeat();
  }
  
  // Returns true if a valid packet is found
  if(a_skirnir.receive_until_packet(packet_decoded)) {
    // Echo packet back to PC
    a_skirnir.send45(packet_decoded);
  }
}
~~~

#### Larger packets

Normally, Skirnir sends data in 45-byte packets, and ignores extra data if trying to .send() more than 45 bytes. The Skirnir180 class works the same as Skirnir, but is also able to send 180-byte packets, at the cost of a couple hundred bytes of memory. Skirnir180::send() will still send smaller payloads in the same 45-byte packets as Skirnir::send().

## License

LGPL
