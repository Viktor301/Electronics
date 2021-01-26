#include <SPI.h>
#include <Wire.h>
#include <math.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

String output;

// Меню

boolean menu_is_active = false;
byte menu_items_counter = 0;
const char *output_mode[] = {"dBm", "microWatt"};
const char *frequency[] = {"80 MHz", "400 MHz", "900 MHz", "1.5 GHz", "2.4 GHz", "3.6 GHz", "6 GHz", "8 GHz"};
const char *attenuation[] = {"10 dB", "20 dB", "30 dB"};

String menu(boolean change_menu_item) {
  if (!change_menu_item) {
    menu_items_counter++;
    if (menu_items_counter == 4) {
      menu_items_counter = 0;
    }
  }
  if (menu_items_counter == 0) {
    return ("dBm");
  } else if (menu_items_counter == 1) {
    return ("80 MHz");
  } else if (menu_items_counter == 2) {
    return ("Att.: 0 dB");
  } else if (menu_items_counter == 3) {
    return ("Calib.");
  }
}

// Вимір вхідного сигналу

String measurement() {

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

  float power;
  power = 1000 / (pow(10, (-dBm / 10)));

  String power_str;
  power_str += String(power);

  if (dBm > 0) {
    return ("Overload");
  } else {
    return (dBm_str + " dBm");
  }

}

void setup()
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  analogReference(INTERNAL);
  Serial.begin(9600);
  pinMode(2, INPUT_PULLUP); // Підключення кнопки Btn1
  pinMode(3, INPUT_PULLUP); // Підключення кнопки Btn2
  pinMode(4, INPUT_PULLUP); // Підключення кнопки Btn3
  pinMode(5, INPUT_PULLUP); // Підключення кнопки Btn4
  pinMode(6, INPUT_PULLUP); // Підключення кнопки Btn5

}

void loop()
{
  boolean start_stop_menu = digitalRead(2); // Натискання на кнопку Btn1 включає і виключає меню
  boolean change_menu_item = digitalRead(3); // Натискання на кнопку Btn2 послідовно переключає пункти меню
  boolean menu_item_value_up = digitalRead(4); // Натискання на кнопку Btn3 збільшує параментр в пункті меню
  boolean menu_item_value_down = digitalRead(5); // Натискання на кнопку Btn4 зменьшує параментр в пункті меню

  if (!start_stop_menu) {
    if (!menu_is_active) {
      menu_is_active = true;
    } else {
      menu_is_active = false;
    }
  }

  if (menu_is_active) {
    output = menu(change_menu_item);
  } else {
    output = measurement();
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.println(output);
  display.display();

}
