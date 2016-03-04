//    _                    ___  ______ _____
//   (_)                  / _ \ | ___ \_   _|
//    _ ___  ___  _ __   / /_\ \| |_/ / | |
//   | / __|/ _ \| '_ \  |  _  ||  __/  | |
//   | \__ \ (_) | | | | | | | || |    _| |_
//   | |___/\___/|_| |_| \_| |_/\_|    \___/
//  _/ |
// |__/
//
// @auth Alex Suslov <suslov@me.com>
// @auth Victor Brutskiy <4refr0nt@gmail.com>

// !!! delete in production
void Serial_var( String name, String var){
  Serial.print(name + ": ");
  Serial.println(var);
}
boolean isLoggedInApi(){
  return true;
}

#if FEATURE_API

#include "app.h"

// @return "name":"value" from name, value
String json_string( String name, String value  ) {
  return "\"" + name + "\"" + ":" + "\"" + value + "\"";
}
/*
// @return "name":value from name, value
String json_number(  String name, String value) {
  return "\"" + name + "\"" + ":" +  value;
}
// return "name":value from name, value
String json_number(  String name, int value) {
  return "\"" + name + "\"" + ":" +  value;
}
// @return "name":[values] from name, values
String json_array(  String name, String values)  {
  return "\"" + name + "\"" + ":[" + values + "]";
}
*/
//********************************************************************************
// API main handler
//********************************************************************************
void handle_api() {

  if (!isLoggedInApi()) return;

  if ( WebServer.method() == HTTP_OPTIONS ) {
    handle_api_options(); // CORS
    return;
  }

  if ( WebServer.method() == HTTP_POST ) {
    if (Settings.UseSerial && (Settings.SerialLogLevel >= LOG_LEVEL_DEBUG)) {
      Serial.print(F("HTTP : RCV POST : "));
      for (byte x = 0; x < WebServer.args(); x++) {
        Serial.print(WebServer.argName(x));
        Serial.print(F(" = "));
        Serial.println(WebServer.arg(x));
      }
    }
  }

  byte query;

  if ( WebServer.hasArg("q") ) {
    query = WebServer.arg("q").toInt();
  } else {
    handle_api_main();
    return;
  }

  switch ( query ) {
    case 0: // root [get]
            handle_api_root();
            break;
    case 1: // config [get] [post] [options]
            handle_api_config();
            break;
    case 2:  // hardware [get] [post] [options]
            handle_api_hardware();
            break;
    case 3:  // devices [get]
            handle_api_devices();
            break;
    case 4:  // device [get] [post] [options]
            handle_api_device();
            break;
    case 5:  // advanced [get] [post] [options]
            handle_api_advanced();
            break;
    case 6:  // log [get]
            handle_api_log();
            break;
    case 7:  // wifi [get]
            handle_api_wifi();
            break;
    case 8:  // i2c [get]
            handle_api_i2c();
            break;
    case 9:  // cmd [post] [options]
            handle_api_cmd();
            break;
    case 10:
            handle_dev_temp();
            break;
    default:
         WebServer.send(500);
    }
}
//********************************************************************************
// API [OPTIONS]
// return headers only, CORS
//********************************************************************************
void handle_api_options() {

     String reply = "";

     WebServer.sendHeader(F("Connection"), F("close"));
     WebServer.sendHeader(F("Access-Control-Allow-Origin") , F("*"));
     WebServer.sendHeader(F("Access-Control-Allow-Methods"), F("POST, GET, OPTIONS"));
     WebServer.sendHeader(F("Access-Control-Allow-Headers"), F("Content-Type, X-Requested-With"));

     WebServer.send(200, "application/json", reply);

} // handle_api_options


//********************************************************************************
// Web Interface Wifi scanner
// API [GET]
// @return [{ SSID:"ssid", RSSI:"rssi"},...]
//********************************************************************************
void handle_api_wifi() {

  String reply = F("[");
  String comma = F(",");

  int n = WiFi.scanNetworks();

  if (n == 0) {
    reply = reply + F("{") + json_string( F("error"), F("No Access Points found") ) + F("}");
  } else {
    for (int i = 0; i < n; ++i) {
      if (i > 0) reply += comma;
      String ssid = String( WiFi.SSID(i) );
      if ( ssid.indexOf("\"") > 0 ) ssid.replace(F("\""),F("<!!!>")); // if <"> symbol in SSID
      reply = reply + F("{") + json_string( F("SSID"), ssid );
      reply = reply + comma + json_string( F("RSSI"), String( WiFi.RSSI(i) )) + F("}");
    } // for
  } // else (n == 0)

  reply = reply + F("]");

  WebServer.send(200, "application/json", reply);

} // handle_api_wifi

