// Konstanty pro počet lokomotiv
const uint8_t locoCount = 6;

// Pinové mapování
const uint8_t speedPins[locoCount] = {A0, A1, A2, A3, A4, A5};
const uint8_t dirPins[locoCount]   = {2, 3, 4, 5, 6, 7};
const uint8_t lightPins[locoCount] = {8, 9, 10, 11, 12, 13};

// Uložení předchozích hodnot
uint8_t prevSpeed[locoCount] = {0};
uint8_t prevDir[locoCount]   = {0};
uint8_t prevLight[locoCount] = {0};

// Minimální změna rychlosti, při které pošleme novou hodnotu
const uint8_t speedThreshold = 3;

void setup() {
  Serial.begin(115200);

  for (uint8_t i = 0; i < locoCount; i++) {
    pinMode(dirPins[i], INPUT_PULLUP);
    pinMode(lightPins[i], INPUT_PULLUP);
  }
}

void loop() {
  for (uint8_t i = 0; i < locoCount; i++) {
    // Čtení potenciometru a mapování do 0-127
    int rawSpeed = analogRead(speedPins[i]);
    uint8_t speed = map(rawSpeed, 0, 1023, 0, 127);

    // Čtení směru a světel (invertováno, protože pullup)
    uint8_t dir = digitalRead(dirPins[i]) == LOW ? 1 : 0;
    uint8_t light = digitalRead(lightPins[i]) == LOW ? 1 : 0;

    // Detekce větší změny rychlosti
    bool speedChanged = abs(speed - prevSpeed[i]) >= speedThreshold;

    // Pokud se něco změnilo, odešli data
    if (speedChanged || dir != prevDir[i] || light != prevLight[i]) {
      Serial.print(i + 1); // Lokomotiva 1..6
      Serial.print(":");
      Serial.print(speed);
      Serial.print(":");
      Serial.print(dir);
      Serial.print(":");
      Serial.println(light);

      // Uložení nových hodnot
      prevSpeed[i] = speed;
      prevDir[i] = dir;
      prevLight[i] = light;
    }
  }

  delay(100);  // Menší zpoždění kvůli stabilitě čtení
}
