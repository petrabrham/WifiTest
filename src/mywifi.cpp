

#include <Wifi.h>
//#include <ESPmDNS.h>
#include "defines.h"
#include "ArduinoNvs.h"

#include "mywifi.h"

// Constructor
CMyWifi::CMyWifi()
{
  m_msLastReconection = millis();
  m_stateConnection = false;
}

void CMyWifi::Init()
{
  // have a wifi credentials stored in NVS?
  wifi_mode_t wifimode = static_cast<wifi_mode_t>(NVS.getInt(NVS_KEY_WIFIMODE));
  String ssid = NVS.getString(NVS_KEY_SSID);
  String pswd = NVS.getString(NVS_KEY_PSWD);

  if (wifimode != WIFI_MODE_STA || ssid.isEmpty())
  {
    // local AP
    m_stateConnection = CreateAP();
  }
  else
  {
    // connect to wifi
    Connect(ssid.c_str(), pswd.c_str());
  
    // if can not connect wifi, AP will be created 
    if (WiFi.status() != WL_CONNECTED)
    {
      PrintStatus();  // print status of unconnected STA
      // run AP for possibility of setup device
      m_stateConnection = CreateAP();
    }
    else 
    {
      // initalize 
      if (wifimode != WIFI_MODE_STA)
      {
        NVS.setInt(NVS_KEY_WIFIMODE, (uint8_t) WIFI_MODE_STA);
      }
      m_stateConnection = true;
    }
  }
  PrintStatus();
/*
  // Start HTTP server 
  if (MDNS.begin("ttgo")) {
    Serial.println("MDNS responder started");
  }
  
  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);
*/
}

bool CMyWifi::CreateAP()
{
  /* Put IP Address details */
  IPAddress local_ip(192,168,4,1);
  IPAddress gateway(192,168,4,1);
  IPAddress subnet(255,255,255,0);

  DEBUG_PRINTLN("Establish WiFi ..");
  /*if(WiFi.getMode() & WIFI_MODE_STA)
  {
    WiFi.disconnect(true, true);
  }*/
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  return WiFi.softAP(OWN_SSID, OWN_PSWD);
}

void CMyWifi::Connect()
{
  String ssid = NVS.getString(NVS_KEY_SSID);
  String pswd = NVS.getString(NVS_KEY_PSWD);

  Connect(ssid.c_str(), pswd.c_str());
}

void CMyWifi::Connect(const char* ssid, const char *password)
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  DEBUG_PRINT("Connecting to WiFi ..");
  uint8_t retries = 0;
  while (WiFi.status() != WL_CONNECTED || retries < WIFI_RETRIES) {
    DEBUG_PRINT('.');
    delay(500);
    retries++;
  }
  DEBUG_PRINTLN('.');
}

String CMyWifi::GetWifiMode()
{
  String mode;

  wifi_mode_t wifiMode = WiFi.getMode();
  switch (wifiMode)
  {
    case WIFI_MODE_NULL:
      mode = "WIFI_MODE_NULL";
      break;

    case WIFI_MODE_STA:
      // WiFi station mode
      mode = "WIFI_MODE_STA";
      break;

    case WIFI_MODE_AP:
      // WiFi soft-AP mode
      mode = "WIFI_MODE_AP";
      break;

    case WIFI_MODE_APSTA:
      // WiFi station + soft-AP mode
      mode = "WIFI_MODE_APSTA";
      break;

    case WIFI_MODE_MAX:
      mode = "WIFI_MODE_MAX";
      break;

    default:
      mode = String(wifiMode);
  }

  return mode; 
}

String CMyWifi::GetWifiStatus()
{
  String status;

  wifi_mode_t wifiMode = WiFi.getMode();
  if (wifiMode == WIFI_MODE_STA)
  {
    switch (WiFi.status())
    {
      case WL_NO_SHIELD:
        status = "WL_NO_SHIELD";
        break;
      case WL_IDLE_STATUS:
        status = "WL_IDLE_STATUS";
        break;
      case WL_NO_SSID_AVAIL:
        status = "WL_NO_SSID_AVAIL";
        break;
      case WL_SCAN_COMPLETED:
        status = "WL_SCAN_COMPLETED";
        break;
      case WL_CONNECTED:
        status = "WL_CONNECTED";
        break;
      case WL_CONNECT_FAILED:
        status = "WL_CONNECT_FAILED";
        break;
      case WL_CONNECTION_LOST:
        status = "WL_CONNECTION_LOST";
        break;
      case  WL_DISCONNECTED:   
        status = "WL_DISCONNECTED";
        break;
      default:
        status = String(WiFi.status());
    }
  }
  else if (wifiMode == WIFI_MODE_AP)
  {
    status = "AP_RUNNING";
  }
  else
  {
    status = "UNKNOWN";
  }

  return status;
}

String CMyWifi::GetWifiIP()
{
  String ip;
  IPAddress cip;

  wifi_mode_t wifiMode = WiFi.getMode();
  if (wifiMode == WIFI_MODE_STA && WiFi.status() == WL_CONNECTED)
  {
    cip = WiFi.localIP();
    ip = cip.toString();  
  }
  else if (wifiMode == WIFI_MODE_AP)
  {
    cip = WiFi.softAPIP();
    ip = cip.toString();  }
  else
  {
    ip = "";
  }

  return ip;
}

String CMyWifi::GetWifiSSID()
{
  if (WiFi.getMode() == WIFI_MODE_AP)
  {
    return OWN_SSID;
  }
  else
  {
    return NVS.getString(NVS_KEY_SSID);
  }
}

String CMyWifi::GetWifiPassword()
{
  if (WiFi.getMode() == WIFI_MODE_AP)
  {
    return OWN_PSWD;
  }
  else
  {
    return "********";
  }
}

// DEBUG print status
void CMyWifi::PrintStatus()
{
  DEBUG_PRINT("Wifi mode: ");
  DEBUG_PRINTLN(GetWifiMode());
  DEBUG_PRINT("Wifi status: ");
  DEBUG_PRINTLN(GetWifiStatus());
  DEBUG_PRINT("IP: ");
  DEBUG_PRINTLN(GetWifiIP());
  if (WiFi.getMode() == WIFI_MODE_AP)
  {
    DEBUG_PRINT("SSID: ");
    DEBUG_PRINTLN(OWN_SSID);
    DEBUG_PRINT("PSWD: ");
    DEBUG_PRINTLN(OWN_PSWD);
  }
}

// Update
bool CMyWifi::Update()
{
  // connection check only after reconnect period
  if ((unsigned long)(millis() - m_msLastReconection) < WIFI_RECONECT_MS)
  {
    return m_stateConnection;
  }

  wifi_mode_t wifiMode = WiFi.getMode();
  switch (wifiMode)
  {
    case WIFI_MODE_AP:
      break;
    case WIFI_MODE_STA:
      if (WiFi.status() == WL_CONNECTED)
      {
        m_stateConnection = true;
      }
      else
      {
        m_stateConnection = WiFi.reconnect();
      }
      break;
    default:
      DEBUG_PRINTLN("Unsupported wifi mode.")
  }

  return m_stateConnection;
}