//********************************************************************************
// API [GET][POST] config
//********************************************************************************
void handle_api_config() {

  if ( WebServer.method() == HTTP_POST ) save_config();  // shared with WEB HTML

  String reply = F("{");
  String comma = F(",");
  char tmp_buff[80];

  reply +=         json_string( F("name"), Settings.Name);
  reply += comma + json_string( F("usedns"), String(Settings.UseDNS) );
  reply += comma + json_string( F("unit"), String(Settings.Unit) );
  reply += comma + json_string( F("protocol"), String(Settings.Protocol));

  sprintf_P(tmp_buff, PSTR("%u.%u.%u.%u"), Settings.Controller_IP[0],
                                           Settings.Controller_IP[1],
                                           Settings.Controller_IP[2],
                                           Settings.Controller_IP[3]);
  reply = reply + comma + json_string( F("controllerip"), String(tmp_buff) );


  sprintf_P(tmp_buff, PSTR("%u.%u.%u.%u"), Settings.IP[0],
                                           Settings.IP[1],
                                           Settings.IP[2],
                                           Settings.IP[3]);
  reply = reply + comma + json_string( F("espip"), String(tmp_buff) );

  sprintf_P(tmp_buff, PSTR("%u.%u.%u.%u"), Settings.Gateway[0],
                                           Settings.Gateway[1],
                                           Settings.Gateway[2],
                                           Settings.Gateway[3]);
  reply = reply + comma + json_string( F("espgateway"), String(tmp_buff) );

  sprintf_P(tmp_buff, PSTR("%u.%u.%u.%u"), Settings.Subnet[0],
                                           Settings.Subnet[1],
                                           Settings.Subnet[2],
                                           Settings.Subnet[3]);
  reply = reply + comma + json_string( F("espsubnet"), String(tmp_buff) );

  sprintf_P(tmp_buff, PSTR("%u.%u.%u.%u"), Settings.DNS[0],
                                           Settings.DNS[1],
                                           Settings.DNS[2],
                                           Settings.DNS[3]);
  reply = reply + comma + json_string( F("espdns"), String(tmp_buff) );

  reply = reply + comma + json_string( F("controllerhostname"), String(Settings.ControllerHostName));
  reply = reply + comma + json_string( F("delay"), String(Settings.Delay));
  reply = reply + comma + json_string( F("deepsleep"), String(Settings.deepSleep));
  reply = reply + comma + json_string( F("controllerport"), String( Settings.ControllerPort) );

  reply = reply + comma + json_string( F("ssid"), String( SecuritySettings.WifiSSID)) +
                  comma + json_string( F("key"), String( SecuritySettings.WifiKey)) +
                  comma + json_string( F("apkey"), String( SecuritySettings.WifiAPKey)) +
                  comma + json_string( F("controllerpassword"), String( SecuritySettings.ControllerPassword)) +
                  comma + json_string( F("controlleruser"), String( SecuritySettings.ControllerUser));

  reply = reply + F("}");

  WebServer.send(200, "application/json", reply);

} // handle_api_config


//********************************************************************************
// api [GET] hardware json
// @return json
//********************************************************************************
void handle_api_hardware() {

  if ( WebServer.method() == HTTP_POST ) save_hardware();  // shared with WEB HTML

  String reply = F("{");
  String comma = F(",");

  reply = reply +         json_string( F("psda"), String(Settings.Pin_i2c_sda) );
  reply = reply + comma + json_string( F("pscl"), String(Settings.Pin_i2c_scl) );
  reply = reply + comma + json_string( F("pled"), String(Settings.Pin_status_led) );

  for (byte x = 0; x < 17; x++) {
    if ( x == 1 || x == 3 || x == 6 || x == 7 || x == 8 || x == 11 ) continue;
    reply = reply + comma + json_string( "p" + String(x), String(Settings.PinStates[x]) );
  }

  reply = reply + F("}");

  WebServer.send(200, "application/json", reply);

} // handle_api_hardware

