// WebServer 


// alternative for https server: https://github.com/fhessel/esp32_https_server

//INCLUDES
#include <WebServer.h>
#include "defines.h"
#include "ArduinoNvs.h"
#include "mywebserver.h"


// GLOBALS
WebServer server(80);

// webobjects
#include "webobjects.inc"

// PAGE HANDLERS
void HandleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void HandleBulbOn()
{
  server.send_P(200,"image/gif", bulb_on_gif, sizeof(bulb_on_gif));
}

void HandleAvatar()
{
  server.send_P(200,"image/jpg", avatar_jpg, sizeof(avatar_jpg));
}


void HandleRoot()
{
  String message = "<!DOCTYPE html>";
  message +="<html><head>\n";
  //message += "<meta http-equiv='refresh' content='10'/>\n";
  message += "<title>TTGO Wifi Test</title>\n";
  message += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
  message += "<link rel=\"stylesheet\" type=\"text/css\" href=\"styles.css\">\n";
  message += "</head>\n";
  message += "<body>\n";
  message += "<div class=\"header\">\n";
	message += "<img src=\"avatar.jpg\" alt=\"avatar\" />\n";
	message += "<h1>TTGO Demo</h1>\n";
	message += "</div>\n";

  message += "<div class=\"row\">\n";
  message += "<div class=\"col-2\">\n";
  message += "<a href=\"setup\">Setup page</a>\n";
  message += "</div>\n";

  message += "<div class=\"col-10\">\n";
  message += "<h2>Hello from TTGO!</h2>\n";
  message += "<p>Up time: ";
  message += String(millis()/1000);
  message += "s <br>\n";
  message += "</p>\n";
  message += "<br/>\n";
  message += "<img src=\"pic_bulbon.gif\" width=\"100\" height=\"180\">\n";
  message += "</div>\n";
  message += "</div>\n";


  message += "<div class=\"footer\">\n";
  message += "<p>&copy; Petr Abrham</p>\n";
  message += "</div>\n";
  message += "</body></html>\n";
  
  server.send(200, "text/html", message);
}

void HandleSetup()
{
  String message = "TTGO Setup Page\n\n";
  String str;
  uint8_t i;

  // help for setup
  message += "Using:\n";
  message += "  setup?arg1=value1[&arg2=value2]...\n\n";
  message += "  arg:\n";
  message += "    ssid=[string] ... WiFi SSID\n";
  message += "    password=[string] ... Wifi password\n";
  message += "    reset=now ... Reset board\n\n";

  // parse arguments
  String argName;
  for (i = 0; i < server.args(); i++) {
    argName = server.argName(i);
    argName.toLowerCase();
    
    if (argName==String("ssid")) {
      str = server.arg(i);
      NVS.setString(NVS_KEY_SSID, str);
    }

    if (argName==String("password")) {
      str = server.arg(i);
      NVS.setString(NVS_KEY_PSWD, str);
    }

    if (argName==String("reset")) {
      if(server.arg(i)=="now") {
        ESP.restart();
      }
    }
  }

  message += "Server Method = "; 
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\n";
  message += "Arguments = " + String(server.args()) + "\n";

  for (i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  message += "-----------------------\n";
  
  server.send(200, "text/plain", message);
}


const char styles_css[] PROGMEM= {"html{font-family:\"LucidaSans\",sans-serif;}\nbody{background:#2c2f34;color:#f2f2f2;}\n*{box-sizing:border-box;}\n.row::after{content:\"\";clear:both;display:table;}\n[class*=\"col-\"]{float:left;padding:15px;}\n.header{background-color:#3c3c3c;text-align:center;padding:15px;}\n.header img{float:left;border-radius:50%;width:80px;height:80px;}\n.header h1{position:relative;top:18px;left:10px;}\n.footer{background-color:#3c3c3c;text-align:center;font-size:12px;position:fixed;left:0;bottom:0;width:100%;}\n[class*=\"col-\"]{width:100%;}\n@media only screen and (min-width:768px){.col-1{width:8.33%;}.col-2{width:16.66%;}.col-3{width:25%;}.col-4{width:33.33%;}.col-5{width:41.66%;}.col-6{width:50%;}.col-7{width:58.33%;}.col-8{width:66.66%;}.col-9{width:75%;}.col-10{width:83.33%;}.col-11{width:91.66%;}.col-12{width:100%;}}"};
void HandleStylesCss()
{
  server.send_P(200, "text/css", styles_css);
}

// Web init
void WebServerInit()
{
  server.on("/", HandleRoot);
  server.on("/setup", HandleSetup);

  server.on("/styles.css", HandleStylesCss);

  server.on("/pic_bulbon.gif", HandleBulbOn);
  server.on("/avatar.jpg", HandleAvatar);

  server.onNotFound(HandleNotFound);
  
  server.begin();
  Serial.println("HTTP server started");
}

// Web update
void WebServerUpdate()
{
  server.handleClient();
}

