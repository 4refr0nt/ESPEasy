//   _    _ ___________
//  | |  | |  ___| ___ \
//  | |  | | |__ | |_/ /   ___ ___  _ __ ___  _ __ ___   ___  _ __
//  | |/\| |  __|| ___ \  / __/ _ \| '_ ` _ \| '_ ` _ \ / _ \| '_ \
//  \  /\  / |___| |_/ / | (_| (_) | | | | | | | | | | | (_) | | | |
//   \/  \/\____/\____/   \___\___/|_| |_| |_|_| |_| |_|\___/|_| |_|
//

#if FEATURE_HTML && FEATURE_API

//********************************************************************************
// Save data - config
//********************************************************************************
void save_config() {

  char tmpString[64];

  String name = WebServer.arg("name");
  String password = WebServer.arg("password");
  String ssid = WebServer.arg("ssid");
  String key = WebServer.arg("key");
  String usedns = WebServer.arg("usedns");
  String controllerip = WebServer.arg("controllerip");
  String controllerhostname = WebServer.arg("controllerhostname");
  String controllerport = WebServer.arg("controllerport");
  String protocol = WebServer.arg("protocol");
  String controlleruser = WebServer.arg("controlleruser");
  String controllerpassword = WebServer.arg("controllerpassword");
  String sensordelay = WebServer.arg("delay");
  String deepsleep = WebServer.arg("deepsleep");
  String espip = WebServer.arg("espip");
  String espgateway = WebServer.arg("espgateway");
  String espsubnet = WebServer.arg("espsubnet");
  String espdns = WebServer.arg("espdns");
  String unit = WebServer.arg("unit");
  String apkey = WebServer.arg("apkey");

  if (ssid[0] != 0)
  {
    strncpy(Settings.Name, name.c_str(), sizeof(Settings.Name));
    strncpy(SecuritySettings.Password, password.c_str(), sizeof(SecuritySettings.Password));
    strncpy(SecuritySettings.WifiSSID, ssid.c_str(), sizeof(SecuritySettings.WifiSSID));
    strncpy(SecuritySettings.WifiKey, key.c_str(), sizeof(SecuritySettings.WifiKey));
    strncpy(SecuritySettings.WifiAPKey, apkey.c_str(), sizeof(SecuritySettings.WifiAPKey));

    if (Settings.Protocol != protocol.toInt())
    {
      Settings.Protocol = protocol.toInt();
      byte ProtocolIndex = getProtocolIndex(Settings.Protocol);
      Settings.ControllerPort = Protocol[ProtocolIndex].defaultPort;
      if (Protocol[ProtocolIndex].usesMQTT)
        CPlugin_ptr[ProtocolIndex](CPLUGIN_PROTOCOL_TEMPLATE, 0, dummyString);
    }
    else
    {
      if (Settings.Protocol != 0)
      {
        Settings.UseDNS = usedns.toInt();
        if (Settings.UseDNS)
        {
          strncpy(Settings.ControllerHostName, controllerhostname.c_str(), sizeof(Settings.ControllerHostName));
          getIPfromHostName();
        }
        else
        {
          if (controllerip.length() != 0)
          {
            controllerip.toCharArray(tmpString, 26);
            str2ip(tmpString, Settings.Controller_IP);
          }
        }

        Settings.ControllerPort = controllerport.toInt();
        strncpy(SecuritySettings.ControllerUser, controlleruser.c_str(), sizeof(SecuritySettings.ControllerUser));
        strncpy(SecuritySettings.ControllerPassword, controllerpassword.c_str(), sizeof(SecuritySettings.ControllerPassword));
      }
    }

    Settings.Delay = sensordelay.toInt();
    Settings.deepSleep = (deepsleep == "on");
    espip.toCharArray(tmpString, 26);
    str2ip(tmpString, Settings.IP);
    espgateway.toCharArray(tmpString, 26);
    str2ip(tmpString, Settings.Gateway);
    espsubnet.toCharArray(tmpString, 26);
    str2ip(tmpString, Settings.Subnet);
    espdns.toCharArray(tmpString, 26);
    str2ip(tmpString, Settings.DNS);
    Settings.Unit = unit.toInt();

    SaveSettings();
  }

} // save_config


