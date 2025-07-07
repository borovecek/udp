#include <WiFi.h>
#include <WiFiClient.h>

// ⚙️ WiFi připojení
const char* ssid = "Z21_ESP";
const char* password = "12345678";

// ⚙️ Z21PG konfigurace
IPAddress z21_ip(192, 168, 0, 111);
const uint16_t z21_port = 21105;

// --- UDP komunikace ---
WiFiUDP udp;

// UART2 (Serial2) piny
#define RXD2 16
#define TXD2 17
WiFiClient z21Client;

void setup() {
  Serial.begin(115200);    // Debug
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);  // UART2

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
    udp.begin(21105);
}

void loop() {
  if (Serial2.available()) {
    String line = Serial2.readStringUntil('\n');
    Serial.print("Received: ");
    Serial.println(line);

    int loco = 0, speed = 0, dir = 0, f0 = 0;
    if (parseCommand(line, loco, speed, dir, f0)) {
      sendLocoDrive(getDccAddress(loco), speed,dir, 3);
      sendLocoFunction(getDccAddress(loco), 0, f0); // F0 ON
    }
  }
}

uint16_t getDccAddress(int loco) {
  switch (loco) {
    case 1: return 3;
    case 2: return 7;
    case 3: return 11;
    case 4: return 15;
    case 5: return 19;
    case 6: return 23;
    default: return 3;
  }
}

bool parseCommand(String input, int &loco, int &speed, int &dir, int &f0) {
  int first = input.indexOf(':');
  int second = input.indexOf(':', first + 1);
  int third = input.indexOf(':', second + 1);

  if (first < 0 || second < 0 || third < 0) return false;

  loco  = input.substring(0, first).toInt();
  speed = input.substring(first + 1, second).toInt();
  dir   = input.substring(second + 1, third).toInt();
  f0    = input.substring(third + 1).toInt();

  return true;
}

void sendLocoDrive(uint16_t address, uint8_t speed, int forward, uint8_t speedSteps) {
  uint8_t adr_msb = (address >> 8) & 0x3F;
  if (address >= 128) {
    adr_msb |= 0xC0; // Horní 2 bity nastavit na 1 pro adresy >=128
  }
  uint8_t adr_lsb = address & 0xFF;

  // DB0: 0x10 + počet speed steps (S)
  uint8_t db0 = 0x10 | (speedSteps & 0x0F);

  // DB3: bit7 = směr (1=vpřed), bity0-6 = rychlost
  uint8_t db3 = speed & 0x7F;
  if (forward) db3 |= 0x80;

  uint8_t packet[10];
  packet[0] = 0x0A;    // délka paketu
  packet[1] = 0x00;    // header1
  packet[2] = 0x40;    // header2
  packet[3] = 0x00;    // header3
  packet[4] = 0xE4;    // typ zprávy - LAN_X_SET_LOCO_DRIVE
  packet[5] = db0;     // počet rychlostních kroků
  packet[6] = adr_msb; // adresa MSB
  packet[7] = adr_lsb; // adresa LSB
  packet[8] = db3;     // rychlost + směr

  // Výpočet XOR pro bajty 1..8
  uint8_t xorByte = 0;
  for (int i = 1; i <= 8; i++) {
    xorByte ^= packet[i];
  }
  packet[9] = xorByte;

  sendUdpPacket(packet, sizeof(packet));
  Serial.print("Sent Drive Packet to address ");
  Serial.println(address);
}
void sendLocoFunction(uint16_t address, uint8_t functionIndex, uint8_t action) {
  uint8_t adr_msb = (address >> 8) & 0x3F;
  if (address >= 128) {
    adr_msb |= 0xC0;
  }
  uint8_t adr_lsb = address & 0xFF;

  // DB0: TTNNNNNN
  uint8_t db0 = ((action & 0x03) << 6) | (functionIndex & 0x3F);

  uint8_t packet[10];
  packet[0] = 0x0A; // délka dat
  packet[1] = 0x00;
  packet[2] = 0x40;
  packet[3] = 0x00;
  packet[4] = 0xE4; // LAN_X_SET_LOCO_FUNCTION
  packet[5] = 0xF8;
  packet[6] = adr_msb;
  packet[7] = adr_lsb;
  packet[8] = db0; // nevyužito pro F0-F12

  uint8_t xorByte = 0;
  for (int i = 1; i <= 8; i++) {
    xorByte ^= packet[i];
  }
  packet[9] = xorByte;

  sendUdpPacket(packet, sizeof(packet));

  Serial.print("Function command sent: Addr=");
  Serial.print(address);
  Serial.print(", Func=");
  Serial.print(functionIndex);
  Serial.print(", Action=");
  Serial.println(action);
}
void sendUdpPacket(std::initializer_list<uint8_t> data) {
  udp.beginPacket(z21_ip, z21_port);
  for (auto b : data) udp.write(b);
  udp.endPacket();
  Serial.println("UDP packet sent");
}

void sendUdpPacket(const uint8_t* data, size_t len) {
  udp.beginPacket(z21_ip, z21_port);
  udp.write(data, len);
  udp.endPacket();
  Serial.println("UDP packet sent");
}
