#include <Arduino.h>
#include <TFT_eSPI.h> // Graphics and font library 
                      // NEED CHANGE: User_Setup_Select.h> comment line 22> //#include <User_Setup.h>  
                      //                                 uncomment line 53> #include <User_Setups/Setup25_TTGO_T_Display.h>    // Setup file for ESP32 and TTGO T-Display ST7789V SPI bus TFT   
#include <SPI.h>
#include <Firebase_ESP_Client.h>  
#include "addons/TokenHelper.h"   //Provide the token generation process info.
#include "addons/RTDBHelper.h"    //Provide the RTDB payload printing info and other helper functions.
#include "defines.h"
#include "ArduinoNvs.h"
#include "mywebserver.h"
#include "mywifi.h"

// GLOBALS
CMyWifi myWifi;
TFT_eSPI tft = TFT_eSPI();
unsigned long millisTFTRefresh;

//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;


// INIT
void setup() 
{
  // console init
  Serial.begin(115200);

  // non volatile storage init
  NVS.begin();

  // wifi init
  myWifi.Init();

  // http server init
  WebServerInit();

  // Display
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  millisTFTRefresh = millis();

  // Firebase: https://randomnerdtutorials.com/esp32-firebase-realtime-database/
   /* Assign the api key (required) */
  config.api_key = FIREBASE_WEB_API_KEY;
  /* Assign the RTDB URL (required) */
  config.database_url = FIREBASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

}

// MAIN LOOP
void loop() 
{
  // display
  if ((unsigned long)(millis() - millisTFTRefresh) > 1000) {
    millisTFTRefresh = millis();

    tft.setTextSize(2);
    tft.setCursor(0, 0, 1);
    tft.println("Wifi");
    tft.print("Mode: ");
    tft.println(myWifi.GetWifiMode());
    tft.print("Status: ");
    tft.println(myWifi.GetWifiStatus());
    tft.print("IP: ");
    tft.println(myWifi.GetWifiIP());
    tft.print("SSID: ");
    tft.println(myWifi.GetWifiSSID());
    tft.print("PSWD: ");
    tft.println(myWifi.GetWifiPassword());


  }
  // wifi check
  myWifi.Update();  

  // web handling
  WebServerUpdate();

  // Firebase
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    // Write an Int number on the database path test/int
    if (Firebase.RTDB.setInt(&fbdo, "test/int", count)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    count++;
    
    // Write an Float number on the database path test/float
    if (Firebase.RTDB.setFloat(&fbdo, "test/float", 0.01 + random(0,100))){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }

  yield();  // iddle, reset watchdog 
}