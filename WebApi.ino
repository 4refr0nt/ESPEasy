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
// @return "name":"value" from name, value
String json_string( String name, String value  ) {
  return "\"" + name + "\"" + ":" + "\"" + value + "\"";
}

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
//********************************************************************************
// API main handler
//********************************************************************************
void handle_api() {

  if (!isLoggedInApi()) return;

  if ( WebServer.method() == HTTP_OPTIONS ) {
    handle_api_options(); // CORS
    return;
  }

  byte query;

  if ( WebServer.hasArg("q") ) {
    query = WebServer.arg("q").toInt();
  } else {
    WebServer.send(500);
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
    case 10: // protocols [get]
            handle_api_protocols();
            break;
    default:
         WebServer.send(500);
    }
}
//********************************************************************************
// API [OPTIONS]
// return headers only, CORS
// @return null
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
      reply = reply + F(",") + json_string( F("RSSI"), String( WiFi.RSSI(i) )) + F("}");
    } // for
  } // else (n == 0)
  reply = reply + F("]");
  WebServer.send(200, "application/json", reply);

} // handle_api_wifi

//********************************************************************************
// API [GET][POST] config
// @return [json] config
// {
//    ControllerHostName: "mqtt.42do.ru"
//    ControllerPassword: "Adm1n1strat0r"
//    ControllerPort: "20000"
//    ControllerUser: "PassW0rd"
//    Controller_IP: [0, 0, 0, 0]
//    DNS: [0, 0, 0, 0]
//    Delay: "60"
//    IP: [0, 0, 0, 0]
//    Name: "NewTestDevice"
//    Protocol: "2"
//    Subnet: [0, 0, 0, 0]
//    Unit: "0"
//    WifiAPKey: "IoT"
//    WifiKey: "WeriSecurPass"
//    WifiSSID: "Trollstigen"
//    deepSleep: "0"
// }
//********************************************************************************
void handle_api_config() {

  if ( WebServer.method() == HTTP_POST ) save_config();  // shared with WEB HTML

  String reply = F("{");

  reply += json_string( F("name"), Settings.Name) +
           F(",") + json_string( F("usedns"), String(Settings.UseDNS) ) +
           F(",") + json_string( F("unit"), String(Settings.Unit) ) +
           F(",") + json_string( F("protocol"), String(Settings.Protocol)) +
           F(",") + json_array( F("controllerip"),
                              String(Settings.Controller_IP[0]) + F(",") +
                              String(Settings.Controller_IP[1]) + F(",") +
                              String(Settings.Controller_IP[2]) + F(",") +
                              String(Settings.Controller_IP[3])
                            ) +
           F(",") + json_array( F("espip"),
                              String(Settings.IP[0]) + F(",") +
                              String(Settings.IP[1]) + F(",") +
                              String(Settings.IP[2]) + F(",") +
                              String(Settings.IP[3])
                            ) +
           F(",") + json_array( F("espsubnet"),
                              String(Settings.Subnet[0]) + F(",") +
                              String(Settings.Subnet[1]) + F(",") +
                              String(Settings.Subnet[2]) + F(",") +
                              String(Settings.Subnet[3])
                            ) +
           F(",") + json_array( F("espdns"),
                              String(Settings.DNS[0]) + F(",") +
                              String(Settings.DNS[1]) + F(",") +
                              String(Settings.DNS[2]) + F(",") +
                              String(Settings.DNS[3])
                            ) +
           F(",") + json_array( F("espgateway"),
                              String(Settings.Gateway[0]) + F(",") +
                              String(Settings.Gateway[1]) + F(",") +
                              String(Settings.Gateway[2]) + F(",") +
                              String(Settings.Gateway[3])
                            ) +
           F(",") + json_string( F("controllerhostname"), Settings.ControllerHostName) +
           F(",") + json_string( F("delay"), String(Settings.Delay)) +
           F(",") + json_string( F("deepsleep"), String(Settings.deepSleep)) +
           F(",") + json_string( F("controllerport"), String( Settings.ControllerPort) )
           ;

  // SecuritySettings
  reply = reply + F(",") + json_string( F("ssid"), SecuritySettings.WifiSSID) +
           F(",") + json_string( F("key"), SecuritySettings.WifiKey) +
           F(",") + json_string( F("apkey"), SecuritySettings.WifiAPKey) +
           F(",") + json_string( F("controllerpassword"), SecuritySettings.ControllerPassword) +
           F(",") + json_string( F("controlleruser"), SecuritySettings.ControllerUser);

  // close json
  reply = reply + F("}\n");

  // debug
  WebServer.send(200, "application/json", reply);

} // handle_api_config

