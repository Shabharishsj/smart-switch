/*IOT Based Smart Switch with Voice Control Using AI*/

/*Code for Home automation using alexa and LCD interface*/

/* Project team : SHABHARISH S J   1PL14EC029
 *                ARJUN V          1PL14EC005
 *                M N FAISAL       1PL13EC004
 *                KIRAN T T        1PL14EC410
 *                
 * Project Guide: Prof. PRATHIBHA RANI 
 * 
 * Department Of ECE  2014 BATCH
 * Shri Pillappa College Of Engineering 
 * 
 * The unmodified version of this code is originally by kakopappa and can be found at http://bit.ly/2kKQiRg.
 * 
 * This version of the code has been modified by Shabharish SJ
 * 
 */

//Headers 

//NodeMCU 8266 Header files
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

//Aleaxa and NodeMCU communication header files
#include <WiFiUdp.h>
#include <functional>
#include "switch.h"
#include "UpnpBroadcastResponder.h"
#include "CallbackFunction.h"

// 20*4 I2C LCD header files
#include <Wire.h>
#include <LiquidCrystal_I2C.h>



// Functions declaration

// Wifi connection
boolean Wifi_connection();

// State functions
void Light1_ON();
void Device1_ON();
void Outlet1_ON();
void Outlet2_ON();
void Light1_OFF();
void Device1_OFF();
void Outlet1_OFF();
void Outlet2_OFF();

void LightA_ON();
void LightA_OFF();
void DeviceA_ON();
void DeviceA_OFF();
void OutletA_ON();
void OutletA_OFF();
void OutletB_ON();
void OutletB_OFF();

//Global Variable Declaration 

//Intializing wificonnection
boolean WiFiconnection = false;

//WiFi NETWORK CONNECTION 
const char* ssid = "minato123";                   // Network ID name
const char* password = "foxysj4546";              // Network Password

//Upnp Broadcasting Response 
UpnpBroadcastResponder upnpBroadcastResponder;

//Initializing Switches to NULL
Switch *light1 = NULL;
Switch *device1 = NULL;
Switch *outlet1= NULL;
Switch *outlet2 = NULL;

Switch *lighta = NULL;
Switch *devicea = NULL;
Switch *outleta = NULL;
Switch *outletb = NULL;

//Setting up of NodeMCU pins to Relay
int Relay1 = 14;                               // Pin Name D5
int Relay2 = 12;                               // Pin Name D6
int Relay3 = 13;                               // Pin Name D7
int Relay4 = 15;                               // Pin Name D8


//Setting up of 20x4 LCD display 
LiquidCrystal_I2C lcd(0x3F, 4, 20);            // Address: 0x3F is the interfacing address for LCD. 20,4 characters 

int LA = 0;
int LAR = 0;
int inLA = 03;

int DA = 0;
int DAR = 0;
int inDA = 16;

int OA = 0;
int OAR = 0;
int inOA = 05;

int OB = 0;
int OBR = 0;
int inOB = 04;
                                             
