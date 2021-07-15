#include<Arduino.h> 
#include <LiquidCrystalIO.h>
#include <IoAbstractionWire.h>
#include <Wire.h>
#include <Arduino_JSON.h> 
#include <ESP8266HTTPClient.h> 
#include <WiFiClient.h> 
#include <ESP8266WiFi.h>  
#include <stdio.h> 
#include <String> 


#define API_KEY "e544484d503214178d65887b620d784c"  
#define SSID  "Tien Thao 01" 
#define PW "88889999" 
#define city "Porto" 
#define countryCode "PT" 
double temp ; 
double humid ; 
// THE DEFAULT TIMER IS SET TO 10 SECONDS FOR TESTING PURPOSES
// For a final application, check the API call limits per hour/minute to avoid getting blocked/banned
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 10 seconds (10000)
unsigned long timerDelay = 10000;
LiquidCrystalI2C_RS_EN(lcd,0x27,false) ;

String jsonBuffer;

void setup() {
  Serial.begin(9600);
  Wire.begin() ; 
  lcd.begin(16,2);
  WiFi.begin(SSID, PW);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 10 seconds (timerDelay variable), it will take 10 seconds before publishing the first reading.");
    //Setting up the lcd 
    taskManager.scheduleFixedRate(250, [] {
    // (note: line 1 is the second row, since counting begins with 0):
        lcd.noBlink();
        lcd.setCursor(0,0) ;
        lcd.print(F("Temp:")); 
        lcd.print(temp) ;
        // print the number of seconds since reset:
        //float secondsFraction =  millis() / 1000.0F;
        lcd.setCursor(0, 1);
        lcd.print(F("Humid:")); 
        lcd.print(humid);
        lcd.print(F("%"));
  });
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(client, serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

void loop() {
  // Send an HTTP GET request
  if ((millis() - lastTime) > timerDelay) {
    // Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      String  serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" city  ","  countryCode "&APPID=" API_KEY;
      
      jsonBuffer = httpGETRequest(serverPath.c_str());
      Serial.println(jsonBuffer);
      JSONVar myObject = JSON.parse(jsonBuffer);
  
      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }
        temp =   myObject["main"]["temp"];
        humid=  myObject["main"]["humidity"]; 
      Serial.print("JSON object = ");
      Serial.println(myObject);
      Serial.print("Temperature: ");
      Serial.println(temp);
      Serial.print("Humidity: ");
      Serial.println(humid);
      /** 
      Serial.println(myObject["main"]["temp"]);
      Serial.print("Pressure: ");
      Serial.println(myObject["main"]["pressure"]);
      Serial.print("Humidity: ");
      Serial.println(myObject["main"]["humidity"]);
      Serial.print("Wind Speed: ");
      Serial.println(myObject["wind"]["speed"]);
      **/ 
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
    taskManager.runLoop() ;
  }

}