//********************************************************************************
// api [GET] devices json
// @return json
//********************************************************************************
void handle_api_devices() {

  char tmpString[41];
  struct EventStruct TempEvent;

  // open json
  String reply = F("[");
  String comma = F("{");

  String deviceName;
  byte DeviceIndex = 0;

  for (byte x = 0; x < TASKS_MAX; x++)
  {
    LoadTaskSettings(x);
    DeviceIndex = getDeviceIndex(Settings.TaskDeviceNumber[x]);
    TempEvent.TaskIndex = x;

    if (ExtraTaskSettings.TaskDeviceValueNames[0][0] == 0)
      PluginCall(PLUGIN_GET_DEVICEVALUENAMES, &TempEvent, dummyString);

    deviceName = "";
    if (Settings.TaskDeviceNumber[x] != 0)
      Plugin_ptr[DeviceIndex](PLUGIN_GET_DEVICENAME, &TempEvent, deviceName);

    if ( deviceName.length() == 0 ) {
      continue;
    }

    reply += comma + json_string( F("DeviceId"), String( x + 1 ) );
    comma = F(",");
    reply += comma + json_string( F("DeviceName"), deviceName );
    reply += comma + json_string( F("TaskDeviceName"), String(ExtraTaskSettings.TaskDeviceName) );

    byte customConfig = false;
    String plgConfig = "";
    customConfig = PluginCall(PLUGIN_WEBFORM_SHOW_CONFIG, &TempEvent, plgConfig);
    if (plgConfig.length() > 0) {
      reply += comma;
      reply += json_string( F("PluginConfig"), plgConfig );
    }

    if (!customConfig)
      if (Device[DeviceIndex].Ports != 0) {
        reply += comma + json_string( F("TaskDevicePort"), String(Settings.TaskDevicePort[x]) );
      }

    if (Settings.TaskDeviceID[x] != 0) {
      reply += comma + json_string( F("TaskDeviceID"), String(Settings.TaskDeviceID[x]) );
    }

    if (Settings.TaskDeviceDataFeed[x] == 0) {
      reply += comma + F("\"DevicePins\":[");

      comma = "";

      if (Device[DeviceIndex].Type == DEVICE_TYPE_I2C)  {
        reply += comma + Settings.Pin_i2c_sda;
        comma = F(",");
        reply += comma + Settings.Pin_i2c_scl;
      }
      if (Device[DeviceIndex].Type == DEVICE_TYPE_ANALOG) {
        reply += comma + 1; // ADC always 1
        comma = F(",");
      }
      if (Settings.TaskDevicePin1[x] != -1)  {
        String s = String( Settings.TaskDevicePin1[x] );
        if (s.length() > 0) {
          reply += comma + s;
          comma = F(",");
        }
      }
      if (Settings.TaskDevicePin2[x] != -1) {
        reply += comma + Settings.TaskDevicePin2[x];
        comma = F(",");
      }
      if (Settings.TaskDevicePin3[x] != -1) {
        reply += comma + Settings.TaskDevicePin3[x];
        comma = F(",");
      }
      reply = reply + F("]");
    }
    comma = F(",");
    byte customValues = false;
    String plgValues = "";
    customValues = PluginCall(PLUGIN_WEBFORM_SHOW_VALUES, &TempEvent, plgValues);
    if (plgValues.length() > 0) {
      reply += comma + json_string( F("PluginCustomValues"), plgValues );
    }
    if (!customValues)
    {
      reply = reply + comma + F("\"Tasks\":[");
      for (byte varNr = 0; varNr < VARS_PER_TASK; varNr++) {
        if ((Settings.TaskDeviceNumber[x] != 0) and (varNr < Device[DeviceIndex].ValueCount)) {
          if (varNr > 0) {
            reply += comma;
          }
          reply = reply + F("{") + json_string( F("TaskDeviceValueName"), String( ExtraTaskSettings.TaskDeviceValueNames[varNr]) );
          reply = reply + comma  + json_string( F("TaskDeviceValue")    , String( UserVar[x * VARS_PER_TASK + varNr]           ) );
          reply = reply + F("}");
        }
      }
      reply = reply + F("]");
    }
    reply = reply + F("}");
    comma = F(",{");
  }

  reply = reply + F("]");

  WebServer.send(200, "application/json", reply);

} // handle_api_devices