//********************************************************************************
// Save data - hardware
//********************************************************************************
void save_hardware() {
  String pin_i2c_sda = WebServer.arg("psda");
  String pin_i2c_scl = WebServer.arg("pscl");
  String pin_status_led = WebServer.arg("pled");

  if (pin_i2c_sda.length() != 0)
  {
    Settings.Pin_i2c_sda     = pin_i2c_sda.toInt();
    Settings.Pin_i2c_scl     = pin_i2c_scl.toInt();
    Settings.Pin_status_led  = pin_status_led.toInt();
    Settings.PinStates[0]  =  WebServer.arg("p0").toInt();
    Settings.PinStates[2]  =  WebServer.arg("p2").toInt();
    Settings.PinStates[4]  =  WebServer.arg("p4").toInt();
    Settings.PinStates[5]  =  WebServer.arg("p5").toInt();
    Settings.PinStates[9]  =  WebServer.arg("p9").toInt();
    Settings.PinStates[10] =  WebServer.arg("p10").toInt();
    Settings.PinStates[12] =  WebServer.arg("p12").toInt();
    Settings.PinStates[13] =  WebServer.arg("p13").toInt();
    Settings.PinStates[14] =  WebServer.arg("p14").toInt();
    Settings.PinStates[15] =  WebServer.arg("p15").toInt();
    Settings.PinStates[16] =  WebServer.arg("p16").toInt();

    SaveSettings();
  }

} // save_hardware


//********************************************************************************
// Save data - advanced
//********************************************************************************
void save_advanced() {

  char tmpString[81];

  String mqttsubscribe = WebServer.arg("mqttsubscribe");
  String mqttpublish = WebServer.arg("mqttpublish");
  String messagedelay = WebServer.arg("messagedelay");
  String ip = WebServer.arg("ip");
  String syslogip = WebServer.arg("syslogip");
  String ntphost = WebServer.arg("ntphost");
  String timezone = WebServer.arg("timezone");
  String dst = WebServer.arg("dst");
  String sysloglevel = WebServer.arg("sysloglevel");
  String udpport = WebServer.arg("udpport");
  String useserial = WebServer.arg("useserial");
  String serialloglevel = WebServer.arg("serialloglevel");
  String webloglevel = WebServer.arg("webloglevel");
  String baudrate = WebServer.arg("baudrate");
#if !FEATURE_SPIFFS
  String customcss = WebServer.arg("customcss");
#endif
  String usentp = WebServer.arg("usentp");
  String wdi2caddress = WebServer.arg("wdi2caddress");
  String usessdp = WebServer.arg("usessdp");
  String edit = WebServer.arg("edit");

  if (edit.length() != 0)
  {
    mqttsubscribe.toCharArray(tmpString, 81);
    strcpy(Settings.MQTTsubscribe, tmpString);
    mqttpublish.toCharArray(tmpString, 81);
    strcpy(Settings.MQTTpublish, tmpString);
    Settings.MessageDelay = messagedelay.toInt();
    Settings.IP_Octet = ip.toInt();
    ntphost.toCharArray(tmpString, 64);
    strcpy(Settings.NTPHost, tmpString);
    Settings.TimeZone = timezone.toInt();
    syslogip.toCharArray(tmpString, 26);
    str2ip(tmpString, Settings.Syslog_IP);
    Settings.UDPPort = udpport.toInt();
    Settings.SyslogLevel = sysloglevel.toInt();
    Settings.UseSerial = (useserial == "on");
    Settings.SerialLogLevel = serialloglevel.toInt();
    Settings.WebLogLevel = webloglevel.toInt();
    Settings.BaudRate = baudrate.toInt();
#if !FEATURE_SPIFFS
    Settings.CustomCSS = (customcss == "on");
#endif
    Settings.UseNTP = (usentp == "on");
    Settings.DST = (dst == "on");
    Settings.WDI2CAddress = wdi2caddress.toInt();
    Settings.UseSSDP = (usessdp == "on");

    SaveSettings();
  }
} // save_advanced