//********************************************************************************
// api [GET] hardware json
// @return json
//********************************************************************************
void handle_api_hardware() {

  if ( WebServer.method() == HTTP_POST ) save_hardware();  // shared with WEB HTML

  String reply = F("{");

  reply += json_string( F("psda"), String(Settings.Pin_i2c_sda) );
  reply = reply + F(",") + json_string( F("pscl"), String(Settings.Pin_i2c_scl) );
  for (byte x = 0; x < 17; x++) {
    reply = reply + F(",") + json_string( "p" + String(x), String(Settings.PinStates[x]) );
  }

  reply = reply + F("}\n");

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

    reply += comma;
    reply += json_string( F("DeviceId"), String(x) );
    comma = F(",");

    reply += comma;
    reply += json_string( F("DeviceName"), deviceName );

    reply += comma;
    reply += json_string( F("TaskDeviceName"), ExtraTaskSettings.TaskDeviceName );

    byte customConfig = false;
    String plgConfig = "";
    customConfig = PluginCall(PLUGIN_WEBFORM_SHOW_CONFIG, &TempEvent, plgConfig);
    if (plgConfig.length() > 0) {
      reply += comma;
      reply += json_string( F("PluginConfig"), plgConfig );
    }
    if (!customConfig)
      if (Device[DeviceIndex].Ports != 0) {
        reply += comma;
        reply += json_string( F("TaskDevicePort"), String(Settings.TaskDevicePort[x]) );
      }

    if (Settings.TaskDeviceID[x] != 0) {
      reply += comma;
      reply += json_string( F("TaskDeviceID"), String(Settings.TaskDeviceID[x]) );
    }

    /////////////// PINS START
    if (Settings.TaskDeviceDataFeed[x] == 0) {
      reply += comma;
      reply = reply + F("\"DevicePins\":[");
      comma = "";
      if (Device[DeviceIndex].Type == DEVICE_TYPE_I2C)  {
        reply += comma;
        reply += Settings.Pin_i2c_sda;
        comma = F(",");
        reply += comma;
        reply += Settings.Pin_i2c_scl;
      }
      if (Device[DeviceIndex].Type == DEVICE_TYPE_ANALOG) {
        reply += comma;
        reply += 1; // ADC always 1
        comma = F(",");
      }
      if (Settings.TaskDevicePin1[x] != -1)  {
        String s = String(Settings.TaskDevicePin1[x]);
        if (s.length() > 0) {
          reply += comma;
          reply += s;
          comma = F(",");
        }
      }
      if (Settings.TaskDevicePin2[x] != -1) {
        reply += comma;
        reply += Settings.TaskDevicePin2[x];
        comma = F(",");
      }
      if (Settings.TaskDevicePin3[x] != -1) {
        reply += comma;
        reply = reply + F("\"PIN\":");
        reply += Settings.TaskDevicePin3[x];
        comma = F(",");
      }
      reply = reply + F("]");
    }
    /////////////// PINS END
    byte customValues = false;
    String plgValues = "";
    customValues = PluginCall(PLUGIN_WEBFORM_SHOW_VALUES, &TempEvent, plgValues);
    if (plgValues.length() > 0) {
      reply += comma;
      reply += json_string( F("PluginCustomValues"), plgValues );
    }
    if (!customValues)
    {
      reply = reply + F(",\"Tasks\":[");
      for (byte varNr = 0; varNr < VARS_PER_TASK; varNr++) {
        if ((Settings.TaskDeviceNumber[x] != 0) and (varNr < Device[DeviceIndex].ValueCount)) {
          if (varNr > 0) {
            reply += comma;
          }
          reply = reply + F("{\"TaskDeviceValueName\":\"");
          reply += ExtraTaskSettings.TaskDeviceValueNames[varNr];
          reply = reply + F("\",\"TaskDeviceValue\":\"");
          reply += UserVar[x * VARS_PER_TASK + varNr];
          reply = reply + F("\"}");
        }
      }
      reply = reply + F("]");
    }
    reply = reply + F("}");
    comma = F(",{");
  }

  // close json
  reply = reply + F("]");
  // debug
  // Serial.println(reply);
  // send to client
  WebServer.send(200, "application/json", reply);

} // handle_api_devices

