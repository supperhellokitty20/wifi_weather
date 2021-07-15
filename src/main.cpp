#include<Arduino.h> 
#include <LiquidCrystalIO.h>
#include <IoAbstractionWire.h>
#include <Wire.h>
#include <Arduino_JSON.h> 
#include <ESP8266HTTPClient.h> 
#include <WiFiClient.h> 
#include <ESP8266WiFi.h>  
#include <String> 


#define API_KEY "e544484d503214178d65887b620d784c"  
#define SSID  "Tien Thao 01 " 
#define PW "88889999" 
#define city "Porto" 
#define countryCode "PT" 
// THE DEFAULT TIMER IS SET TO 10 SECONDS FOR TESTING PURPOSES
 // For a final application, check the API call limits per hour/minute to avoid getting blocked/banned
unsigned long lastTime = 0;
 // Timer set to 10 minutes (600000)
 //unsigned long timerDelay = 600000;
 // Set timer to 10 seconds (10000)
unsigned long timerDelay = 10000;
double temp ; 
double humid   ;  

String jsonBuffer ; 

LiquidCrystalI2C_RS_EN(lcd,0x27,false) ;

void setup(){ 
    //Set up lcd 
    Serial.begin(9600) ; 
    Wire.begin() ; 
    lcd.begin(16,2) ; 
    //Set up wifi 
    WiFi.begin(SSID,PW) ; 
    while(WiFi.status() !=WL_CONNECTED){ 
        delay(500) ; 
        Serial.print("_") ; 
    } 
    Serial.println("Connected to wifi") ; 
    Serial.println(String(WiFi.localIP())) ;  
    //Setting up lcd  
    taskManager.scheduleFixedRate(timerDelay,[]{ 
        //Print temp 
        lcd.setCursor(0,0) ;         
        lcd.print(F("TEMP")) ;
        lcd.print(temp) ; 
        //Print humid 
        lcd.setCursor(0,1) ;         
        lcd.print(F("HIMID")) ;
        lcd.print(humid) ; 
        lcd.print("%") ; 
    }) ;  
} 
// Handling get requqesst  
String  get_request(const char* serverName ) { 
    WiFiClient client ; 
    HTTPClient http ;  
    http.begin(client ,serverName) ; 
    int  resp_code = http.GET() ; 
    String payload = "{}" ; 
    // If valid code 
    if(resp_code>0) { 
        Serial.print("Response code") ; 
        Serial.println(resp_code)  ;
        payload = http.getString() ;  
    } 
    else { 
        Serial.print("Error") ; 
        Serial.println(resp_code) ; 
    } 

    http.end() ;
    return payload ;
} 
// Getting info from the server 
void  get_info() { 
   if( ( millis()-lastTime) > timerDelay) { 
            //Your server name 
          String serverPath = (String) "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + API_KEY;
            //Get data from the server 
            jsonBuffer = get_request(serverPath.c_str()) ; 
            Serial.println(jsonBuffer) ; 
            JSONVar json_info = JSON.parse(jsonBuffer) ; 
            //Parsing failed 
            if(JSON.typeof(json_info)=="undefined")  { 
                Serial.println("parse failed") ;
                return ; 
            }             
            // Handling your json info here 
            // Adjust to match your type 
          /**  
            temp = json_info["main"]["feels_like"]  ;              
            humid= json_info["main"]["humidity"]   ;
            **/
            Serial.println(json_info["main"]["feels_like"]) ; 
            Serial.println(json_info["main"]["humidity"]); 
        } 
        lastTime = millis() ; 

   }  

void loop() { 
    // This will set the value for temp and humid var   
    get_info() ; 
} 

