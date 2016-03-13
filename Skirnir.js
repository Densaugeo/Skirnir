/**
 * @description JS interface for Skirnir. Configures serial ports and carries packets
 */
var cp = require('child_process');
var EventEmitter = require('events');
var fs = require('fs');
var path = require('path');
var util = require('util');

/**
 * @module Skirnir inherits EventEmitter
 * @description Manages a folder of serial ports
 * 
 * @example var Skirnir = require('../Skirnir.js');
 * @example var skirnir = new Skirnir({dir: '/dev', autoscan: true, autoadd: true});
 * @example
 * @example skirnir.on('add', console.log('Added device: ' + e.device)});
 * @example skirnir.on('connect', console.log('Connected to device: ' + e.device)});
 * @example skirnir.on('disconnect', console.log('Disconnected from device: ' + e.device)});
 * @example skirnir.on('remove', console.log('Removed device: ' + e.device)});
 * @example skirnir.on('error', function(e) {console.log(e.error)});
 * @example
 * @example skirnir.on('message', function(e) {
 * @example   console.log('Received message from ' + e.device + ':');
 * @example   console.log(e.data.toString());
 * @example });
 * @example
 * @example for(var i in skirnir.connections) {
 * @example   skirnir.connections[i].send(new Buffer([1, 2, 3, 4])); // Send [1, 2, 3, 4] to all connected devices
 * @example }
 */
var Skirnir = function(args) {
  var self = this;
  
  EventEmitter.call(this);
  
  if(typeof args.dir !== 'string') {
    throw new Error('Error at Skirnir(): args.dir must have type \'string\'');
  }
  
  // @prop String dir -- Directory to look for ports in. Read-only
  // @option String dir -- Sets .dir
  Object.defineProperty(this, 'dir', {value: args.dir, enumerable: true});
  
  // @prop Number baud -- Baud rate for all serial connections. Defaults to 9600. Read-only
  // @option Number baud -- Sets .baud
  Object.defineProperty(this, 'baud', {value: args.baud || 9600, enumerable: true});
  
  // @prop Boolean autoscan -- If true, scans search directory every 5000ms
  // @option Boolean autoscan -- Sets .autoscan
  this.autoscan = Boolean(args.autoscan);
  
  // @prop Boolean autoadd -- If true, automatically adds devices found in scans
  // @option Boolean autoadd -- Sets .autoadd
  this.autoadd = Boolean(args.autoadd);
  
  // @prop {ChildProcess} devices -- Available TTY ports, indexed by port name
  this.devices = {};
  
  // @prop {ChildProcess} connections -- TTY ports with active Skirnir heartbeats, indexed by port name
  this.connections = {};
  
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

// @method proto undefined connect(String name) -- Emit a connect event and add port to .connections
// @event connect {device: String} -- Notice of a connection, fired when a heartbeat is detected
Skirnir.prototype.connect = function(name) {
  if(this.connections[name] === undefined) {
    this.connections[name] = this.devices[name];
    this.emit('connect', {device: name});
  }
}

// @method proto undefined disconnect(String name) -- Emit a disconnect event and remove port from .connections
// @event disconnect {device: String} -- Notice of a disconnection, fired when heartbeat times out (7000ms)
Skirnir.prototype.disconnect = function(name) {
  if(this.connections[name] !== undefined) {
    delete this.connections[name];
    this.emit('disconnect', {device: name});
  }
}

// @method proto undefined add(String device) -- Spawn a child process that connects to the given port. Child is added to .devices
// @event add {device: String} -- Notice of adding a new device, fired when a new device is added
// @event remove {device: String} -- Notice of removing a device, fired when a device's io streams break
// @event message {data: [Number], device: String} -- Fired when a valid Skirnir packet is received. Always contains 45 bytes
Skirnir.prototype.add = function(device) {
  var serial_child = cp.fork(__dirname + '/Skirnir_connection.js', [path.join(this.dir, device), this.baud]);
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

// @method proto undefined scan() -- Scan a directory for new ttyUSB or ttyACM devices. Emits scan and error events
// @event error {call: String, error: Error} -- Notice of error. .call gives the name of the method of origin
// @event scan {found: [String]} -- Fired every time .scan is called, including autoscans. Lists new devices found by filename
Skirnir.prototype.scan = function() {
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