//********************************************************************************
// API [GET] device
// @return [json]
//********************************************************************************
void handle_api_device() {

  String taskindex;
  if ( WebServer.hasArg("index") ) {
    taskindex  = WebServer.arg("index");
  } else {
    WebServer.send(500);
    return;
  }

  if ( WebServer.method() == HTTP_POST ) device_save();  // shared with WEB HTML

  struct EventStruct TempEvent;
  byte index = taskindex.toInt();
  index --;
  byte DeviceIndex = getDeviceIndex(Settings.TaskDeviceNumber[index]);
  TempEvent.TaskIndex = index;

  LoadTaskSettings(index);

  if (ExtraTaskSettings.TaskDeviceValueNames[0][0] == 0) {
    PluginCall(PLUGIN_GET_DEVICEVALUENAMES, &TempEvent, dummyString);
  }


  String deviceName = "";
  if (Settings.TaskDeviceNumber[index] != 0) {
      Plugin_ptr[DeviceIndex](PLUGIN_GET_DEVICENAME, &TempEvent, deviceName);
  }

  String reply = F("{");
  String comma = F(",");

  reply = reply +         json_string(F("taskdevicenumber"),       String( Settings.TaskDeviceNumber[index] )      ); // byte [TASKS_MAX]
  reply = reply + comma + json_string(F("devicename"),             deviceName                                      ); // String
  reply = reply + comma + json_string(F("index"),                  String( index + 1 )                             ); // byte  [TASKS_MAX]
  reply = reply + comma + json_string(F("taskdevicename"),         String( ExtraTaskSettings.TaskDeviceName )      ); // char[26]
  reply = reply + comma + json_string(F("taskdeviceid"),           String( Settings.TaskDeviceID[index] )          ); // unsigned int [TASKS_MAX]
  reply = reply + comma + json_string(F("taskdevicepin1"),         String( Settings.TaskDevicePin1[index] )        ); // int8_t [TASKS_MAX]
  reply = reply + comma + json_string(F("taskdevicepin2"),         String( Settings.TaskDevicePin2[index] )        ); // int8_t [TASKS_MAX]
  reply = reply + comma + json_string(F("taskdevicepin3"),         String( Settings.TaskDevicePin3[index] )        ); // int8_t [TASKS_MAX]
  reply = reply + comma + json_string(F("taskdevicepin1pullup"),   String( Settings.TaskDevicePin1PullUp[index] )  ); // boolean [TASKS_MAX]
  reply = reply + comma + json_string(F("taskdevicepin1inversed"), String( Settings.TaskDevicePin1Inversed[index] )); // boolean [TASKS_MAX]
  reply = reply + comma + json_string(F("taskdeviceport"),         String( Settings.TaskDevicePort[index]         )); // byte [TASKS_MAX]
  reply = reply + comma + json_string(F("taskdevicesenddata"),     String( Settings.TaskDeviceSendData[index]     )); // boolean [TASKS_MAX]
  reply = reply + comma + json_string(F("taskdeviceglobalsync"),   String( Settings.TaskDeviceGlobalSync[index]   )); // boolean [TASKS_MAX]

  for (byte varNr = 0; varNr < VARS_PER_TASK; varNr++) {
    if (varNr < Device[DeviceIndex].ValueCount) {
      String arg = F("taskdeviceformula"); // char TaskDeviceFormula[VARS_PER_TASK][41];
      arg += varNr + 1;
      reply = reply + comma + json_string( arg , String( ExtraTaskSettings.TaskDeviceFormula[varNr] ));

      arg = F("taskdevicevaluename"); // char TaskDeviceValueNames[VARS_PER_TASK][26]
      arg += varNr + 1;
      reply = reply + comma + json_string( arg , String( ExtraTaskSettings.TaskDeviceValueNames[varNr] ));

      arg = F("taskdevicevalue");  // float UserVar[VARS_PER_TASK * TASKS_MAX];
      arg += varNr + 1;
      reply = reply + comma + json_string( arg , String( UserVar[index * VARS_PER_TASK + varNr]  ));
    }
  }

  if (Device[DeviceIndex].TimerOption) {
     reply = reply + comma + json_string( F("taskdevicetimer"), String( Settings.TaskDeviceTimer[index] ) );
  }


  String selectTemplate = "";
  PluginCall(PLUGIN_WEBFORM_LOAD, &TempEvent, selectTemplate);

  reply = reply + comma + json_string( F("template"), selectTemplate );

  reply = reply + F("}");

  WebServer.send(200, "application/json", reply);

} // handle_api_device


