#include <SPI.h>
#include <Wire.h>
#include <math.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

String output;
boolean menu_is_active = false;
byte menu_item_counter = 0;
int menu_item_value = 0;

// Параментри пунктів меню в вигляді масивів
const char *output_unit[] = {"dBm", "microWatt"};
const char *output_mode[] = {"Average", "Min", "Max"};
const char *frequency[] = {"80 MHz", "400 MHz", "900 MHz", "1.5 GHz", "2.4 GHz", "3.6 GHz", "6 GHz", "8 GHz"};
const char *attenuation[] = {"Att: 0 dB", "Att: 10 dB", "Att: 20 dB", "Att: 30 dB"};

// Меню

String menu(boolean menu_item_change, int menu_item_value) {
  if (!menu_item_change) {
    menu_item_counter++;
    if (menu_item_counter == 5) {
      menu_item_counter = 0;
    }
  }

  // Обмеження максимального значення параметра в пункті меню
  if (menu_item_counter == 0 and menu_item_value > 1) menu_item_value = 1;
  else if (menu_item_counter == 1 and menu_item_value > 2) menu_item_value = 2;
  else if (menu_item_counter == 2 and menu_item_value > 7) menu_item_value = 7;
  else if (menu_item_counter == 3 and menu_item_value > 3) menu_item_value = 3;
 
  if (menu_item_counter == 0) return output_unit[menu_item_value];
  else if (menu_item_counter == 1) return output_mode[menu_item_value];
  else if (menu_item_counter == 2) return frequency[menu_item_value];
  else if (menu_item_counter == 3) return attenuation[menu_item_value];
  else if (menu_item_counter == 4) return "Calib.";
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
  boolean menu_on_off = digitalRead(2); // Натискання на кнопку Btn1 включає і виключає меню
  boolean menu_item_change = digitalRead(3); // Натискання на кнопку Btn2 послідовно переключає пункти меню
  boolean menu_item_value_up = digitalRead(4); // Натискання на кнопку Btn3 збільшує параметр в пункті меню
  boolean menu_item_value_down = digitalRead(5); // Натискання на кнопку Btn4 зменьшує параметр в пункті меню

  // Фіксація стану меню (включено, виключено)
  if (!menu_on_off) {
    if (!menu_is_active) {
      menu_is_active = true;
    } else {
      menu_is_active = false;
    }
  }

  // Скидання параметрів пунктів меню на 0 при перемиканні пунктів меню
  if (!menu_item_change) menu_item_value = 0;

  // Фіксація значення параметра в пункті меню
  if (!menu_item_value_up) menu_item_value++;
  if (!menu_item_value_down) {
    menu_item_value--;
    if (menu_item_value < 0) menu_item_value = 0;
  }

  //Виведення на екран меню або результатів виміру
  if (menu_is_active) {
    output = menu(menu_item_change, menu_item_value);
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
