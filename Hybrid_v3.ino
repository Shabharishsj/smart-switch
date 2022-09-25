#include <dht.h>
#include <DS3231.h>
dht DHT;
#define DHT11_PIN A0

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 4, 20); 

const int trigPin = 3;
const int echoPin = 2;
// defines variables
long duration;
int distance;

DS3231  rtc(SDA, SCL);
Time t;

const int OnHour = 14;
const int OnMin = 50;
const int OffHour = 14;
const int OffMin = 51;
const int OnHour1 = 14;
const int OnMin1 = 52;
const int OffHour1 = 14;
const int OffMin1 = 53;


int outLA = 8;
int outDA = 7;
int outOA = 6;
int outOB = 5;


#include <SoftwareSerial.h>
#define RX 10
#define TX 11
String AP = "minato123";       // CHANGE ME
String PASS = "foxysj4546"; // CHANGE ME
   // CHANGE ME
String HOST = "api.thingspeak.com";
String PORT = "80";
String field = "field1";
String field2 = "field2";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
int valSensor = 1;
int valSensor2 = 1;
SoftwareSerial esp8266(RX,TX); 




char rx_char;

void setup(){

  
  Serial.begin(9600);
  esp8266.begin(115200);


    Serial.println("UPDATE VALUES??");
    
    rx_char = Serial.read();
   while (!Serial.available()){
    //Do Absolutely Nothing until something is received over the serial port
  }
  rx_char = Serial.read();

if (rx_char == 'Y' )
{


  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");

}
  
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  pinMode(8, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT);
  
  rtc.begin();

   lcd.init();                                // Initializing the LCD
   lcd.backlight();                           // Turning ON the Backlight of LCD
   lcd.clear(); 


}




void loop()
{
  Serial.println(" \n ");
  DHT.read11(DHT11_PIN);
  Serial.print("Temperature = ");
  Serial.println(DHT.temperature);
  Serial.print("Humidity = ");
  Serial.println(DHT.humidity);

  // Clears the trigPin
digitalWrite(trigPin, LOW);
delayMicroseconds(2);
// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);
// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH,50000);
// Calculating the distance
distance= duration*0.034/2;
// Prints the distance on the Serial Monitor
Serial.print("Distance: ");
Serial.println(distance);
//Serial.println(rx_char);

t = rtc.getTime();
 Serial.print(t.hour);
 Serial.print(" hour(s), ");
 Serial.print(t.min);
 Serial.print(" minute(s)");
 Serial.println(" ");
  delay (1000);
Serial.println(" \n ");
Serial.println(" \n ");

if (rx_char == 'Y' )
{


  
String API = "3GO0XTVQ07C1HZJG";
Serial.println(" distance ");
valSensor = getSensorData2();
 String getData = "GET /update?api_key="+ API +"&"+ field +"="+String(valSensor);
sendCommand("AT+CIPMUX=1",5,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
 sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
 esp8266.println(getData);delay(100);countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK");
 
 delay(2000);
 
 Serial.println("Temp");
 valSensor2 = getSensorData();
 getData = "GET /update?api_key="+ API +"&"+ field2 +"="+String(valSensor2);
sendCommand("AT+CIPMUX=1",5,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
 sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
 esp8266.println(getData);delay(100);countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK");

 
}
 
lcd.setCursor(0, 0);// Setting up the cusor location in the LCD for displaying the entered information
lcd.print("Temperature:");  // String wrttien within " " which will be displayed on LCD
lcd.print(DHT.temperature); 
lcd.print("degrees"); 
lcd.setCursor(0, 1);// Setting up the cusor location in the LCD for displaying the entered information
lcd.print("Distance:");  // String wrttien within " " which will be displayed on LCD
lcd.print(distance);
lcd.print("cms   "); 
lcd.setCursor(0, 2);// Setting up the cusor location in the LCD for displaying the entered information
lcd.print("OnT:");  // String wrttien within " " which will be displayed on LCD
lcd.print(OnHour); 
lcd.print(":");
lcd.print(OnMin); 
lcd.print(" OFT:");  // String wrttien within " " which will be displayed on LCD
lcd.print(OffHour); 
lcd.print(":");
lcd.print(OffMin);
lcd.setCursor(0, 3);// Setting up the cusor location in the LCD for displaying the entered information
lcd.print("OnT:");  // String wrttien within " " which will be displayed on LCD
lcd.print(OnHour1);
lcd.print(":"); 
lcd.print(OnMin1); 
lcd.print(" OFT:");  // String wrttien within " " which will be displayed on LCD
lcd.print(OffHour1); 
lcd.print(":");
lcd.print(OffMin1);

  

  if(distance<100)
  {
    Serial.println("LIGHT TURNS ON");
    digitalWrite(outLA, HIGH);
    
  }
  else
  {
    Serial.println("LIGHT TURNS OFF");
    digitalWrite(outLA, LOW);
  }

  if(DHT.temperature>32)
  {
    Serial.println("FAN TURNS ON");
    digitalWrite(outDA, HIGH);
  }
  else
  {
    Serial.println("FAN TURNS OFF");
    digitalWrite(outDA, LOW);
  }


     
if(t.hour == OnHour && t.min == OnMin){
    Serial.println("OUTLET 1 ON");
    digitalWrite(outOA, HIGH);
    }
    
    else if(t.hour == OffHour && t.min == OffMin){
      Serial.println("OUTLET 1 OFF");
      digitalWrite(outOA, LOW);
    }

if(t.hour == OnHour1 && t.min == OnMin1){
    Serial.println("OUTLET 2 ON");
    digitalWrite(outOB, HIGH);
    }
    
    else if(t.hour == OffHour1 && t.min == OffMin1){
      Serial.println("OUTLET 2 OFF");
      digitalWrite(outOB, LOW);
    }
    
      
Serial.println(" \n ");
Serial.println("_________________________________________________________");


}


int getSensorData(){
  return DHT.temperature; // Replace with 
}

int getSensorData2(){
  return distance; // Replace with 
}




void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }

