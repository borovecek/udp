void Status() {
  sendUdpPacket({0x07, 0x00, 0x40, 0x00, 0x21, 0x80, 0xa1}); // LAN_status
  delay(20);
  checkUdpResponse();  // pravidelně kontroluj odpovědi
  /*  uint8_t status = buffer[6];  // status je na pozici 6

    switch (status) {
      case 0x01:
        // První případ EmergencyStop
        Serial.println("Status = 0x00: První možnost");
        break;
      case 0x02:
        // Druhý případ TrackVoltageOff
        Serial.println("Status = 0x01: Druhá možnost");
        break;
      case 0x04:
        // Třetí případ ShortCircuit
        Serial.println("Status = 0x02: Třetí možnost");
        break;
      case 0x20:
        // Čtvrtý případ  ProgrammingModeActive
        Serial.println("Status = 0x03: Čtvrtá možnost");
        break;
    }*/
}
void checkUdpResponse() {
  int packetSize = udp.parsePacket();
  if (packetSize > 0) {
    Serial.print("Received UDP packet, size: ");
    Serial.println(packetSize);
    uint8_t buffer[64];
    int len = udp.read(buffer, sizeof(buffer));
    Serial.print("Data: ");
    for (int i = 0; i < len; i++) {
      Serial.printf("%02X ", buffer[i]);
    }
    Serial.println();
  }
}
