/*
Bluetooth HM01 Demo Code
2014 Copyright (c) Seeed Technology Inc.  All right reserved.
Author: Jacky Zhang
This demo code is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.
This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
For more details about the product please check http://www.seeedstudio.com/depot/
*/

/* Upload this sketch into Arduino Uno and press reset*/

#define __MEGA2560__ 1					//0: Uno, 1: Mega 2560
#define MASTER 0    					//change this macro to define the Bluetooth as Master or not 
#define ATCOMMAND_NAME_AUTH	0			//change module authentication and name
#define NLCR 0                          //module type requires NL and CR
#define BAUDRATE 9600

#if __MEGA2560__
	#define RxD 15
	#define TxD 14	
	#define blueToothSerial Serial3		//the software serial port
#else
	#include <SoftwareSerial.h>   		//Software Serial Port
	#define RxD 2
	#define TxD 3
	SoftwareSerial blueToothSerial(RxD,TxD);	//the software serial port 
#endif

int BOARD_LED = 13 ;              		// define LED Interface

char recv_str[100];

void setup() 
{
    Serial.begin(BAUDRATE);   			//Serial port for debugging
    blueToothSerial.begin(BAUDRATE);    //BT module baud rate
    Serial.println("\r\nPower on!!");
    setupBlueToothConnection(); 		//initialize Bluetooth
    //this block is waiting for connection was established.
    while(1)
    {
        if(recvMsg(100) == 0)
        {
            if(strcmp((char *)recv_str, (char *)"OK+CONN") == 0)
            {
                Serial.println("connected\r\n");
                break;
            }
        }
        delay(200);
    }
} 

void loop() 
{
    #if MASTER  //central role
    //in master mode, the bluetooth send message periodically. 
    delay(400);
    Serial.println("send: hi");
    blueToothSerial.print("hi");
    delay(100);
    //get any message to print
    if(recvMsg(1000) == 0)
    {
        Serial.print("recv: ");
        Serial.print((char *)recv_str);
        Serial.println("");
    }
    #else   //peripheral role
    delay(200);
    //the slave role only send message when received one.
    if(recvMsg(1000) == 0)
    {
        Serial.print("recv: ");
        Serial.print((char *)recv_str);
        Serial.println("");
        Serial.println("send: ACK");
        blueToothSerial.print("ACK");	//return back message
    }
    #endif
}

//used for compare two string, return 0 if one equals to each other
int strcmp(char *a, char *b)
{
    unsigned int ptr = 0;
    while(a[ptr] != '\0')
    {
        if(a[ptr] != b[ptr]) return -1;
        ptr++;
    }
    return 0;
}

//configure the Bluetooth through AT commands
int setupBlueToothConnection()
{
    #if MASTER
    Serial.println("this is MASTER\r\n");
    #else
    Serial.println("this is SLAVE\r\n");
    #endif

    Serial.print("Setting up Bluetooth link\r\n");
    delay(2000);//wait for module restart

    //wait until Bluetooth was found
    while(1)
    {
        if(sendBlueToothCommand("AT") == 0)
        {
            if(strcmp((char *)recv_str, (char *)"OK") == 0)
            {
                Serial.println("Bluetooth exists\r\n");
                break;
            }
        }
        delay(500);
    }

    //configure the Bluetooth
    sendBlueToothCommand("AT+DEFAULT");			//restore factory configurations, what parameters?
    delay(2000);

    //set role according to the macro
    sendBlueToothCommand("AT+NAME?");

    #if MASTER
    sendBlueToothCommand("AT+ROLEM");			//set to master mode
    #else
    sendBlueToothCommand("AT+ROLES");			//set to slave mode
    #endif
    sendBlueToothCommand("AT+RESTART");			//restart module to take effect

	#if ATCOMMAND_NAME_AUTH
    sendBlueToothCommand("AT+AUTH1");			//enable authentication
    sendBlueToothCommand("AT+PASS123456");		//set password
    sendBlueToothCommand("AT+NOTI1");			//set password	
	sendBlueToothCommand("AT+NAMEZETAHM-10");
	#endif
	
    delay(2000);//wait for module restart

    //check if the Bluetooth always exists
    if(sendBlueToothCommand("AT") == 0)
    {
        if(strcmp((char *)recv_str, (char *)"OK") == 0)
        {
            Serial.print("Setup complete\r\n\r\n");
            return 0;;
        }
    }

    return -1;
}

//send command to Bluetooth and return if there is a response received
int sendBlueToothCommand(char command[])
{
    Serial.print("send: ");
    Serial.print(command);
    Serial.println("");
#if NLCR
    blueToothSerial.println(command);
#else
    blueToothSerial.print(command);
#endif    
    delay(300);

    if(recvMsg(200) != 0) return -1;

    Serial.print("recv: ");
    Serial.print(recv_str);
    Serial.println("");
    return 0;
}

//receive message from Bluetooth with time out
int recvMsg(unsigned int timeout)
{
    //wait for feedback
    unsigned int time = 0;
    unsigned char num;
    unsigned char i;

    //waiting for the first character with time out
    i = 0;
    while(1)
    {
        delay(50);
        if(blueToothSerial.available())
        {
            recv_str[i] = char(blueToothSerial.read());
            i++;
            break;
        }
        time++;
        if(time > (timeout / 50)) return -1;
    }

    //read other characters from uart buffer to string
    while(blueToothSerial.available() && (i < 100))
    {                                              
        recv_str[i] = char(blueToothSerial.read());
        i++;
    }
#if NLCR    
    recv_str[i-2] = '\0';       //discard two character \n\r
#else
    recv_str[i] = '\0';
#endif
    return 0;
}
