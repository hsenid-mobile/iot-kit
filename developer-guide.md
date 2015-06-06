#IoT Kit Developer Guide


### Prerequisites

Setup IoT-Kit as mentioned in [README](https://github.com/hsenid-mobile/iot-kit/README.md)

### IoT API

IoT API can be used by telco applications to send 'programmable commands' to the IoT devices(IoT-Kit installed arduinos) 
and receive notifications when the commands executes, through a WebSocket connection.

IoT API's programmable commands are defined in [Bitlash](http://bitlash.net/) format.


### Application Connection URL

IoT Application can open a WebSocket connection to following URL 

```
ws://api.hsenidmobile.com:9008/iot/connect?token=<your application's token>
```

Application will be authenticated by the token given in the URL (You can get the token from devspace application detail email).
 
### Message Types

IoT application can program/instruct the connected arduinos to control actuators/read sensors 
by sending command messages and receive their output via notification messages.

Following Message Types are defined in the API

#### Boot Message Format
-------------------
This message is sent by the device to application, when device starts.
Purpose of this message is to identify the device and it's name. 


```json
{ "type"      : "boot",
  "device"    : "imei:866762024216218"}
```

parameter descriptions

```
'type'     : boot type
'device'   : mobile number /masked mobile number / imei number of the device
``` 

please note that, you have to use the received 'device' value to send a command back.


#### Command Message Format
----------------------
This message is sent by application to device, when application wants to load a programmable command

For example: 

Assume that, the device is connected to sensor on analog pin0 
and we want to get a notification when pin0 value goes to < 200

Such a program can be written in Bitlash like following

```c
"function schedule {if(a0 < 200){print 'level=' a0;};}"
```

Here print function is used to collect the output and send back as notification message.
If you don't need notifications, you can skip using the print function.


To load this program to our "Coffee Machine 001", we have to send command message like following

```json
{ "type"    : "command",
  "content" : "function schedule {if(a0 < 200){print 'level=' a0;};};",
  "device"  : "imei:866762024216218" }
```
For more details, Please check [Bitlash Commands](https://github.com/billroy/bitlash/wiki/commands)

```
'type'     : command type
'content'  : Bitlash program
'device'   : mobile number /masked mobile number / imei number of the device
```

#### Status Message Format
----------------------
This message is sent by device to application, when the command is loaded successfully in the device

```json
{ "type"    : "status",
  "content" : "S1000",
  "device"  : "imei:866762024216218"}
```

```
'type'     : status type
'content'  : status code of the command
'device'   : mobile number /masked mobile number / imei number of the device
```

#### Notification Message Format
---------------------------
This message is sent by device to application, when the program print it's output

```json
{ "type"    : "notification",
  "content" : "level=160",
  "device"  : "imei:866762024216218"}
```

```
'type'     : notification type
'content'  : output of the command
'device'   : mobile number /masked mobile number / imei number of the device
```


### Sample Node.js Code

Please check the [IoT Sample Application](https://github.com/hsenid-mobile/iot-app)


### Sample Websocket Console output

You can install [wscat](https://www.npmjs.com/package/wscat) and control the device for fun.

```

$ wscat -c ws://api.hsenidmobile.com:9008/iot/connect?token=<your iot token> -p 13

< {"device":"imei:866762024216218","type":"boot"}

> {"type":"command","content":"function schedule { if(a0 > 300){print 'level=' a0;};}","device":"imei:866762024216218"}

< {"content":"level=475","device":"imei:866762024216218","type":"notification"}

< {"content":"level=475","device":"imei:866762024216218","type":"notification"}

< {"content":"level=298","device":"imei:866762024216218","type":"notification"}

< {"content":"level=318","device":"imei:866762024216218","type":"notification"}

< {"content":"level=320","device":"imei:866762024216218","type":"notification"}

< {"content":"level=329","device":"imei:866762024216218","type":"notification"}

< {"content":"level=346","device":"imei:866762024216218","type":"notification"}

< {"content":"level=307","device":"imei:866762024216218","type":"notification"}

```
 
 
 
 
 
 
 
 
 
 
