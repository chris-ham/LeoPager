/*
This Arduino App is designed to allow an external device to send (and receive) properly formatted
messages to bar style vibrating 433MHz pagers. The message payload is sent to the LeoStick as an RS232 string
via the arduino's USB serial port.

The reason for using a LeoStick in this way is to allow transmission of the pager data
without the waveform being corrupted by other embedded processes.

The 'pay load' is sent out of the Arduino's RS232 port after each 'bit' is converted
to an ASCII char that will give the correct vaveform for the vibrating pager to recognise. 
 
**Board
Arduino LeoStick 2560

**Pager banks
*WHITE  1010100000111011
*YELLOW 1001000000000111
*RED    0010111100011011
*BLUE   0010101100111011
*GREEN  1010010000111011

** .... message format
+first byte is the number of times to send the message data
+Second byte is the delay between messages
+rest of message contains the actual message to send. This message is binary ie 011011001...

Sending the serial message 'GetCode' to the LeoStick will result in the 433MHz codes received
  by the receiver being echoed out the USB serial port
Sending the serial message 'SendCode' will result in the 433MHz code reception being turned off
The default is to not echo received codes

*/


//Pager block
#include <SoftwareSerial.h>
String PagerMsg;
bool NewLine = true;

//set up the software serial port that will be used to transmit the 433Mhz
//codes to the pagers. A Jaycar ZW3100 is used as the RF module
//https://www.jaycar.co.nz/wireless-modules-transmitter-433mhz/p/ZW3100
//SoftwareSerial used so we can invert the polarity of the data being set to teh RF module
//   this saves having to use a hardware inverter.
//In the setup below we have Rx, Tx, Inv
//Pin 12 is connected to Rx
//Pin 11 is connected to the Transmitter
//Signal is inverted
#define TxIsOnPin 12
#define RxIsOnPin 11
#define Inverted 1
SoftwareSerial mySerial(RxIsOnPin, TxIsOnPin, Inverted);

int BitCount = 0;
byte one = 0x80;
byte zero = 0xFC;
String currentLine; 
int queryStringStart;
int queryStringEnd;

// pins for the LEDs:
const int redPin = 13;
const int greenPin = 9;
const int bluePin = 10;

int incomming = '.';
String received = "";
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
const long interval = 3500;
int Mode = 0;   //transmit

char inChar = '0';

//************************************************
void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  analogWrite(redPin, 127);
  analogWrite(greenPin, 0);
              
  // Open serial communications:
  Serial.begin(9600);
  Serial.println("Pager Transmiter");   
  
  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
}

//************************************************
//sends the data with the correct waveform out the serial port
//each 'bit' is represented by a 'zero' or 'one' character being
//sent out of the RS232 port. The reason for converting the 'zero' and 'one'
//is to emulate manchester encoding https://en.wikipedia.org/wiki/Manchester_code
//The LSB of the pager number is sent first
void sendData(String data){
  char ID = 0;
  int Count = data.length();
  for (int i=0; i<Count; i++){
    ID = data[i];
    if (ID == '0'){
      mySerial.write(zero);
    }else{
      mySerial.write(one);
    }
  }
}

//************************************************
//Sends the msg the requested number of times
//   with set delay between each data burst
void sendMsg(String toSend){
  int NumTimes = toSend[0] - '0';
  int aDelay = toSend[1] - '0';
  for (int i=0; i<NumTimes; i++){
    sendData(toSend.substring(2,27));
    delay(aDelay);
  }
}

//************************************************
//************************************************
//************************************************
void loop() {

  //+++++++++++++++++ deal with received data +++++++++++
  //get received 433MHz data and send it to the USB serial port
  //this will only happen when we have set the Arduino to read mode.
  if (mySerial.available() > 0){
    currentMillis = micros();
    if (currentMillis - previousMillis >= interval) {
      if ((BitCount == 24) && (Mode == 1)){
        Serial.println(received);
      }
      BitCount = 0;
      received = "";
    }else{
      BitCount ++;
    };
    previousMillis = currentMillis;

    incomming = mySerial.read();
    if (incomming == 0xFC){
      received = received + '0';
    };  
    if (incomming == 0x80){
      received = received + '1';
    }; 
  }

  //+++++++++++++++++ send data burst without interruption +++++++++++
  // if there's any serial available, read it
  //and transmit it when the time comes
  while (Serial.available() > 0) {
    inChar = Serial.read();

    //if we get \r set led to red to indicate error
    if (inChar == '\r'){
      Serial.println(currentLine);
      currentLine = "";      
    }
    
    //only transmit on \n
    if (inChar == '\n'){
      PagerMsg = currentLine;
      if (PagerMsg == "GetCode"){
        //turn on 433Mhz RX line and Tx line off
        analogWrite(greenPin, 127);
        analogWrite(redPin, 0);
        Mode = 1;  //receive  
      }else{
         if (PagerMsg == "SendCode"){
            //turn off 433MHz Rx line and Tx line on
            analogWrite(greenPin, 0);
            analogWrite(redPin, 127);
            Mode = 0; //transmit
          }else{
            sendMsg(PagerMsg);
            analogWrite(bluePin, 0);
          }
      }      
      Serial.println(currentLine);
      currentLine = "";
      analogWrite(bluePin, 0);
    }
    
    //only add character if it is not CR or LF
    if ((inChar != '\r') && (inChar != '\n')){
      currentLine += inChar;
    }
  }

}