/////////////////////////////////////////////////////////////
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
  LoadTaskSettings(index);

  if (ExtraTaskSettings.TaskDeviceValueNames[0][0] == 0) {
    PluginCall(PLUGIN_GET_DEVICEVALUENAMES, &TempEvent, dummyString);
  }

  byte DeviceIndex = getDeviceIndex(Settings.TaskDeviceNumber[index]);

  String deviceName = "";
  if (Settings.TaskDeviceNumber[index] != 0) {
      Plugin_ptr[DeviceIndex](PLUGIN_GET_DEVICENAME, &TempEvent, deviceName);
  }

  String reply = F("{");

  reply +=        json_number(F("taskdevicenumber"),       String( Settings.TaskDeviceNumber[index] )      ); // byte [TASKS_MAX]
  reply = reply + F(",") + json_string(F("devicename"),             deviceName                                      ); // String
  reply = reply + F(",") + json_number(F("index"), index); // byte  [TASKS_MAX]
  reply = reply + F(",") + json_string(F("taskdevicename"),         String( ExtraTaskSettings.TaskDeviceName )      ); // char[26]
  reply = reply + F(",") + json_number(F("taskdeviceid"),           String( Settings.TaskDeviceID[index])           ); // unsigned int [TASKS_MAX]
  reply = reply + F(",") + json_number(F("taskdevicepin1"),         Settings.TaskDevicePin1[index]                  ); // int8_t [TASKS_MAX]
  reply = reply + F(",") + json_number(F("taskdevicepin2"),         Settings.TaskDevicePin2[index]                  ); // int8_t [TASKS_MAX]
  reply = reply + F(",") + json_number(F("taskdevicepin3"),         Settings.TaskDevicePin3[index]                  ); // int8_t [TASKS_MAX]
  reply = reply + F(",") + json_number(F("taskdevicepin1pullup"),   Settings.TaskDevicePin1PullUp[index]            ); // boolean [TASKS_MAX]
  reply = reply + F(",") + json_number(F("taskdevicepin1inversed"), Settings.TaskDevicePin1Inversed[index]          ); // boolean [TASKS_MAX]
  reply = reply + F(",") + json_number(F("taskdeviceport"),         Settings.TaskDevicePort[index]                  ); // byte [TASKS_MAX]
  reply = reply + F(",") + json_number(F("taskdevicesenddata"),     Settings.TaskDeviceSendData[index]              ); // boolean [TASKS_MAX]
  reply = reply + F(",") + json_number(F("taskdeviceglobalsync"),   Settings.TaskDeviceGlobalSync[index]            ); // boolean [TASKS_MAX]

  reply = reply + F(",\"Tasks\":[");
  String comma = F(",");

      for (byte varNr = 0; varNr < VARS_PER_TASK; varNr++) {
        if ((Settings.TaskDeviceNumber[index] != 0) and (varNr < Device[DeviceIndex].ValueCount)) {
          if (varNr > 0) {
            reply += comma;
          }
          reply = reply + F("{\"taskdevicevaluename\":\"");
          reply += ExtraTaskSettings.TaskDeviceValueNames[varNr]; // char TaskDeviceValueNames[VARS_PER_TASK][26]
          reply = reply + F("\"");
          reply += comma;
          reply = reply + F("\"taskdeviceformula\":\"");
          reply += ExtraTaskSettings.TaskDeviceFormula[varNr]; // char TaskDeviceFormula[VARS_PER_TASK][41];
          reply = reply + F("\"");
          reply += comma;
          reply = reply + F("\"taskdevicevalue\":\"");
          reply += UserVar[index * VARS_PER_TASK + varNr]; // float UserVar[VARS_PER_TASK * TASKS_MAX];
          reply = reply + F("\"}");
        }
      }

  reply = reply + F("]}");
  WebServer.send(200, "application/json", reply);

} // handle_api_device


