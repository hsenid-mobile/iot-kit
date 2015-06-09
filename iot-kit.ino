#include <bitlash.h>
#include <sim900.h>
#include <GPRS_Shield_Arduino.h>

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
  #define serialPrintByte(b) Serial.write(b)
#else
  #define serialPrintByte(b) Serial.print(b,BYTE)
#endif

// APN Settings
#define GPRS_APN   "dialogbb"

#define BAUDRATE  9600

GPRS gprs(BAUDRATE);

char GATEWAY_HOST[] = "api.hsenidmobile.com";
int  GATEWAY_PORT = 9010;

String poll_cmd = "Poll";

String OK = "Ok";
String Ntfy = "Ntfy:";

volatile bool gprs_ready = false;
volatile bool gprs_connected = false;
volatile bool device_registered = false;

char* notification_data;

char* saved = "saved";
char* stop_process = "stop *;";
char* rm_process = "rm schedule;";
                                       
int max_ntfy_attempts = 3;
int notification_retry_count = 0;

String return_data="";

char send_buffer [256];
char receive_buffer [256];

void serialHandler(byte b) {
  if (b == 13 ){
    if (return_data.equals("saved")){
      return_data = "";       
    }
    else {
      char charBuf[200];                              
      return_data.toCharArray(charBuf,200);
      notification_data = charBuf;
      Serial.println(F("Preparing to send the notification"));
      send_notification();

      return_data = "";
    }   
  }
  else {
    if (b == 10){
      //
    }
    else{
      return_data.concat((char)b);       
    }  
  }
}

void setup() {
  initBitlash(9600);
  Serial1.begin(9600);
  Serial2.begin(9600); 
  Serial3.begin(9600);
  addBitlashFunction("sw", (bitlash_function) sw);
  addBitlashFunction("sr", (bitlash_function) sr);
  setOutputHandler(&serialHandler);
  Serial.println("Booting the Device");
  setupIfGPRSNotReady();
}

void loop() {
  runBitlash();   

  if (!gprs_ready) {
    if (!setupIfGPRSNotReady()) {
      return;
    }  
  }     
    
  if (!gprs_connected) {
    if(!connectGateway()) {
       return;
    } 
  } 
      
  if (!device_registered) {
      String boot_cmd = get_boot_cmd();
      Serial.println(boot_cmd);
      write_message(boot_cmd);
      String response = read_message();
      Serial.println(response);
      device_registered = true;
  } else {
      Serial.println(poll_cmd);   
      write_message(poll_cmd);
      String response = read_message();
      Serial.println(response);
      
      if (response.startsWith("Ok")) {
        Serial.println("No pending command");
      } else if(response.startsWith("Cmd")) {
        Serial.println("New command received");
        execute_instructions(response);
      } else {
        Serial.println("Unexpected message received, reconnecting");
        gprs_connected = false;
      } 
  }

}

String read_message(){
  int attempts = 0;
  
  while(gprs.readable() <= 0 && attempts < 3) {
   delay(200); 
   attempts++;
  }  
  
  int ret = gprs.recv(receive_buffer, sizeof(receive_buffer)-1);

  receive_buffer[ret] = '\0';
  
  if(attempts > 3 || ret <= 0) {
    gprs.close();
    gprs_connected = false;
    
    return "";
  } else {
     String data = String(receive_buffer);
     int len = data.length();
     
     if (data.endsWith("#")) {
       data.remove(len - 1);
       return data;
     } else {
       return data;
     }
  }     
}

void write_message(String message){
  uint8_t len = message.length();
  
  for(int i = 0; i < len; i++) {
    send_buffer[i] = message.charAt(i);
  }  

  send_buffer[len] = '#';
  send_buffer[len+1] = '\0';
  
  gprs.send(send_buffer, len + 1);
}