//********************************************************************************
// Save data - device
//********************************************************************************
void device_save() {

  char tmpString[41];
  struct EventStruct TempEvent;

  String taskindex = WebServer.arg("index");
  String taskdevicenumber = WebServer.arg("taskdevicenumber");
  String taskdevicetimer = WebServer.arg("taskdevicetimer");
  String taskdeviceid = WebServer.arg("taskdeviceid");
  String taskdevicepin1 = WebServer.arg("taskdevicepin1");
  String taskdevicepin2 = WebServer.arg("taskdevicepin2");
  String taskdevicepin3 = WebServer.arg("taskdevicepin3");
  String taskdevicepin1pullup = WebServer.arg("taskdevicepin1pullup");
  String taskdevicepin1inversed = WebServer.arg("taskdevicepin1inversed");
  String taskdevicename = WebServer.arg("taskdevicename");
  String taskdeviceport = WebServer.arg("taskdeviceport");
  String taskdeviceformula[VARS_PER_TASK];
  String taskdevicevaluename[VARS_PER_TASK];
  String taskdevicesenddata = WebServer.arg("taskdevicesenddata");
  String taskdeviceglobalsync = WebServer.arg("taskdeviceglobalsync");

  for (byte varNr = 0; varNr < VARS_PER_TASK; varNr++)
  {
    char argc[25];
    String arg = "taskdeviceformula";
    arg += varNr + 1;
    arg.toCharArray(argc, 25);
    taskdeviceformula[varNr] = WebServer.arg(argc);

    arg = "taskdevicevaluename";
    arg += varNr + 1;
    arg.toCharArray(argc, 25);
    taskdevicevaluename[varNr] = WebServer.arg(argc);
  }

  String edit = WebServer.arg("edit");
  byte index = taskindex.toInt();

  byte DeviceIndex = 0;

  if (edit.toInt() != 0)
  {
    if (Settings.TaskDeviceNumber[index - 1] != taskdevicenumber.toInt()) // change of device, clear all other values
    {
      taskClear(index - 1, false); // clear settings, but do not save
      Settings.TaskDeviceNumber[index - 1] = taskdevicenumber.toInt();
    }
    else if (taskdevicenumber.toInt() != 0)
    {
      Settings.TaskDeviceNumber[index - 1] = taskdevicenumber.toInt();
      DeviceIndex = getDeviceIndex(Settings.TaskDeviceNumber[index - 1]);

      if (taskdevicetimer.toInt() > 0)
        Settings.TaskDeviceTimer[index - 1] = taskdevicetimer.toInt();
      else
        Settings.TaskDeviceTimer[index - 1] = Settings.Delay;

      taskdevicename.toCharArray(tmpString, 26);
      strcpy(ExtraTaskSettings.TaskDeviceName, tmpString);
      Settings.TaskDevicePort[index - 1] = taskdeviceport.toInt();
      if (Settings.TaskDeviceNumber[index - 1] != 0)
        Settings.TaskDeviceID[index - 1] = taskdeviceid.toInt();
      else
        Settings.TaskDeviceID[index - 1] = 0;
      if (Device[DeviceIndex].Type == DEVICE_TYPE_SINGLE)
      {
        Settings.TaskDevicePin1[index - 1] = taskdevicepin1.toInt();
      }
      if (Device[DeviceIndex].Type == DEVICE_TYPE_DUAL)
      {
        Settings.TaskDevicePin1[index - 1] = taskdevicepin1.toInt();
        Settings.TaskDevicePin2[index - 1] = taskdevicepin2.toInt();
      }

      if (taskdevicepin3.length() != 0)
        Settings.TaskDevicePin3[index - 1] = taskdevicepin3.toInt();

      if (Device[DeviceIndex].PullUpOption)
        Settings.TaskDevicePin1PullUp[index - 1] = (taskdevicepin1pullup == "on");

      if (Device[DeviceIndex].InverseLogicOption)
        Settings.TaskDevicePin1Inversed[index - 1] = (taskdevicepin1inversed == "on");

      if (Device[DeviceIndex].SendDataOption)
        Settings.TaskDeviceSendData[index - 1] = (taskdevicesenddata == "on");

      if (Device[DeviceIndex].GlobalSyncOption)
        Settings.TaskDeviceGlobalSync[index - 1] = (taskdeviceglobalsync == "on");

      // Send task info if set global
      if (Settings.TaskDeviceGlobalSync[index - 1])
      {
        SendUDPTaskInfo(0, index - 1, index - 1);
      }

      for (byte varNr = 0; varNr < Device[DeviceIndex].ValueCount; varNr++)
      {
        taskdeviceformula[varNr].toCharArray(tmpString, 41);
        strcpy(ExtraTaskSettings.TaskDeviceFormula[varNr], tmpString);
      }

      // task value names handling.
      TempEvent.TaskIndex = index - 1;
      for (byte varNr = 0; varNr < Device[DeviceIndex].ValueCount; varNr++)
      {
        taskdevicevaluename[varNr].toCharArray(tmpString, 26);
        strcpy(ExtraTaskSettings.TaskDeviceValueNames[varNr], tmpString);
      }
      TempEvent.TaskIndex = index - 1;
      PluginCall(PLUGIN_WEBFORM_SAVE, &TempEvent, dummyString);
      PluginCall(PLUGIN_INIT, &TempEvent, dummyString);
    }
    SaveTaskSettings(index - 1);

    SaveSettings();
  }
} // device_save


#endif // FEATURE_HTML && FEATURE_API
//********************************************************************************
