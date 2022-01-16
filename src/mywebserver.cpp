// WebServer 


// alternative for https server: https://github.com/fhessel/esp32_https_server

//INCLUDES
#include <WebServer.h>
#include "defines.h"
#include "ArduinoNvs.h"
#include "mywebserver.h"
#include "mywifi.h"

// GLOBALS
WebServer server(80);

//externs
extern CMyWifi myWifi;

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
  String message;
  String str;
  uint8_t i;

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

  Serial.print("Server Method = "); 
  Serial.println((server.method() == HTTP_GET) ? "GET" : "POST");
  Serial.println("Arguments = " + String(server.args()));

  for (i = 0; i < server.args(); i++) {
    Serial.println(" " + server.argName(i) + ": " + server.arg(i));
  }

  message += "<!DOCTYPE html>\n";
  message += "<html>\n";
  message += "<head>\n";
  message += "\t<title> TTGO Demo </title>\n";
  message += "\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
  message += "\t<link rel=\"stylesheet\" type=\"text/css\" href=\"dark.css\"/>\n";
  message += "\t<script>\n";
  message += "\tfunction show1(){\n";
  message += "\t\tdocument.getElementById('sta').style.display ='none';\n";
  message += "\t}\n";
  message += "\tfunction show2(){\n";
  message += "\t\tdocument.getElementById('sta').style.display = 'block';\n";
  message += "\t}\n";
  message += "\t</script>\n";
  message += "</head>\n";
  message += "<body>\n";
  message += "\t<div class=\"header\">\n";
  message += "\t\t<img src=\"avatar-petr_100px.jpg\" alt=\"logo\" />\n";
  message += "\t\t<h1>TTGO Demo</h1>\n";
  message += "\t</div>\n";
  message += "\n";
  message += "\t<div class=\"sidebar\">\n";
  message += "\t\t<a href=\"index.html\">Home</a>\n";
  message += "\t\t<a class=\"active\" href=\"settings.html\">Wifi Settings</a>\n";
  message += "\t</div>\n";
  message += "\n";
  message += "\t<div class=\"content\">\n";
  message += "\t\t<form action=\"#\">\n";
  wifi_mode_t wifiMode = WiFi.getMode();
  message += "\t\t\t<input type=\"radio\" name=\"tab\" value=\"wifi_ap\" onclick=\"show1();\" "+(wifiMode==WIFI_MODE_AP ? "checked":"")+"/>AP<br/>\n";
  message += "\t\t\t<input type=\"radio\" name=\"tab\" value=\"wifi_sta\" onclick=\"show2();\" "+(wifiMode==WIFI_MODE_STA ? "checked":"")+"/>STA<br/>\n";
  message += "\t\t\t<div id=\"sta\" class=\"hide\">\n";
  message += "\t\t\t<fieldset>\n";
  message += "\t\t\t\t<label for=\"ssid\">SSID</label>\n";
  /*message += "\t\t\t\t<select id=\"ssid\" name=\"ssid\">\n";
  message += "\t\t\t\t<option value=\"australia\">Australia</option>\n";
  message += "\t\t\t\t<option value=\"canada\">Canada</option>\n";
  message += "\t\t\t\t<option value=\"usa\">USA</option>\n";
  message += "\t\t\t\t</select>\n";*/
  message += "\t\t\t\t<input type=\"text\" id=\"ssid\" name=\"ssid\">\n";
  message += "\t\t\t\t<label for=\"password\">Password</label>\n";
  message += "\t\t\t\t<input type=\"password\" id=\"password\" name=\"password\">\n";
  message += "\t\t\t</fieldset>\n";
  message += "\t\t\t</div>\n";
  message += "\n";
  message += "\t\t\t<input type=\"submit\" value=\"Submit\">\n";
  message += "\t\t</form>\n";
  message += "\t</div>\n";
  message += "\t\n";
  message += "\t<div class=\"footer\">\n";
  message += "\t\t<p>&copy; Petr Abrham</p>\n";
  message += "\t</div>\n";
  message += "\n";
  message += "</body>\n";
  message += "</html>";
  
  server.send(200, "text/html", message);
}

// https://www.toptal.com/developers/cssminifier/
const char styles_css[] PROGMEM= {"*{box-sizing:border-box}body{margin:0;font-family:Lato,sans-serif;background:#2c2f34;color:#f2f2f2}.header{background-color:#3c3c3c;text-align:center;padding:15px}.header img{float:left;border-radius:50%;width:80px;height:80px}.header h1{position:relative;top:18px;left:10px}.sidebar{margin:0;padding:0;width:200px;background-color:#3c3c3c;position:fixed;height:100%;overflow:auto}.sidebar a{display:block;color:#3549ff;padding:16px;text-decoration:none}.sidebar a.active{background-color:#0f186b;color:#fff}.sidebar a:hover:not(.active){background-color:#555;color:#fff}.footer{background-color:#3c3c3c;text-align:center;font-size:12px;padding:15px;position:fixed;left:0;bottom:0;width:100%;height:20px}div.content{margin-left:200px;padding:1px 16px;height:500px}.hide{display:none}input,select{width:100%;padding:12px 20px;margin:8px 0;display:block;border:1px solid #ccc;border-radius:4px;box-sizing:border-box}input[type=radio]{width:30px;padding:12px 20px;margin:8px 0;display:inline-block;border:1px solid #ccc}input[type=submit]{width:25%;background-color:#0f186b;color:#fff;padding:14px 20px;margin:8px 0;border:none;border-radius:4px;cursor:pointer}input[type=submit]:hover{background-color:#3549ff}fieldset{border-radius:4px}@media screen and (max-width:700px){.sidebar{width:100%;height:auto;position:relative}.sidebar a{float:left}div.content{margin-left:0}}@media screen and (max-width:360px){.sidebar a{text-align:center;float:none}}"};
void HandleStylesCss()
{
  server.send_P(200, "text/css", styles_css);
}

// Web init
void WebServerInit()
{
  server.on("/", HandleRoot);
  server.on("/index", HandleRoot);
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