//********************************************************************************
// API [GET] root
// @return [json]
//********************************************************************************
void handle_api_root() {

  String reply = F("{");
  String comma = F(",");

  reply = reply + json_string( F("Name"), String(Settings.Name) );

  reply = reply + comma + json_string( F("FreeMem"), String( ESP.getFreeHeap() ) );

#if FEATURE_TIME
  if (Settings.UseNTP) {
    String time = String(hour());
    time += ":";
    if (minute() < 10)
      time += "0";
    time += minute();
    reply = reply + comma + json_string( F("Time"), time );
  }
#endif

  reply = reply + comma + json_string( F("Uptime"), String(wdcounter / 2) );

  char tmp_buff[80]; // tmp buffer
  IPAddress ip = WiFi.localIP();
  sprintf_P(tmp_buff, PSTR("%u.%u.%u.%u"), ip[0], ip[1], ip[2], ip[3]);
  reply = reply + comma + json_string( F("IP"), String(tmp_buff) );

  IPAddress gw = WiFi.gatewayIP();
  sprintf_P(tmp_buff, PSTR("%u.%u.%u.%u"), gw[0], gw[1], gw[2], gw[3]);
  reply = reply + comma + json_string( F("Gateway"), String(tmp_buff) );

  reply = reply + comma + json_string( F("Build"), String(BUILD));
  reply = reply + comma + json_string( F("Unit"), String(Settings.Unit));
  uint8_t mac[] = {0, 0, 0, 0, 0, 0};
  uint8_t* macread = WiFi.macAddress(mac);
  sprintf_P(tmp_buff, PSTR("%02x:%02x:%02x:%02x:%02x:%02x"), macread[0], macread[1], macread[2], macread[3], macread[4], macread[5]);
  reply = reply + comma + json_string( F("STA_MAC"), String(tmp_buff) );

  macread = WiFi.softAPmacAddress(mac);
  sprintf_P(tmp_buff, PSTR("%02x:%02x:%02x:%02x:%02x:%02x"), macread[0], macread[1], macread[2], macread[3], macread[4], macread[5]);
  reply = reply + comma + json_string( F("AP_MAC"), String(tmp_buff));

  reply = reply + comma + json_string( F("Chip_id"), String(ESP.getChipId()));
  reply = reply + comma + json_string( F("Flash_Chip_id"), String(ESP.getFlashChipId()));
  reply = reply + comma + json_string( F("Flash_Size"), String(ESP.getFlashChipRealSize()/1024));
  reply = reply + comma + json_string( F("Boot_cause"), String(lastBootCause) ) + comma;
  reply = reply + F("\"NodeList\":[");

  comma  = F("{");

  for (byte x = 0; x < UNIT_MAX; x++)  {
    if (Nodes[x].ip[0] != 0)    {
      sprintf_P(tmp_buff, PSTR("%u.%u.%u.%u"), Nodes[x].ip[0], Nodes[x].ip[1], Nodes[x].ip[2], Nodes[x].ip[3]);
      reply = reply + comma;
      reply = reply + json_string( F("Unit"), String(x) );
      reply = reply + comma + json_string( F("Url"), String(tmp_buff) );
      reply = reply + comma + json_string( F("Age"), String(Nodes[x].age) );
      reply = reply + F("\"}");
      comma  = F(",{");
    }
  }

  reply = reply + F("]}");

  WebServer.send(200, "application/json", reply );
} // handle_api_root


//*****************************************************************************
// Log api
//*****************************************************************************
void handle_api_log() {

  bool comma = false;
  String reply = F("[");

  if (logcount != -1) {
    byte counter = logcount;
    do {
      counter++;
      if (counter > 9)  counter = 0;
      if (Logging[counter].timeStamp > 0) {
        if (comma) reply = reply + F(",");
        reply = reply + F("{") + json_string( F("timeStamp"), String( Logging[counter].timeStamp) );
        reply = reply + F(",") + json_string( F("Message"),   Logging[counter].Message );
        reply = reply + F("}");
        comma = true;
      }
    }  while (counter != logcount);
  }

  reply = reply + F("]");
  WebServer.send(200, "application/json", reply );

} // handle_api_log


//********************************************************************************
// json I2C scanner
//********************************************************************************
void handle_api_i2c() {

  String reply = "[";
  byte error, address, nDevices;
  nDevices = 0;
  String comma = "";

  for (address = 1; address <= 127; address++ )
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      reply += comma;
      reply = reply + F("{") + json_string( F("address"), String(address, HEX) );
      reply = reply + F(", \"device\":\"");
      comma = F(",");
      switch (address) {
        case 0x20:
        case 0x27:
        case 0x3F:
          reply = reply + F("PCF8574, MCP23017, LCD Modules");
          break;
        case 0x23:
          reply = reply + F("BH1750 Lux Sensor");
          break;
        case 0x24:
          reply = reply + F("PN532 RFID Reader");
          break;
        case 0x39:
          reply = reply + F("TLS2561 Lux Sensor");
          break;
        case 0x3C:
        case 0x3D:
          reply = reply + F("OLED SSD1306 Display");
          break;
        case 0x40:
          reply = reply + F("SI7021 Temp/Hum Sensor, INA219");
          break;
        case 0x48:
          reply = reply + F("PCF8591 ADC");
          break;
        case 0x68:
          reply = reply + F("DS1307 RTC");
          break;
        case 0x76:
          reply = reply + F("BME280");
          break;
        case 0x77:
          reply = reply + F("BMP085");
          break;
        case 0x7f:
          reply = reply + F("Arduino Pro Mini IO Extender");
          break;
      }
      reply = reply + F("\"}");
      nDevices++;
    }
  }

  reply = reply + F("]");
  WebServer.send(200, "application/json", reply );
} // handle_api_i2c

