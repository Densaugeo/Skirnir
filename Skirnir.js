var cp = require('child_process');
var EventEmitter = require('events');
var fs = require('fs');
var path = require('path');
var util = require('util');

// Skirnir({dir: String})
var Skirnir = function(args) {
  if(typeof args.dir !== 'string') {
    throw new Error('Error at Skirnir(): args.dir must have type \'string\'');
  }
  
  var self = this;
  
  EventEmitter.call(this);
  
  // @prop {ChildProcess} devices -- Available TTY ports, indexed by port name
  this.devices = {};
  
  // @prop {ChildProcess} connections -- TTY ports with active Skirnir heartbeats, indexed by port name
  this.connections = {};
  
  // @prop String dir -- Directory to look for ports in
  Object.defineProperty(this, 'dir', {value: args.dir, enumerable: true});
  
  // @prop Boolean autoscan -- If true, scans search directory every 5000ms
  this.autoscan = Boolean(args.autoscan);
  
  // @prop Boolean autoadd -- If true, automatically adds devices found in scans
  this.autoadd = Boolean(args.autoadd);
  
  setInterval(function() {
    if(args.autoscan) {
      self.scan();
    }
  }, 5000);
  
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

// @method undefined connect(String name, ChildProcess process) -- Emit a connect event and add port to .connections
Skirnir.prototype.connect = function(name) {
  if(this.connections[name] === undefined) {
    this.connections[name] = this.devices[name];
    this.emit('connect', {device: name});
  }
}

// @method undefined disconnect(String name) -- Emit a disconnect event and remove port from .connections
Skirnir.prototype.disconnect = function(name) {
  if(this.connections[name] !== undefined) {
    delete this.connections[name];
    this.emit('disconnect', {device: name});
  }
}

// @method undefined add(String device) -- Spawn a child process that connects to the given port. Child is added to .devices
Skirnir.prototype.add = function(device) {
  var serial_child = cp.fork('./Skirnir_connection.js', [path.join(this.dir, device)]);
  this.devices[device] = serial_child;
  this.emit('add', {device: device});
  
  var self = this;
  
  serial_child.once('exit', function() {
    self.disconnect(device);
    delete self.devices[device];
    self.emit('remove', {device: device});
  });
  
  serial_child.on('message', function(e) {
    switch(e.type) {
      case 'Buffer'    : self.emit('message', {data: e.data, device: device}); break;
      case 'connect'   : self.connect(device); break;
      case 'disconnect': self.disconnect(device); break;
    }
  });
}

// @method undefined scan(Function cb) -- Scan a directory for new ttyUSB or ttyACM devices. cb is called with cb([String] devices_names)
Skirnir.prototype.scan = function(cb) {
  var self = this;
  
  fs.readdir(this.dir, function(error, files) {
    if(error) {
      self.emit('error', {call: 'Skirnir.prototype.scan()', error: error});
      return;
    }
    
    var result = [];
    
    files.forEach(function(v) {
      if(v.substring(0, 6) === 'ttyUSB' || v.substring(0, 6) === 'ttyACM') {
        if(self.devices[v] === undefined) {
          result.push(v);
        }
      }
    });
    
    self.emit('scan', {found: result});
    
    if(self.autoadd) {
      result.forEach(function(v) {
        self.add(v);
      });
    }
  });
}

module.exports = Skirnir;
