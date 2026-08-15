void SSDP_init(void) {
  // SSDP дескриптор
  static bool schemaRouteRegistered = false;
  if (!schemaRouteRegistered) {
    HTTP.on(F("/description.xml"), HTTP_GET, []() {
      SSDP.schema(HTTP.client());
    });
    schemaRouteRegistered = true;
  }

  LAMP_NAME = jsonRead(configSetup, "SSDP");
  SSDP.setName(jsonRead(configSetup, "SSDP"));
  SSDP.setDeviceType(F("upnp:rootdevice"));
  SSDP.setSchemaURL(F("description.xml"));
  SSDP.setHTTPPort(80);
  SSDP.setName(jsonRead(configSetup, "SSDP"));
  SSDP.setSerialNumber(get_Chip_ID());
  SSDP.setURL("/");
  SSDP.setModelName(F("FieryLedLamp"));
  SSDP.setModelNumber(jsonRead(configSetup, "SSDP") + VERSION);  
  SSDP.setModelURL(F("https://github.com/MishanyaTS/FieryLedLamp"));
  SSDP.setManufacturer(F("MishanyaTS"));
  SSDP.setManufacturerURL(F("https://github.com/MishanyaTS/FieryLedLamp"));
  SSDP.begin();
}