//********************************************************************************
// [post] handle_api_cmd
//********************************************************************************
void handle_api_cmd() {

  if ( WebServer.method() == HTTP_GET ) {
    WebServer.send(500); // GET not allowed
    return;
  }

  String webrequest;
  if ( WebServer.hasArg("c") ) {
    webrequest = WebServer.arg("c");
  } else {
    WebServer.send(500); // argument needed
    return;
  }


  String reply = "{\"resp\":\"";
  printToWeb = true;
  printWebString = "";

  struct EventStruct TempEvent;
  parseCommandString(&TempEvent, webrequest);
  if (PluginCall(PLUGIN_WRITE, &TempEvent, webrequest)) {
    // TODO
  } else {
    ExecuteCommand(webrequest.c_str());
  }

  reply += printWebString;
  reply += F("\"}");

  WebServer.send(200, "application/json", reply );
} // handle_api_cmd

//********************************************************************************
// handle_api_advanced
//********************************************************************************
void handle_api_advanced() {

  if ( WebServer.method() == HTTP_POST ) save_advanced();  // shared with WEB HTML

  String reply = F("{");
  String comma = F(",");

  reply = reply +         json_string(F("mqttsubscribe"), String(Settings.MQTTsubscribe));
  reply = reply + comma + json_string(F("mqttpublish"  ), String(Settings.MQTTpublish));
  reply = reply + comma + json_string(F("messagedelay" ), String(Settings.MessageDelay));
  reply = reply + comma + json_string(F("ip"           ), String(Settings.IP_Octet));

#if FEATURE_TIME
  reply = reply + comma + json_string(F("usentp"   ), String(Settings.UseNTP));
  reply = reply + comma + json_string(F("ntphost"  ), String(Settings.NTPHost));
  reply = reply + comma + json_string(F("timezone" ), String(Settings.TimeZone));
  reply = reply + comma + json_string(F("dst"      ), String(Settings.DST));
#endif // FEATURE_TIME

  char str[20];
  str[0] = 0;
  sprintf_P(str, PSTR("%u.%u.%u.%u"), Settings.Syslog_IP[0], Settings.Syslog_IP[1], Settings.Syslog_IP[2], Settings.Syslog_IP[3]);

  reply = reply + comma + json_string(F("syslogip"       ), String(str));
  reply = reply + comma + json_string(F("sysloglevel"    ), String(Settings.SyslogLevel));
  reply = reply + comma + json_string(F("udpport"        ), String(Settings.UDPPort));
  reply = reply + comma + json_string(F("useserial"      ), String(Settings.UseSerial));
  reply = reply + comma + json_string(F("serialloglevel" ), String(Settings.SerialLogLevel));
  reply = reply + comma + json_string(F("webloglevel"    ), String(Settings.WebLogLevel));
  reply = reply + comma + json_string(F("baudrate"       ), String(Settings.BaudRate));
  reply = reply + comma + json_string(F("wdi2caddress"   ), String(Settings.WDI2CAddress));
  reply = reply + comma + json_string(F("usessdp"        ), String(Settings.UseSSDP));

  #if !FEATURE_SPIFFS
    reply = reply + comma + json_string(F("customcss"), String(Settings.CustomCSS));
  #endif // !FEATURE_SPIFFS

  reply = reply + F("}");
  WebServer.send(200, "application/json", reply );

} // handle_api_advanced

//********************************************************************************
// [get] handle_api_protocols
//********************************************************************************
void api_protocols() {

  String reply = F("[");
  String comma = F(",");

  for (byte x = 0; x <= protocolCount; x++)  {
    if ( x > 0 ) {
      reply = reply + F(",{");
    } else {
      reply = reply + F("{");
    }
    String ProtocolName = "";
    CPlugin_ptr[x](CPLUGIN_GET_DEVICENAME, 0, ProtocolName);

    reply = reply         + json_string(F("Number"     ), String(Protocol[x].Number      ) );
    reply = reply + comma + json_string(F("Name"       ), String(ProtocolName            ) );
    reply = reply + comma + json_string(F("MQTT"       ), String(Protocol[x].usesMQTT    ) );
    reply = reply + comma + json_string(F("Account"    ), String(Protocol[x].usesAccount ) );
    reply = reply + comma + json_string(F("Password"   ), String(Protocol[x].usesPassword) );
    reply = reply + comma + json_string(F("defaultPort"), String(Protocol[x].defaultPort ) );
    reply = reply + F("}");
    WebServer.sendContent(reply);
    reply = "";
  }

  reply = reply + F("]");
  WebServer.sendContent(reply);

} // api_protocols