void setup() 
{
  //Setting up LCD
    Wire.begin(2,0);                           // Begin of Interfacing
    lcd.init();                                // Initializing the LCD
    lcd.backlight();                           // Turning ON the Backlight of LCD
    lcd.clear();                               // Clearing the LCD 


  //Displaying Intro messages 

  //Project Name
    lcd.setCursor(5, 0);                       // Setting up the cusor location in the LCD for displaying the entered information
    lcd.print("IOT Based");                    // String wrttien within " " which will be displayed on LCD
    delay(500);                                // Delay added which ables to read without chaning too fast. value 1= 1ms
    lcd.setCursor(5, 1);                       // Spacing Of 5 characters from left in ROW 2
    lcd.print("Smart Home");
    lcd.setCursor(5, 2);                       // ROW 3   
    lcd.print("Automation");
    delay(3000);                               // 3000= 3secs
    lcd.clear();                               // LCD cleared
    lcd.setCursor(5, 1);                      
    lcd.print("With Voice");        
    lcd.setCursor(4, 2);                       // Spacing Of 4 characters from left in ROW 3
    lcd.print("Recognition");
    delay(5000);
    lcd.clear();                               // LCD cleared

   //Displaying Team Members and Guide 
      lcd.setCursor(0, 0);
      lcd.print("SHABHARISH SJ");
      delay(500);
      lcd.setCursor(0, 1);
      lcd.print("ARJUN V BASHA");
      delay(500);
      lcd.setCursor(0, 2);
      lcd.print("FAISAL N M");
      delay(500);
      lcd.setCursor(0, 3);
      lcd.print("KIRAN T T");
      delay(5000);
      lcd.clear();

      lcd.setCursor(7, 2);
      lcd.print("Guide:");
      delay(500);
      lcd.setCursor(0, 3);
      lcd.print("PROF.PRATHIBA RANI");
      delay(5000);
      lcd.clear();
    
   //Initialise Wifi Connectivity 

      lcd.setCursor(0, 0);
      lcd.print("Connecting to WiFi..");      //Keeps on displaying until connected 
      delay(3000);
      WiFiconnection = Wifi_connection();     // Wifi_connection returns a vlaue boolean type TRUE  if coonected 
      
      lcd.clear();

   //Adding Callbacks for specific Switches
   if (WiFiconnection) {
    upnpBroadcastResponder.beginUdpMulticast();

   // Show WiFi status on LCD along with SSID of network
    lcd.setCursor(0, 0);
    lcd.print("***WiFi Connected***");
    delay(500);
    lcd.setCursor(0, 2);
    lcd.print("NetworkID:");
    lcd.print(ssid);
    delay(2000);
    lcd.clear();

    // Define your switches here. Max 14
    // Format: Alexa invocation name, local port no, on callback, off callback
    light1 = new Switch("Light One", 80, Light1_ON, Light1_OFF);
    device1 = new Switch("Device One", 81, Device1_ON, Device1_OFF);
    outlet1 = new Switch("Outlet One", 82, Outlet1_ON, Outlet1_OFF);
    outlet2 = new Switch("Outlet Two", 83, Outlet2_ON, Outlet2_OFF);

    lighta = new Switch("Light Auto", 84, LightA_ON, LightA_OFF);
    devicea = new Switch("Device Auto", 85, DeviceA_ON, DeviceA_OFF);
    outleta = new Switch("Outlet 1 Auto", 86, OutletA_ON, OutletA_OFF);
    outletb = new Switch("Outlet 2 Auto", 87, OutletB_ON, OutletB_OFF);

    //Serial.println("Adding switches upnp broadcast responder");
    upnpBroadcastResponder.addDevice(*light1);
    upnpBroadcastResponder.addDevice(*device1);
    upnpBroadcastResponder.addDevice(*outlet1);
    upnpBroadcastResponder.addDevice(*outlet2);

    upnpBroadcastResponder.addDevice(*lighta);
    upnpBroadcastResponder.addDevice(*devicea);
    upnpBroadcastResponder.addDevice(*outleta);
    upnpBroadcastResponder.addDevice(*outletb);
    

    //Setting Modes of pins in NodeMCU to relay 
    pinMode(14, OUTPUT);                                //PIN Name D5
    pinMode(12, OUTPUT);                                //PIN Name D6                                      
    pinMode(13, OUTPUT);                                //PIN Name D7
    pinMode(15, OUTPUT);                                //PIN Name D8

    pinMode(03, INPUT);
    pinMode(16, INPUT);
    pinMode(05, INPUT);
    pinMode(04, INPUT);


    //Displaying Searching for devices
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("   Searching For  ");
    lcd.setCursor(0, 2);
    lcd.print("  Smart Devices  ");
    delay(2000);
    lcd.clear();

    //Initializing the Relays to OFF states and displaying the searched devices
    //Set each relay pin to HIGH . HIGH turns OFF relay and LOW turns ON relay
    digitalWrite(Relay1, HIGH);   // sets relay1 OFF
    lcd.print("Light  One");
    delay(1000);
    digitalWrite(Relay2, HIGH);   // sets relay2 OFF
    lcd.setCursor(0, 1);
    lcd.print("Device One");
    delay(1000);
    digitalWrite(Relay3, HIGH);   // sets relay3 OFF
    lcd.setCursor(0, 2);
    lcd.print("Outlet One");
    delay(1000);
    digitalWrite(Relay4, HIGH);   // sets relay6 OFF
    lcd.setCursor(0, 3);
    lcd.print("Outlet Two");
    delay(3000);
    lcd.clear();

    //Displaying System ready for commands
    lcd.clear();
    lcd.setCursor(0, 0);
    delay(1000);
    lcd.print(" System Initialzed  ");
    delay(1000);
    lcd.setCursor(0, 2);
    lcd.print(" Ready For Commands ");
    delay(2000);
    lcd.clear();

    //Set up device status message
    lcd.clear();
    lcd.print("Light  One: off    ");
    delay(500);
    lcd.setCursor(0, 1);
    lcd.print("Device One: off   ");
    delay(500);
    lcd.setCursor(0, 2);
    lcd.print("Outlet One: off    ");
    delay(500);
    lcd.setCursor(0, 3);
    lcd.print("Outlet Two: off    ");
    delay(500);

    
   }
}


