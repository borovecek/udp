#include <WiFi.h>
#include <WiFiClient.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

int stopBut = 13;
int stopButHelp = 1;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ⚙️ WiFi připojení
const char* ssid = "Z21_ESP";
const char* password = "12345678";
/*const char* ssid = "HOKAMI_VISITORS";
  const char* password = "yespcb257";
  /*
  const char* ssid = "Z21_ESP";
  const char* password = "12345678";
*/

// ⚙️ Z21PG konfigurace
IPAddress z21_ip(192, 168, 0, 111);
const uint16_t z21_port = 21105;

// --- UDP komunikace ---
WiFiUDP udp;

// UART2 (Serial2) piny
#define RXD2 16
#define TXD2 17
WiFiClient z21Client;

const unsigned char pix [] PROGMEM = {
  0x00, 0x00, 0x0f, 0x00, 0x39, 0xc0, 0x40, 0x20, 0x9f, 0x90, 0x20, 0x40, 0x0f, 0x00, 0x10, 0x80,
  0x06, 0x00, 0x06, 0x00
};
unsigned long previousMillis1 = 0;
unsigned long interval1 = 3000;
unsigned long previousMillis2 = 0;
unsigned long interval2 = 5000;
unsigned long previousMillis3 = 0;
unsigned long interval3 = 500;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const unsigned char Power_EmSTOP[] PROGMEM = {
  //Power Em Stop Logo: 38x39px
  0x00, 0x7f, 0xff, 0xf8, 0x03, 0x00, 0xff, 0xff, 0xfc, 0x03, 0x01, 0xff, 0xff, 0xfe, 0x03, 0x01, 0xc0, 0x00, 0x0e, 0x03,
  0x03, 0xc0, 0x00, 0x0f, 0x03, 0x07, 0x80, 0x00, 0x07, 0x83, 0x07, 0x00, 0x00, 0x03, 0x83, 0x0f, 0x00, 0x00, 0x03, 0xc3,
  0x0e, 0x00, 0x00, 0x01, 0xe3, 0x1c, 0x00, 0x00, 0x00, 0xe3, 0x3c, 0x00, 0x00, 0x00, 0xf3, 0x38, 0x00, 0x00, 0x00, 0x73,
  0x78, 0xef, 0x9f, 0x1f, 0x7b, 0xf1, 0x02, 0x31, 0x91, 0x3f, 0xe1, 0x02, 0x20, 0x91, 0x1f, 0xe1, 0xc2, 0x20, 0x91, 0x1f,
  0xe0, 0x22, 0x20, 0x9f, 0x1f, 0xe0, 0x22, 0x20, 0x90, 0x1f, 0xf0, 0x22, 0x31, 0x90, 0x3f, 0x79, 0xc2, 0x1f, 0x10, 0x3b,
  0x38, 0x00, 0x00, 0x00, 0x73, 0x3c, 0x00, 0x00, 0x00, 0xf3, 0x3e, 0x00, 0x00, 0x01, 0xe3, 0x2e, 0x00, 0x00, 0x01, 0xe3,
  0xff, 0x00, 0x00, 0x03, 0xff, 0x27, 0x80, 0x00, 0x03, 0xc3, 0x27, 0x80, 0x00, 0x07, 0xc3, 0x23, 0xc0, 0x00, 0x0f, 0x43,
  0x21, 0xc0, 0x00, 0x1e, 0x43, 0x21, 0xff, 0xff, 0xfe, 0x43, 0x20, 0xff, 0xff, 0xfc, 0x43, 0x20, 0xff, 0xff, 0xf8, 0x43,
  0x20, 0x82, 0x04, 0x10, 0x43, 0x24, 0x82, 0x04, 0x12, 0x43, 0x20, 0x82, 0x04, 0x10, 0x43, 0x20, 0x82, 0x04, 0x10, 0x43,
  0xff, 0xff, 0xff, 0xff, 0xff, 0x20, 0x82, 0x04, 0x10, 0x43, 0x3f, 0x83, 0xfc, 0x1f, 0xc3
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  pinMode(stopBut, INPUT_PULLUP);
  Serial.begin(115200);    // Debug
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);  // UART2

  display.setRotation(2);
  display.display();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK); // Draw white text
  display.setCursor(0, 20);     // Start at top-left corner
  display.println("Connecting to WiFi...");
  display.display();

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  display.clearDisplay();
  display.setCursor(20, 20);
  display.println("WiFi connected");
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();
  udp.begin(21105);
  /////////////////////////////////////////////////////////////////////////
  sendLocoDrive(1, 0, 0, 3);
  delay(20);
  sendLocoFunction(1, 0, 0);
  delay(20);
  sendLocoDrive(2, 0, 0, 3);
  delay(20);
  sendLocoFunction(2, 0, 0);
  delay(20);
  sendLocoDrive(3, 0, 0, 3);
  delay(20);
  sendLocoFunction(3, 0, 0);
  delay(20);
  sendLocoDrive(4, 0, 0, 3);
  delay(20);
  sendLocoFunction(4, 0, 0);
  delay(20);
  sendLocoDrive(5, 0, 0, 3);
  delay(20);
  sendLocoFunction(5, 0, 0);
  delay(20);
  sendLocoDrive(6, 0, 0, 3);
  delay(20);
  sendLocoFunction(6, 0, 0);
  delay(20);
  /////////////////////////////////////////////////////////////////////////
}