bool setupIfGPRSNotReady(){
  Serial.println(F("Setting up GPRS"));
  
  while(!gprs.init()) {
      delay(1000);
      Serial.println(F("GPRS init error, Retrying"));
  }

  delay(3000);  
 
  while(!gprs.join(F(GPRS_APN))) {
      Serial.println(F("GPRS attach error, Retrying"));
      delay(2000);
  }

  gprs_ready = true;

  Serial.println(F("GPRS attach success"));
 
  Serial.print(F("IP Address is : "));
  Serial.println(gprs.getIPAddress());

  return gprs_ready;  
}

bool connectGateway() {
  Serial.println(F("Connecting to IoT Gateway..."));
  
  gprs_connected = false;
  device_registered = false;
  
  int attempts = 0;
          
          
  while((gprs_connected = gprs.connect(TCP,GATEWAY_HOST, GATEWAY_PORT)) == false && attempts < 3) {
     attempts ++;
     delay(3000);
  }  
  
  if (gprs_connected){
    Serial.println(F("Connected to IoT Gateway"));
  } else{
    Serial.println(F("Failed to connect. will retry later ..."));
    gprs.disconnect();
    gprs_ready = false;
  }
  
  return gprs_connected;
}  


void send_notification(){
  String notification = Ntfy;
  notification.concat(notification_data);
  Serial.println(notification);
   
  write_message(notification);
  String response = read_message();
  
  if (response.startsWith("Ok")){
    notification_data = "";  
    notification_retry_count = 0;  
  } else if (notification_retry_count < max_ntfy_attempts){
    delay(1000);
    notification_retry_count +=1;
    send_notification();
  }
}

void execute_instructions(String instruction){
  instruction.replace("'", "\"");
   
  Serial.println(instruction);
  
  String cmd =  split_str(instruction, ':', 1);
 
  if (cmd.startsWith("function")) {
    if (cmd.startsWith("function schedule")) {
       char command_data[256];                              
       cmd.toCharArray(command_data, cmd.length() + 1);
    
       doCommand(stop_process);
       doCommand(rm_process);
    
       doCommand(command_data);
       doCommand("run schedule;");
       Serial.println("Starting schedule command");
    } else if (cmd.startsWith("function startup")) {
       char command_data[256];                              
       cmd.toCharArray(command_data, cmd.length() + 1);
    
       doCommand(command_data);
       doCommand("run boot;");
       Serial.println("Executing startup command");   
    } else {
       notification_data = "Unsupported function format";
       Serial.println(notification_data);
       send_notification();  
    }   
  } else { 
    Serial.println("Running do command..");    
    char command_data[cmd.length()];
    
    cmd.toCharArray(command_data, cmd.length() + 1);
    doCommand(command_data);
 } 
}

String get_boot_cmd() {
  char imei[32];
  String boot_cmd = "Boot:";
  
  sim900_send_cmd("AT+GSN\r\n");
  sim900_clean_buffer(imei,32);
  sim900_read_buffer(imei,32);

  for(int i=0; i < 32; i++) {
    char c = imei[i];
    if (c == 0x4F || c == 0x45) break;
    
    if ( c > 0x2F && c < 0x3A) {
      boot_cmd.concat(c);
    }  
  }  
   
  return boot_cmd; 
}  


void sw(){
   int serial_port = getarg(1);
   int value =  getarg(2);
  
   if (serial_port==1){
     Serial1.write(value);
   }else if (serial_port==2){
     Serial2.write(value);
   }else if (serial_port==3){
     Serial3.write(value);
   }else{
     doCommand("print \"Invalid Serial Port\"");
   }  
}

numvar sr(){
   int incomingByte = 0;                      
   int serial_port = getarg(1);
      
   if (serial_port==1){
      if (Serial1.available() > 0) {
         incomingByte = Serial1.read();
      }
   }else if (serial_port==2){
      if (Serial2.available() > 0) {
         incomingByte = Serial2.read();
      }
   }else if (serial_port==3){
      if (Serial3.available() > 0) {
         incomingByte = Serial3.read();
      }
   }else{
         incomingByte = -1;
   }   
   return incomingByte; 
}


String split_str(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
      found++;
      strIndex[0] = strIndex[1]+1;
      strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}