void loop()
{
  if (WiFiconnection) {
    upnpBroadcastResponder.serverLoop();
    light1->serverLoop();
    device1->serverLoop();
    outlet1->serverLoop();
    outlet2->serverLoop();
    
    lighta->serverLoop();
    devicea->serverLoop();
    outleta->serverLoop();
    outletb->serverLoop();

    LA = digitalRead(inLA); 
    DA = digitalRead(inDA); 
    OA = digitalRead(inOA); 
    OB = digitalRead(inOB); 

    if(LAR == 1)
    {
     LightA_ON();
    }

    if(DAR == 1)
    {
     DeviceA_ON();
    }

    if(OAR == 1)
    {
     OutletA_ON();
    }

    if(OBR == 1)
    {
     OutletB_ON();
    }


    
  }
}


//Functions


//LIGHTS
//LIGHT ONE
void Light1_ON() {
  if(LAR == 0)
  {
  digitalWrite(Relay1, LOW);                        // Sets Relay 1 ON ->>> LIGHT ONE ON 
  lcd.setCursor(0, 0);
  lcd.print("Light  One: On      ");
  }
}

void Light1_OFF() {
  if( LAR == 0)
  {
  digitalWrite(Relay1, HIGH);                       // Sets Relay 1 OFF ->>> LIGHT ONE OFF
  lcd.setCursor(0, 0);    
  lcd.print("Light  One: Off     ");
  }
}



//DEVICES
//DEVICE ONE
void Device1_ON() {
  if(DAR == 0)
  {
  digitalWrite(Relay2, LOW);                        // Sets Relay 2 ON ->>> DEVICE ONE ON 
  lcd.setCursor(0, 1);
  lcd.print("Device One: On      ");
  }
}

void Device1_OFF() {
  if(DAR == 0)
  {
  digitalWrite(Relay2, HIGH);                       // Sets Relay 2 OFF ->>> DEVICE ONE OFF
  lcd.setCursor(0, 1);
  lcd.print("Device One: Off     ");
  }
}


//OUTLETS
//OUTLET ONE
void Outlet1_ON() {
  if(OAR == 0)
  {
  digitalWrite(Relay3, LOW);                       // Sets Relay 3 ON ->>> OUTLET ONE ON
  lcd.setCursor(0, 2);
  lcd.print("Outlet One: On     ");
  }
}

void Outlet1_OFF() {
  if(OAR == 0)
  {
  digitalWrite(Relay3, HIGH);                       // Sets Relay 3 OFF ->>> OUTLET ONE OFF
  lcd.setCursor(0, 2);
  lcd.print("Outlet One: Off     ");
  }
}

//OUTLET TWO
void Outlet2_ON() {
  if(OBR == 0)
  {
  digitalWrite(Relay4, LOW);                       // Sets Relay 6 ON ->>> OUTLET TWO ON
  lcd.setCursor(0, 3);
  lcd.print("Outlet Two: On     ");
  }
}