//********************************************************************************
// API [GET] root
// @return [json]
//********************************************************************************
void handle_api_root() {

  String reply = F("{");

  reply = reply + json_string( F("Name"), String(Settings.Name) );

  reply = reply + F(",") + json_string( F("FreeMem"), String( ESP.getFreeHeap() ) );

#if FEATURE_TIME
  if (Settings.UseNTP) {
    String time = String(hour());
    time += ":";
    if (minute() < 10)
      time += "0";
    time += minute();
    reply = reply + F(",") + json_string( F("Time"), time );
  }
#endif

  reply = reply + F(",") + json_string( F("Uptime"), String(wdcounter / 2) );

  char tmp_buff[80]; // tmp buffer
  IPAddress ip = WiFi.localIP();
  sprintf_P(tmp_buff, PSTR("%u.%u.%u.%u"), ip[0], ip[1], ip[2], ip[3]);
  reply = reply + F(",") + json_string( F("IP"), String(tmp_buff) );

  IPAddress gw = WiFi.gatewayIP();
  sprintf_P(tmp_buff, PSTR("%u.%u.%u.%u"), gw[0], gw[1], gw[2], gw[3]);
  reply = reply + F(",") + json_string( F("Gateway"), String(tmp_buff) );

  reply = reply + F(",") + json_string( F("Build"), String(BUILD));
  reply = reply + F(",") + json_string( F("Unit"), String(Settings.Unit));
  uint8_t mac[] = {0, 0, 0, 0, 0, 0};
  uint8_t* macread = WiFi.macAddress(mac);
  sprintf_P(tmp_buff, PSTR("%02x:%02x:%02x:%02x:%02x:%02x"), macread[0], macread[1], macread[2], macread[3], macread[4], macread[5]);
  reply = reply + F(",") + json_string( F("STA_MAC"), String(tmp_buff) );

  macread = WiFi.softAPmacAddress(mac);
  sprintf_P(tmp_buff, PSTR("%02x:%02x:%02x:%02x:%02x:%02x"), macread[0], macread[1], macread[2], macread[3], macread[4], macread[5]);
  reply = reply + F(",") + json_string( F("AP_MAC"), String(tmp_buff));

  reply = reply + F(",") + json_string( F("Chip_id"), String(ESP.getChipId()));
  reply = reply + F(",") + json_string( F("Flash_Chip_id"), String(ESP.getFlashChipId()));
  reply = reply + F(",") + json_string( F("Flash_Size"), String(ESP.getFlashChipRealSize()/1024));
  reply = reply + F(",") + json_string( F("Boot_cause"), String(lastBootCause) ) + F(",");
  reply = reply + F("\"NodeList\":[");

  String comma  = F("{");
  for (byte x = 0; x < UNIT_MAX; x++)  {
    if (Nodes[x].ip[0] != 0)    {
      sprintf_P(tmp_buff, PSTR("%u.%u.%u.%u"), Nodes[x].ip[0], Nodes[x].ip[1], Nodes[x].ip[2], Nodes[x].ip[3]);
      reply = reply + comma;
      reply = reply + json_string( F("Unit"), String(x) );
      reply = reply + F(",") + json_string( F("Url"), String(tmp_buff) );
      reply = reply + F(",") + json_string( F("Age"), String(Nodes[x].age) );
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
        reply = reply + F("{ \"timeStamp\":\"") + Logging[counter].timeStamp + F("\", \"Message\": \"") + Logging[counter].Message + F("\"}");
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
      reply = reply + F("{\"address\":\"");
      reply += String(address, HEX) + F("\", \"device\":\"");
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

  if ( WebServer.method() == HTTP_GET ) WebServer.send(500); // GET not allowed

  String sCommand = WebServer.arg("c");

  String reply = "";
  printToWeb = true;
  printWebString = "";
  ExecuteCommand( sCommand.c_str() );
  reply = reply + F( "{\"resp\":\"" );
  reply = reply + printWebString + F("\"}");
  WebServer.send( 200, "application/json", reply );
  printWebString = "";
  printToWeb = false;

} // handle_api_cmd

//********************************************************************************
// handle_api_advanced
//********************************************************************************
void handle_api_advanced() {

  if ( WebServer.method() == HTTP_POST ) save_advanced();  // shared with WEB HTML

  String reply = "{";

  reply +=       json_string(F("MQTTsubscribe"), String(Settings.MQTTsubscribe));
  reply = reply + F(",") + json_string(F("MQTTpublish"  ), String(Settings.MQTTpublish));
  reply = reply + F(",") + json_string(F("MessageDelay" ), String(Settings.MessageDelay));
  reply = reply + F(",") + json_string(F("IP_Octet"     ), String(Settings.IP_Octet));

#if FEATURE_TIME
  reply = reply + F(",") + json_string(F("UseNTP"   ), String(Settings.UseNTP));
  reply = reply + F(",") + json_string(F("NTPHost"  ), String(Settings.NTPHost));
  reply = reply + F(",") + json_string(F("TimeZone" ), String(Settings.TimeZone));
  reply = reply + F(",") + json_string(F("DST"      ), String(Settings.DST));
#endif // FEATURE_TIME

  char str[20];
  str[0] = 0;
  sprintf_P(str, PSTR("%u.%u.%u.%u"), Settings.Syslog_IP[0], Settings.Syslog_IP[1], Settings.Syslog_IP[2], Settings.Syslog_IP[3]);

  reply = reply + F(",") + json_string(F("Syslog_IP"      ), String(str));
  reply = reply + F(",") + json_string(F("SyslogLevel"    ), String(Settings.SyslogLevel));
  reply = reply + F(",") + json_string(F("UDPPort"        ), String(Settings.UDPPort));
  reply = reply + F(",") + json_string(F("SerialLogLevel" ), String(Settings.SerialLogLevel));
  reply = reply + F(",") + json_string(F("WebLogLevel"    ), String(Settings.WebLogLevel));
  reply = reply + F(",") + json_string(F("BaudRate"       ), String(Settings.BaudRate));
  reply = reply + F(",") + json_string(F("WDI2CAddress"   ), String(Settings.WDI2CAddress));

  #if !FEATURE_SPIFFS
    reply = reply + F(",") + json_string(F("CustomCSS"), String(Settings.CustomCSS));
  #endif // !FEATURE_SPIFFS

  reply = reply + F("}");
  WebServer.send(200, "application/json", reply );

} // handle_api_advanced

//********************************************************************************
// [get] handle_api_protocols
//********************************************************************************
void handle_api_protocols() {

  String reply = F("[");
  for (byte x = 0; x <= protocolCount; x++)  {
    if ( x > 0 ) {
      reply = reply + F(",{");
    } else {
      reply = reply + F("{");
    }
    reply = reply          + json_string(F("Number"     ), String(Protocol[x].Number      ) );
//    reply = reply + F(",") + json_string(F("Name"       ), String(Protocol[x].Name        ) );
    reply = reply + F(",") + json_string(F("MQTT"       ), String(Protocol[x].usesMQTT    ) );
    reply = reply + F(",") + json_string(F("Account"    ), String(Protocol[x].usesAccount ) );
    reply = reply + F(",") + json_string(F("Password"   ), String(Protocol[x].usesPassword) );
    reply = reply + F(",") + json_string(F("defaultPort"), String(Protocol[x].defaultPort ) );
    reply = reply + F("}");
  }

  reply = reply + F("]");
  WebServer.send(200, "application/json", reply );

} // handle_api_protocols

#endif // FEATURE_API
