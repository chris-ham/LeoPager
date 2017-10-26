# LeoPager
Project that enables a web page interface running on a Raspberry Pi to send 433MHz pager messages using a LeoStick.<br />
The restaurant pagers that have been tested with this design are from AliExpress https://www.aliexpress.com/item/Guest-Coaster-Pager-System-for-order-taking-one-set-CTP320/32816821424.html?spm=a2g0s.9042311.0.0.RaPpa7<br />

The above pager 'sets' each have a unique (as far as I know) identifier ie 1010100000111011 this is followed by the ID of the individual unit LSB first so pager 1 is 100000000 pager 2 is 010000000 etc<br />

The LeoStick https://www.freetronics.com.au/products/leostick#.WfGg4miCwUE receives messages via its USB interface and transmits the pager message out of a software serial port.<br />

Messages can also be received when the LeoStick is put into the correct mode.<br />

Message format is all human readable ASCII ie ?61010100000111011010000000\n<br />
+first byte is the number of times to send the message data - ie 9 sends message 9 times, ? sends it 15 times<br />
+Second byte is the delay between messages<br />
+rest of message contains the actual message to send. This message is an ASCII representation of binary ie 011011001...<br />
So ?61010100000111011010000000\n sends 1010100000111011010000000 15 times with a delay of 6 to the 2nd pager in bank 1010100000111011<br />

Sending the serial message 'GetCode' to the LeoStick will result in the 433MHz codes received by the receiver being echoed out the LeoStick's USB serial port. This will show the received message as 1010100000111011010000000 or such repeated several times. There will be the odd short message received however that is to be expected. These received messages  can be used to work out the bank number of the pager.<br />
  
Sending the serial message 'SendCode' will result in the 433MHz code reception being turned off<br />
The default is to not echo received codes<br />

The pagers can also be put into a 'learn code' mode if needed. See the scanned pager user manual in this hub for info on how to put them into learn mode. 
