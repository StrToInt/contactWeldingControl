#include "TM1637.h" //либа для дисплея на этой микре

#define PIN_BTN 10 //пин кнопки
#define PIN_SWITCH 11 //пин оптореле

#define LCD_CLK 13 //дисплей такт
#define LCD_DATA 12 // дисплей данные


//********************************************************
// Навигация по меню: удержание кнопки более 1.5с
// Три режима: увеличение времени импульса/уменьшение/огонь_нах!
// При удерджании больше 200 изменение на +-100мс
// все действия с кнопкой происходят при ее отпускании
//********************************************************




byte btn=0;
TM1637 tm1637(LCD_CLK, LCD_DATA);
int time = 10;

#define MODE_TIME_INC 0
#define MODE_TIME_DEC 1
#define MODE_ACTION 2
byte mode = MODE_TIME_INC;

long pressedTime = 0;

void setup() {
  tm1637.init();
  tm1637.set(BRIGHT_TYPICAL);
  digitalWrite(PIN_BTN,HIGH);
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(PIN_SWITCH,OUTPUT);
}

byte flag = 0;

void loop() {
  flag = 0;
  delay(5);
  
  if (digitalRead(PIN_BTN) == LOW) {
    if (btn == 0) {
      btn = 1;
      pressedTime = millis();
      //down  
    } else {
      //pressed
      if (millis() - pressedTime > 1500 && millis() - pressedTime < 2000) {
        
        int8_t msg1[] = {100,0xD,0xE,0x0C};
        int8_t msg2[] = {0xF,175,19,0x0e};
        int8_t msg3[] = {100,175,21,0x0C};
        tm1637.point(false);
        if (mode == MODE_TIME_INC)
          tm1637.display(msg1);
        else if (mode == MODE_TIME_DEC)
          tm1637.display(msg2);
        else if (mode == MODE_ACTION)
          tm1637.display(msg3);
        flag = 1;
      }
    }
  } else {
    if (btn == 1) {
      btn = 0;
      //up
      if (millis() - pressedTime > 1500) {
        mode++;
        if (mode > 2)
          mode = 0;
      } else {
        if (mode == MODE_TIME_INC) {
          if (millis() - pressedTime > 200)
            time+=100;
          else
            time++;
        } else if (mode == MODE_TIME_DEC) {
          if (millis() - pressedTime > 200) {
            if (time > 10+100)
              time-=100;
          } else {
            if (time > 10)
              time--;
          }
        } else if (mode == MODE_ACTION) {
          digitalWrite(LED_BUILTIN,HIGH);
          digitalWrite(PIN_SWITCH,HIGH);
          delay(time);
          digitalWrite(LED_BUILTIN,LOW);
          digitalWrite(PIN_SWITCH,LOW);
        }
      }
    }
  }
  if (flag == 0) {
    tm1637.display(time);
  }
}
