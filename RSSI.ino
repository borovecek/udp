void RssI () {
  display.drawLine(0, 12, 128, 12, SSD1306_WHITE);
  int bars;
  bars = WiFi.RSSI();
 // Serial.println(bars);
  display.drawBitmap(101, 0, pix , 12, 10, WHITE);
  /////////////////////////////////////////////////////////////////////////
  if (bars > -55) { // 5
    display.fillRect(125, 0, 2, 10, WHITE);
  }
  else {
    display.fillRect(125, 0, 2, 10, BLACK);
  }
  /////////////////////////////////////////////////////////////////////////
  if ( bars > -65) { // 4
    display.fillRect(122, 2, 2, 8, WHITE);
  }
  else {
    display.fillRect(122, 2, 2, 8, BLACK);
  }
  /////////////////////////////////////////////////////////////////////////
  if ( bars > -70) { // 3
    display.fillRect(119, 4, 2, 6, WHITE);
  }
  else {
    display.fillRect(119, 4, 2, 6, BLACK);
  }
  /////////////////////////////////////////////////////////////////////////
  if ( bars > -78) { // 2
    display.fillRect(116, 6, 2, 4, WHITE);
  }
  else {
    display.fillRect(116, 6, 2, 4, BLACK);
  }
  /////////////////////////////////////////////////////////////////////////
  if (bars < -78 & bars > -82) { // 1
    display.fillRect(113, 8, 2, 2, WHITE);
  }
  else {
    display.fillRect(113, 8, 2, 2, BLACK);
  }
  /////////////////////////////////////////////////////////////////////////

  display.display();
}
