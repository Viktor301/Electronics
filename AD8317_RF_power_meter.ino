#include <SPI.h>
#include <Wire.h>
#include <math.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);


boolean menu_is_active = false;
byte menu_items_counter = 0;

boolean menu(boolean change_menu_item) {
  if (!change_menu_item) {
    menu_items_counter++;
    if (menu_items_counter == 4) {
      menu_items_counter = 0;
    }
  }
  if (menu_items_counter == 0) {
    display.println("dBm");
  } else if (menu_items_counter == 1) {
    display.println("Band: 80 MHz");
  } else if (menu_items_counter == 2) {
    display.println("Att: 0 dB");
  } else if (menu_items_counter == 3) {
    display.println("Cal: 10 dBm");
  }
}

void setup()
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  analogReference(INTERNAL);
  Serial.begin(9600);
  pinMode(2, INPUT_PULLUP); // Підключення кнопки Btn1
  pinMode(3, INPUT_PULLUP); // Підключення кнопки Btn2
}

void loop()
{
  boolean start_stop_menu = digitalRead(2); // Натискання на кнопку Btn1 включає і виключає меню
  boolean change_menu_item = digitalRead(3); // Натискання на кнопку Btn2 послідовно переключає пункти меню

  float voltage;
  float voltage_sum = 0;

  for (int i = 0; i < 3000; i++) {
    voltage = analogRead(A0) * 1.1 / 1024 * 2 * 0.989;
    voltage_sum += voltage;
  }

  float dBm;
  dBm = (voltage_sum / 3000 - 0.48) / -0.022 - 0.575;

  String dBm_str;
  dBm_str += String(dBm);

  //float power;
  //power = 1000 / (pow(10,(-dBm / 10)));

  //String power_str;
  //power_str += String(power);

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.setTextSize(2);

  if (dBm > 0) {
    display.println("Overload");
  } else {
    display.println(dBm_str + "dBm");
    //display.println(power_str);
  }

  if (!start_stop_menu) {
    if (!menu_is_active) {
      menu_is_active = true;
    } else {
      menu_is_active = false;
    }
  }

  if (menu_is_active) {
    menu(change_menu_item);
  }

  display.display();

}
