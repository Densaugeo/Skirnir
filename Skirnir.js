var cp = require('child_process');
var EventEmitter = require('events');
var fs = require('fs');
var path = require('path');
var util = require('util');

var Skirnir = function() {
  var self = this;
  
  EventEmitter.call(this);
  
  // @prop {ChildProcess} devices -- Available TTY ports, indexed by port name
  this.devices = {};
  
  // @prop {ChildProcess} connections -- TTY ports with active Skirnir heartbeats, indexed by port name
  this.connections = {};
  
  // Try to prevent dangling child processes. This should be refactored to not use children
  var cleanup = function() {
    for(var i in self.devices) {
      self.devices[i].kill();
    }
  }
  
  process.once('exit', cleanup);
  process.once('SIGTERM', cleanup);
  process.once('SIGINT', cleanup);
  process.once('SIGHUP', cleanup);
  process.once('SIGBREAK', cleanup);
}
util.inherits(Skirnir, EventEmitter);

// @method undefined open(String name, ChildProcess process) -- Emit an open event and add port to .connections
Skirnir.prototype.open = function(name) {
  if(this.connections[name] === undefined) {
    this.connections[name] = this.devices[name];
    this.emit('open', {device: name});
  }
}

// @method undefined close(String name) -- Emit a close event and remove port from .connections
Skirnir.prototype.close = function(name) {
  if(this.connections[name] !== undefined) {
    delete this.connections[name];
    this.emit('close', {device: name});
  }
}

// @method undefined connect(String device) -- Spawn a child process that connects to the given port. Child is added to .devices
Skirnir.prototype.connect = function(device) {
  var serial_child = cp.fork('./Skirnir_connection.js', [device]);
  this.devices[device] = serial_child;
  this.emit('new', {device: device});
  
  var self = this;
  
  serial_child.on('exit', function() {
    self.close(device);
    delete self.devices[device];
    self.emit('exit', {device: device});
  });
  
  serial_child.on('message', function(e) {
    switch(e.type) {
      case 'Buffer': self.emit('message', {data: e.data, device: device}); break;
      case 'open'  : self.open(device); break;
      case 'close' : self.close(device); break;
    }
  });
}

// @method undefined scan(String dir, Function cb) -- Scan a directory for new ttyUSB or ttyACM devices. cb is called with cb(Error err, [String] devices_names)
Skirnir.prototype.scan = function(dir, cb) {
  var self = this;
  
  fs.readdir(dir, function(err, files) {
    if(err) {
      cb(err, null);
      return;
    }
    
    var result = [];
    
    files.forEach(function(v) {
      if(v.substring(0, 6) === 'ttyUSB' || v.substring(0, 6) === 'ttyACM') {
        if(self.devices[path.join(dir, v)] === undefined) {
          result.push(path.join(dir, v));
        }
      }
    });
    
    cb(null, result);
  });
}

// @method undefined scanAndConnect(String dir, Function cb) -- Scan a directory and connect to new ttyUSB or ttyACM devices. cb is called with cb(Error err, [String] devices_names)
Skirnir.prototype.scanAndConnect = function(dir, cb) {
  var self = this;
  
  this.scan(dir, function(err, devices) {
    if(err) {
      cb(err, null);
      return;
    }
    
    devices.forEach(function(v) {
      self.connect(v);
    });
    
    cb(null, devices);
  });
  
}

// @method undefined .autoConnect(String dir, Function cb) -- Automatically connect to new devices. Cb is only called if there is an error
Skirnir.prototype.autoConnect = function(dir, cb) {
  var self = this;
  
  this.scanAndConnect(dir, function(err) {
    if(err) {
      cb(err);
    }
  });
  
  setInterval(function() {
    self.scanAndConnect(dir, function(err) {
      if(err) {
        cb(err);
      }
    });
  }, 5000);
};

module.exports = Skirnir;
