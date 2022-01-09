#ifndef MYWIFI_H
#define MYWIFI_H

#ifndef ARDUINOMWF_SILENT
#define ARDUINOMVF_SILENT 0
#endif

#if ARDUINOMWF_SILENT
  #define DEBUG_PRINT(...) { }
  #define DEBUG_PRINTLN(...) { }
  #define DEBUG_PRINTF(fmt, args...) { }
#else
  #define DEBUG_PRINTER Serial
  #define DEBUG_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
  #define DEBUG_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
  #define DEBUG_PRINTF(fmt, args...) { DEBUG_PRINTER.printf(fmt,## args); }
#endif

#define OWN_SSID "ESP32local"
#define OWN_PSWD "1234567890"
#define WIFI_RETRIES 10
#define WIFI_RECONECT_MS 5000


class CMyWifi
{
  private:
    unsigned long m_msLastReconection;
    bool m_stateConnection;
    
    bool CreateAP();
    void Connect(const char* ssid, const char *password);
    void Connect();
    void PrintStatus();


  public:
    CMyWifi();    
    void Init();
    bool Update();

    String GetWifiMode();
    String GetWifiStatus();
    String GetWifiIP();
    String GetWifiSSID();
    String GetWifiPassword();
};

#endif