void Outlet2_OFF() {
  if(OBR == 0)
  {
  digitalWrite(Relay4, HIGH);                       // Sets Relay 6 OFF ->>> OUTLET TWO OFF
  lcd.setCursor(0, 3);
  lcd.print("Outlet Two: Off     ");
  }
}

void LightA_ON() {
  LAR = 1;
  if(LA == 1)
  {
  digitalWrite(Relay1, LOW);                        // Sets Relay 1 ON ->>> LIGHT ONE ON 
  lcd.setCursor(0, 0);
  lcd.print("Light  One: Auto On ");
  }
  else
  {
  digitalWrite(Relay1, HIGH);                        // Sets Relay 1 ON ->>> LIGHT ONE ON 
  lcd.setCursor(0, 0);
  lcd.print("Light  One: Auto Off");
  }
}

void LightA_OFF() {
  LAR = 0;
  digitalWrite(Relay1, HIGH);                       // Sets Relay 1 OFF ->>> LIGHT ONE OFF
  lcd.setCursor(0, 0);    
  lcd.print("Light  One: Off     ");
}



void DeviceA_ON() {
  DAR = 1;
  if(DA == 1)
  {
  digitalWrite(Relay2, LOW);                        // Sets Relay 1 ON ->>> LIGHT ONE ON 
  lcd.setCursor(0, 1);
  lcd.print("Device One: Auto On ");
  }
  else
  {
  digitalWrite(Relay2, HIGH);                        // Sets Relay 1 ON ->>> LIGHT ONE ON 
  lcd.setCursor(0, 1);
  lcd.print("Device One: Auto Off");
  }
}

void DeviceA_OFF() {
  DAR = 0;
  digitalWrite(Relay2, HIGH);                       // Sets Relay 1 OFF ->>> LIGHT ONE OFF
  lcd.setCursor(0, 1);    
  lcd.print("Device One: Off     ");
}


void OutletA_ON() {
  OAR = 1;
  if(OA == 1)
  {
  digitalWrite(Relay3, LOW);                        // Sets Relay 1 ON ->>> LIGHT ONE ON 
  lcd.setCursor(0, 2);
  lcd.print("Outlet One: Auto On ");
  }
  else
  {
  digitalWrite(Relay3, HIGH);                        // Sets Relay 1 ON ->>> LIGHT ONE ON 
  lcd.setCursor(0, 2);
  lcd.print("Outlet One: Auto Off");
  }
}

void OutletA_OFF() {
  OAR = 0;
  digitalWrite(Relay3, HIGH);                       // Sets Relay 1 OFF ->>> LIGHT ONE OFF
  lcd.setCursor(0, 2);    
  lcd.print("Outlet One: Off     ");
}

void OutletB_ON() {
  OBR = 1;
  if(OB == 1)
  {
  digitalWrite(Relay4, LOW);                        // Sets Relay 1 ON ->>> LIGHT ONE ON 
  lcd.setCursor(0, 3);
  lcd.print("Outlet Two: Auto On ");
  }
  else
  {
  digitalWrite(Relay4, HIGH);                        // Sets Relay 1 ON ->>> LIGHT ONE ON 
  lcd.setCursor(0, 3);
  lcd.print("Outlet Two: Auto Off");
  }
}

void OutletB_OFF() {
  OBR = 0;
  digitalWrite(Relay4, HIGH);                       // Sets Relay 1 OFF ->>> LIGHT ONE OFF
  lcd.setCursor(0, 3);    
  lcd.print("Outlet Two: Off     ");
}


// Wifi Connectivty Check
// connect to wifi – returns true if successful or false if not
boolean Wifi_connection(){
  boolean state = true;
  int i = 0;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  // Serial.print("Connecting ...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
    if (i > 10) {
      state = false;
      break;
    }
    i++;
  }

  if (state) {
    //  Serial.println("");
    //  Serial.print("Connected to ");
    //  Serial.println(ssid);
    // Serial.print("IP address: ");
    //  Serial.println(WiFi.localIP());
  }
  else {
    // Serial.println("");
    //Serial.println("Connection failed.");
  }

  return state;         // RETURN TRUE OR FLASE ..AFTER WHILE LOOP
}


