# Skirnir.js

JS interface for Skirnir. Configures serial ports and carries packets

Dependencies: None

---

## Skirnir

Inherits: `EventEmitter`

Manages a folder of serial ports

```
var Skirnir = require('../Skirnir.js');
var skirnir = new Skirnir({dir: '/dev', autoscan: true, autoadd: true});

skirnir.on('add', console.log('Added device: ' + e.device)});
skirnir.on('connect', console.log('Connected to device: ' + e.device)});
skirnir.on('disconnect', console.log('Disconnected from device: ' + e.device)});
skirnir.on('remove', console.log('Removed device: ' + e.device)});
skirnir.on('error', function(e) {console.log(e.error)});

skirnir.on('message', function(e) {
  console.log('Received message from ' + e.device + ':');
  console.log(e.data.toString());
});

for(var i in skirnir.connections) {
  skirnir.connections[i].send(new Buffer([1, 2, 3, 4])); // Send [1, 2, 3, 4] to all connected devices
}
```

#### Options

`Boolean` **autoadd** -- Sets .autoadd

`Boolean` **autoscan** -- Sets .autoscan

`Number` **baud** -- Sets .baud

`String` **dir** -- Sets .dir

#### Properties

`Boolean` **autoadd** -- If true, automatically adds devices found in scans

`Boolean` **autoscan** -- If true, scans search directory every 5000ms

`Number` **baud** -- Baud rate for all serial connections. Defaults to 9600. Read-only

`{ChildProcess}` **connections** -- TTY ports with active Skirnir heartbeats, indexed by port name

`{ChildProcess}` **devices** -- Available TTY ports, indexed by port name

`String` **dir** -- Directory to look for ports in. Read-only

#### Methods

`undefined` proto **add**`(String device)` -- Spawn a child process that connects to the given port. Child is added to .devices

`undefined` proto **connect**`(String name)` -- Emit a connect event and add port to .connections

`undefined` proto **disconnect**`(String name)` -- Emit a disconnect event and remove port from .connections

`undefined` proto **scan**`()` -- Scan a directory for new ttyUSB or ttyACM devices. Emits scan and error events

#### Events

**add** `{device: String}` -- Notice of adding a new device, fired when a new device is added

**connect** `{device: String}` -- Notice of a connection, fired when a heartbeat is detected

**disconnect** `{device: String}` -- Notice of a disconnection, fired when heartbeat times out (7000ms)

**error** `{call: String, error: Error}` -- Notice of error. .call gives the name of the method of origin

**message** `{data: [Number], device: String}` -- Fired when a valid Skirnir packet is received. Always contains 45 bytes

**remove** `{device: String}` -- Notice of removing a device, fired when a device's io streams break

**scan** `{found: [String]}` -- Fired every time .scan is called, including autoscans. Lists new devices found by filename