//********************************************************************************
// [get] api_tasks
//********************************************************************************
void api_tasks() {

  struct EventStruct TempEvent;
  String reply = F("[");
  String comma = F(",");
  String deviceName;

  for (byte x = 0; x <= deviceCount ; x++) {

    if (x > 0) reply += comma;

    deviceName = "";
    Plugin_ptr[x](PLUGIN_GET_DEVICENAME, &TempEvent, deviceName);

    reply = reply + F("[")+ Device[x].Number             ; // byte Number;
    reply = reply + comma + Device[x].Type               ; // byte Type;
    reply = reply + comma + Device[x].VType              ; // byte VType;
    reply = reply + comma + Device[x].Ports              ; // byte Ports;
    reply = reply + comma + Device[x].PullUpOption       ; // boolean PullUpOption;
    reply = reply + comma + Device[x].InverseLogicOption ; // boolean InverseLogicOption;
    reply = reply + comma + Device[x].FormulaOption      ; // boolean FormulaOption;
    reply = reply + comma + Device[x].ValueCount         ; // byte ValueCount;
    reply = reply + comma + Device[x].Custom             ; // boolean Custom;
    reply = reply + comma + Device[x].SendDataOption     ; // boolean SendDataOption;
    reply = reply + comma + Device[x].GlobalSyncOption   ; // boolean GlobalSyncOption;
    reply = reply + comma + Device[x].TimerOption        ; // boolean TimerOption;
    reply = reply + comma + "\"" + deviceName + "\""     ; // boolean TimerOption;
    reply = reply + F("]");
    WebServer.sendContent(reply);
    reply = "";
  }

  reply = reply + F("]");

  WebServer.sendContent(reply);

} // api_tasks


//********************************************************************************
// API [GET]
// @return 3 x [json]
//********************************************************************************
void handle_api_main() {
  WebServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
  WebServer.sendHeader("Connection", "close");
  WebServer.send(200, "application/json", "");
  WebServer.sendContent(F("{\"Protocols\":"));
  api_protocols();
  WebServer.sendContent(F(",\"Tasks\":"));
  api_tasks();
  WebServer.sendContent("}");
}

//********************************************************************************
// main app
//********************************************************************************
void handle_app() {
  WebServer.send_P(200, "text/html", app_html, app_html_len);
} // handle_app

