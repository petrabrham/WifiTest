#include <Arduino.h>
#include <TFT_eSPI.h> // Graphics and font library 
                      // NEED CHANGE: User_Setup_Select.h> comment line 22> //#include <User_Setup.h>  
                      //                                 uncomment line 53> #include <User_Setups/Setup25_TTGO_T_Display.h>    // Setup file for ESP32 and TTGO T-Display ST7789V SPI bus TFT   
#include <SPI.h>
#include "defines.h"
#include "ArduinoNvs.h"
#include "mywebserver.h"
#include "mywifi.h"

// GLOBALS
CMyWifi myWifi;
TFT_eSPI tft = TFT_eSPI();
unsigned long millisTFTRefresh;

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

  yield();  // iddle, reset watchdog 
}