void loop() {
  /////////////////////////////////////////////////////////////////////////
  if ((digitalRead(stopBut) == HIGH) && (stopButHelp == 1)) {
    sendUdpPacket({0x07, 0x00, 0x40, 0x00, 0x21, 0x80, 0xa1}); // LAN_stop
    display.fillRect(0, 18, 128, 64, SSD1306_BLACK); //remove the config data
    display.drawBitmap(42, 23, Power_EmSTOP, 38, 39, SSD1306_WHITE); //EmSTOP Logo
    delay(100);
    sendUdpPacket({0x07, 0x00, 0x40, 0x00, 0x21, 0x80, 0xa1}); // LAN_stop
    delay(20);
    stopButHelp = 0;
  }
  if ((digitalRead(stopBut) == LOW) && (stopButHelp == 0)) {
    sendUdpPacket({0x07, 0x00, 0x40, 0x00, 0x21, 0x81, 0xa0}); // LAN_Nstop
      display.fillRect(0, 18, 128, 64, SSD1306_BLACK); //remove the config data
  //  display.drawBitmap(42, 23, Power_EmSTOP, 38, 39, SSD1306_BLACK); //EmSTOP Logo
    delay(100);
    sendUdpPacket({0x07, 0x00, 0x40, 0x00, 0x21, 0x81, 0xa0}); // LAN_Nstop
    delay(20);
    stopButHelp = 1;
  }
  /////////////////////////////////////////////////////////////////////////
  if (Serial2.available()) {
    String line = Serial2.readStringUntil('\n');
    Serial.print("Received: ");
    Serial.println(line);

    int loco = 0, speed = 0, dir = 0, f0 = 0;
    if (parseCommand(line, loco, speed, dir, f0)) {
      for (int i = 0; i < 3; i++) {
        sendLocoDrive(getDccAddress(loco), speed, dir, 3);
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE, SSD1306_BLACK); // Draw white text
        display.setCursor(60, 30);     // Start at top-left corner
        display.println(loco);
        display.setCursor(10, 50);     // Start at top-left corner
        display.printf("%3d", speed);
        display.setCursor(65, 50);     // Start at top-left corner
        display.println(dir);
        display.display();
        delay(20); // malá pauza mezi pakety (např. 20 ms)
      }
      for (int i = 0; i < 3; i++) {
        sendLocoFunction(getDccAddress(loco), 0, f0);
        display.setCursor(110, 50);     // Start at top-left corner
        display.println(f0);
        display.display();
        delay(20);
      }
    }
  }
  /////////////////////////////////////////////////////////////////////////
  unsigned long currentMillis1 = millis();
  if (currentMillis1 - previousMillis1 >= interval1) {
    // save the last time you blinked the LED
    previousMillis1 = currentMillis1;
    //   Serial.println(WiFi.RSSI());
    display.display();
    RssI();
  }
  /////////////////////////////////////////////////////////////////////////
  /*  unsigned long currentMillis3 = millis();
    if (currentMillis3 - previousMillis3 >= interval3) {
      // save the last time you blinked the LED
      previousMillis3 = currentMillis3;
      Status();
    }*/
  /////////////////////////////////////////////////////////////////////////
  unsigned long currentMillis2 = millis();
  // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis2 - previousMillis2 >= interval2)) {
    Serial.print(millis());
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis2 = currentMillis2;
  }
  /////////////////////////////////////////////////////////////////////////
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
