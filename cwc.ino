#include "TM1637.h" //либа для дисплея на этой микре

#define PIN_BTN1 10 //пин кнопки
#define PIN_BTN2 14 //пин 2-й кнопки
#define PIN_SWITCH 11 //пин оптореле

#define LCD_CLK 13 //дисплей такт
#define LCD_DATA 12 // дисплей данные


//********************************************************
// Навигация по меню: удержание кнопки более 1.5с
// Три режима: увеличение времени импульса/уменьшение/огонь_нах!
// При удерджании больше 200 изменение на +-100мс
// все действия с кнопкой происходят при ее отпускании
//********************************************************




byte btn1 = 0, btn2 = 0;
long btnTime1, btnTime2 = 0;
TM1637 tm1637(LCD_CLK, LCD_DATA);
byte modeChange = 0;

#define TIME_MAX 5000
#define TIME_MIN 30
int time = TIME_MIN;
byte timeIncDec = 0;

#define MODE_BEGIN 0

#define MODE_TIME_CHANGE 0
#define MODE_FIRE_ONCE 1
#define MODE_FIRE_INTERVAL 2
#define MODE_FIRE 3

#define MODE_LAST 3

byte mode = MODE_BEGIN;

void setup() {
  Serial.begin(9600);
  tm1637.init();
  tm1637.set(BRIGHT_TYPICAL);
  tm1637.point(false);
  
  digitalWrite(PIN_BTN1,HIGH);
  digitalWrite(PIN_BTN2,HIGH);
  
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(PIN_SWITCH,OUTPUT);
}

byte flag = 0;

void loop() {
  delay(10);

  //button 0
  if (digitalRead(PIN_BTN1) == LOW) {
    if (btn1 == 0) {
      btn1 = 1;
      btnTime1 = millis();
      //down  
      btnDown(0);
    } else { 
      //press
      btnPress(0);
    }
  } else {
    if (btn1 == 1) {
      btn1 = 0;
      //up
      btnUp(0);
    }
  }

  //button1
  
  if (digitalRead(PIN_BTN2) == LOW) {
    if (btn2 == 0) {
      btn2 = 1;
      btnTime2 = millis();
      //down  
      btnDown(1);
    } else { 
      //press
      btnPress(1);
    }
  } else {
    if (btn2 == 1) {
      btn2 = 0;
      //up
      btnUp(1);
    }
  }

  if (modeChange == 0) {
    if (mode == MODE_TIME_CHANGE) {
      if (btn1 == 0) {
        //int8_t msg[] = {time,175,21,0x0C};
        tm1637.display(time);
        Serial.println(time);
      }
    } else {
      showMode();
    }
  }
  Serial.println(time);
}

//button actions

void btnDown(byte btn) {
  if (btn == 1) {
    if (mode == MODE_TIME_CHANGE) {
      if (timeIncDec == 0) {
        //inc
        if (time < TIME_MAX) time ++;
      } else {
        //dec
        if (time > TIME_MIN) time --;
      }
    }
  }
  if (btn == 0) {
    if (mode == MODE_TIME_CHANGE) {
      timeIncDec = !timeIncDec;
      if (timeIncDec == 0) {
        int8_t msg[] = {100,175,21,0x0C};
        tm1637.display(msg);
      } else {
        int8_t msg[] = {100,0xD,0xE,0x0C};
        tm1637.display(msg);
      }
    }
  }
}

void btnPress(byte btn) {
  if (btn == 1) {
    if (mode == MODE_TIME_CHANGE) {
      if (millis() - btnTime2 > 250) {
        btnDown(1);   
      }
    }
  }
  if (btn == 0 && millis() - btnTime1 > 300) {
    if (modeChange == 0) {
      modeChange = 1;
      mode++;
      if (mode > MODE_LAST)
        mode = MODE_BEGIN;
      showMode();
    }
  }
}

void btnUp(byte btn) {
  if (btn == 0) {
    modeChange = 0;
  }
}

void showMode() {
  if (mode == MODE_FIRE_ONCE) {
    int8_t msg[] = {0,21,0xC,0x0E};
    tm1637.display(msg);
  } else if (mode == MODE_FIRE_INTERVAL) {
    int8_t msg[] = {100,175,21,22};
    tm1637.display(msg);
  } else if (mode == MODE_FIRE) {
    int8_t msg[] = {0xF,175,19,0x0e};
    tm1637.display(msg);
  } 
}

/*void loop() {
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
}*/
