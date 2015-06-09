# IoT-Kit
Kit for hSenid Mobile Telco Application Platform's IOT API

## Installation Guide

### Prerequisites
#### Hardware

* Arduino Mega 2560 Board
* SIM900A GSM Shield
* SMPS Power Adapter for Arduino (9V 2A)
* Dialog SIM

Follow [this link](https://github.com/hsenid-mobile/iot-kit/wiki/How-to-connect-SIM900A) to connect the board with SIM900A GSM Shield  


#### Software

* Install [Arduino IDE](http://www.arduino.cc/en/main/software) in your computer

### Step 1.
git clone https://github.com/hsenid-mobile/iot-kit.git


### Step 2. 
Open the iot-kit.ino file using the Arduino IDE


### Step 3. 
Inside Arduino IDE
go to Sketch -> Include Library -> Add .ZIP Library and add all files inside iot-kit/deps

### Step 4.
Connect your arduino board to the computer

### Step 5.
Make sure you have selected the correct board and port in the IDE
(to check this go to the tools/board and tools/port tabs in the IDE)

### Step 6.
Load the code to the arduino board using the IDE.

### Step 7.
Disconnect the arduino from the computer, connect the sensors and actuators and the gsm shield, and finally connect the power supply.

Now your arduino is ready to run an application.

Please follow the [developer-guide](https://github.com/hsenid-mobile/iot-kit/blob/master/developer-guide.md) to develop your application.

