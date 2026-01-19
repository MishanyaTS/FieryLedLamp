void SSDP_init(void) {
  //String chipID = String( ESP.getChipId() ) + "-" + String( ESP.getFlashChipId() );
  // SSDP дескриптор
  HTTP.on(F("/description.xml"), HTTP_GET, []() {
    SSDP.schema(HTTP.client());
  });

  LAMP_NAME = jsonRead(configSetup, "SSDP");
  SSDP.setName(jsonRead(configSetup, "SSDP"));
  SSDP.setDeviceType(F("upnp:rootdevice"));
  SSDP.setSchemaURL(F("description.xml"));
  SSDP.setHTTPPort(80);
  SSDP.setName(jsonRead(configSetup, "SSDP"));
  #ifdef ESP32_USED
  SSDP.setSerialNumber(get_Chip_ID());
  #else
  SSDP.setSerialNumber(ESP.getChipId());
  #endif
  SSDP.setURL("/");
  SSDP.setModelName(F("FieryLedLamp"));
  SSDP.setModelNumber(jsonRead(configSetup, "SSDP") + VERSION);  
  SSDP.setModelURL(F("https://github.com/MishanyaTS/FieryLedLamp"));
  SSDP.setManufacturer(F("MishanyaTS"));
  SSDP.setManufacturerURL(F("https://github.com/MishanyaTS/FieryLedLamp"));
  SSDP.begin();
}