//********************************************************************************
// handle_dev_temp
//********************************************************************************
void handle_dev_temp() {

  String taskindex;
  if ( WebServer.hasArg("index") ) {
    taskindex  = WebServer.arg("index");
  } else {
    WebServer.send(500);
    return;
  }

  if ( WebServer.method() == HTTP_POST ) device_save();  // shared with WEB HTML

  String reply="";
  struct EventStruct TempEvent;
  byte index = taskindex.toInt();
  byte DeviceIndex = getDeviceIndex(Settings.TaskDeviceNumber[index - 1]);

    LoadTaskSettings(index - 1);
    TempEvent.TaskIndex = index - 1;

    if (ExtraTaskSettings.TaskDeviceValueNames[0][0] == 0)
      PluginCall(PLUGIN_GET_DEVICEVALUENAMES, &TempEvent, dummyString);

    reply += F("{\"template\":\"<form name='frmselect' method='post'><table><TH>Task Settings<TH>Value");

    reply += F("<TR><TD>Device:<TD>");
    //addDeviceSelect(reply, "taskdevicenumber", Settings.TaskDeviceNumber[index - 1]);

    if (Settings.TaskDeviceNumber[index - 1] != 0 )
    {
      reply += F("<a class='button-link' href='http://www.esp8266.nu/index.php/plugin");
      reply += Settings.TaskDeviceNumber[index - 1];
      reply += F("' target='_blank'>?</a>");

      reply += F("<TR><TD>Name:<TD><input type='text' maxlength='25' name='taskdevicename' value='");
      reply += ExtraTaskSettings.TaskDeviceName;
      reply += F("'>");

      if (Device[DeviceIndex].TimerOption)
      {
        reply += F("<TR><TD>Delay:<TD><input type='text' name='taskdevicetimer' value='");
        reply += Settings.TaskDeviceTimer[index - 1];
        reply += F("'>");
      }

      if (!Device[DeviceIndex].Custom)
      {
        reply += F("<TR><TD>IDX / Var:<TD><input type='text' name='taskdeviceid' value='");
        reply += Settings.TaskDeviceID[index - 1];
        reply += F("'>");
      }

      if (!Device[DeviceIndex].Custom && Settings.TaskDeviceDataFeed[index - 1] == 0)
      {
        if (Device[DeviceIndex].Ports != 0)
        {
          reply += F("<TR><TD>Port:<TD><input type='text' name='taskdeviceport' value='");
          reply += Settings.TaskDevicePort[index - 1];
          reply += F("'>");
        }

        if (Device[DeviceIndex].Type == DEVICE_TYPE_SINGLE || Device[DeviceIndex].Type == DEVICE_TYPE_DUAL)
        {
          reply += F("<TR><TD>1st GPIO:<TD>");
          addPinSelect(false, reply, "taskdevicepin1", Settings.TaskDevicePin1[index - 1]);
        }
        if (Device[DeviceIndex].Type == DEVICE_TYPE_DUAL)
        {
          reply += F("<TR><TD>2nd GPIO:<TD>");
          addPinSelect(false, reply, "taskdevicepin2", Settings.TaskDevicePin2[index - 1]);
        }

        if (Device[DeviceIndex].PullUpOption)
        {
          reply += F("<TR><TD>Pull UP:<TD>");
          if (Settings.TaskDevicePin1PullUp[index - 1])
            reply += F("<input type=checkbox name=taskdevicepin1pullup checked>");
          else
            reply += F("<input type=checkbox name=taskdevicepin1pullup>");
        }

        if (Device[DeviceIndex].InverseLogicOption)
        {
          reply += F("<TR><TD>Inversed:<TD>");
          if (Settings.TaskDevicePin1Inversed[index - 1])
            reply += F("<input type=checkbox name=taskdevicepin1inversed checked>");
          else
            reply += F("<input type=checkbox name=taskdevicepin1inversed>");
        }
      }

      PluginCall(PLUGIN_WEBFORM_LOAD, &TempEvent, reply);

      if (Device[DeviceIndex].SendDataOption)
      {
        reply += F("<TR><TD>Send Data:<TD>");
        if (Settings.TaskDeviceSendData[index - 1])
          reply += F("<input type=checkbox name=taskdevicesenddata checked>");
        else
          reply += F("<input type=checkbox name=taskdevicesenddata>");
      }

      if (Device[DeviceIndex].GlobalSyncOption && Settings.TaskDeviceDataFeed[index - 1] == 0 && Settings.UDPPort != 0)
      {
        reply += F("<TR><TD>Global Sync:<TD>");
        if (Settings.TaskDeviceGlobalSync[index - 1])
          reply += F("<input type=checkbox name=taskdeviceglobalsync checked>");
        else
          reply += F("<input type=checkbox name=taskdeviceglobalsync>");
      }

      if (!Device[DeviceIndex].Custom)
      {
        reply += F("<TR><TH>Optional Settings<TH>Value");

        if (Device[DeviceIndex].FormulaOption)
        {
          for (byte varNr = 0; varNr < Device[DeviceIndex].ValueCount; varNr++)
          {
            reply += F("<TR><TD>Formula ");
            reply += ExtraTaskSettings.TaskDeviceValueNames[varNr];
            reply += F(":<TD><input type='text' maxlength='25' name='taskdeviceformula");
            reply += varNr + 1;
            reply += F("' value='");
            reply += ExtraTaskSettings.TaskDeviceFormula[varNr];
            reply += F("'>");
            if (varNr == 0)
              reply += F("<a class='button-link' href='http://www.esp8266.nu/index.php/EasyFormula' target='_blank'>?</a>");
          }
        }

        for (byte varNr = 0; varNr < Device[DeviceIndex].ValueCount; varNr++)
        {
          reply += F("<TR><TD>Value Name ");
          reply += varNr + 1;
          reply += F(":<TD><input type='text' maxlength='25' name='taskdevicevaluename");
          reply += varNr + 1;
          reply += F("' value='");
          reply += ExtraTaskSettings.TaskDeviceValueNames[varNr];
          reply += F("'>");
        }
      }

    }
    reply += F("<TR><TD><TD>");
    reply += F("<input type='submit' value='Submit'>");
    reply += F("<input type='hidden' name='edit' value='1'>");
    reply += F("</table></form>\"}");
    WebServer.send(200, "application/json", reply );
}
#endif // FEATURE_API
