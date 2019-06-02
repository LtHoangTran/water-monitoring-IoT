#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <String.h>
#include<LiquidCrystal.h> 
SoftwareSerial mySerial(9, 10);

LiquidCrystal lcd(7, 6, 5, 4, 3, 2); 
#define ONE_WIRE_BUS 12  //digital D12 pin for temperature value
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

float temp=0.0;
float turbid = 0.0; 
int sms_count = 0;
void setup()
{
  
  sensors.begin();
  mySerial.begin(9600);  // the GPRS baud rate  
  Serial.begin(9600);    // the GPRS baud rate 
  lcd.begin(16,2);       // lcd begin
  delay(1000);
}
 
void loop()
{
   sensors.requestTemperatures();
   temp = sensors.getTempCByIndex(0);
  turbid = analogRead(A1);// read the input on analog pin 0:
  turbid = turbid * (5.0 / 1024.0); // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):

    lcd.setCursor(0,0);
    lcd.print("Temp in C= "); //display temp
    lcd.print(temp);
    lcd.setCursor(0,1);
    lcd.print("Turbidity = ");//display turbidity
    lcd.print(turbid); 
      delay(2);          
    
    if(temp>40||turbid<1.5) //conditions for temp and turbidity alert
  {
      SetAlert(); // Function to send SMS Alerts
  }
       Send_data();
   
  if (mySerial.available())
    Serial.write(mySerial.read());
  
   
}
void Send_data()   //to send data to thingspeak channel
{
  mySerial.println("AT");
  delay(1000);

  mySerial.println("AT+CPIN?");
  delay(1000);

  mySerial.println("AT+CREG?");
  delay(1000);

  mySerial.println("AT+CGATT?");
  delay(1000);

  mySerial.println("AT+CIPSHUT");
  delay(1000);

  mySerial.println("AT+CIPSTATUS");
  delay(2000);

  mySerial.println("AT+CIPMUX=0");
  delay(2000);
 
  ShowSerialData();
 
  mySerial.println("AT+CSTT=\"www\"");//start task and setting the APN,
  delay(1000);
 
  ShowSerialData();
 
  mySerial.println("AT+CIICR");//bring up wireless connection
  delay(3000);
 
  ShowSerialData();
 
  mySerial.println("AT+CIFSR");//get local IP adress
  delay(2000);
 
  ShowSerialData();
 
  mySerial.println("AT+CIPSPRT=0");
  delay(3000);
 
  ShowSerialData();
  
  mySerial.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");//start up the connection
  delay(6000);
 
  ShowSerialData();
 
  mySerial.println("AT+CIPSEND");//begin send data to remote server
  delay(4000);
  ShowSerialData();
  
  String str="GET https://api.thingspeak.com/update?api_key=SALKQM2665XVB256&field1=" + String(temp)+"&field2=" + String(turbid);
  
  mySerial.println(str);//begin send data to remote server
  delay(4000);
  ShowSerialData();

  mySerial.println((char)26);//sending
  delay(5000);//waitting for reply, the time is base on the condition of internet 
  mySerial.println();
 
  ShowSerialData();
 
  mySerial.println("AT+CIPSHUT");//close the connection
  delay(100);
  ShowSerialData();
} 
void ShowSerialData()
{
  while(mySerial.available()!=0)
    Serial.write(mySerial.read());
}
void SetAlert()
{
  while(sms_count<3) //Number of SMS Alerts to be sent
  {  
    SendTextMessage(); // Function to send AT Commands to GSM module
  }
}
void SendTextMessage()
{
  mySerial.println("AT+CMGF=1");    //To send SMS in Text Mode
  delay(2000);
  mySerial.println("AT+CMGS=\"+91XXXXXXXXXX\"\r"); // change to the phone number you using 
  delay(2000);
  mySerial.println("Alert for Temperature and Turbidity! ");//the content of the message
   mySerial.println(temp);
   mySerial.println(turbid);
  delay(200);
  mySerial.println((char)26);//the stopping character
  delay(5000);
  sms_count++;
}
