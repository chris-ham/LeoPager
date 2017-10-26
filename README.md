# LeoPager
Project that enables a web page interface running on a Raspberry Pi to send 433MHz pager messages using a LeoStick.

The LeoStick https://www.freetronics.com.au/products/leostick#.WfGg4miCwUE receives messages via its USB interface and transmits the pager message out of a software serial port.

Messages can also be received when the LeoStick is put into the correct mode.

message format is all human readable ASCII
+first byte is the number of times to send the message data - ie 9 sends message 9 times
+Second byte is the delay between messages
+rest of message contains the actual message to send. This message is binary ie 011011001...

Sending the serial message 'GetCode' to the LeoStick will result in the 433MHz codes received
  by the receiver being echoed out the USB serial port
Sending the serial message 'SendCode' will result in the 433MHz code reception being turned off
The default is to not echo received codes
