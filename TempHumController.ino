//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#include "DHT.h"
 
#define DHTPIN 2        // SDA 핀의 설정
#define DHTTYPE DHT22   // DHT22 (AM2302) 센서종류 설정

#define TEMP_PIN 4  // 온도 히터
#define HUMI_PIN 5  // 가습기 히터

#define TEMP_UP 11 // 온도설정 UP
#define TEMP_DOWN 10 // 온도설정 DOWN
#define HUMI_UP 9 // 습도설정 UP
#define HUMI_DOWN 8 // 습도설정 DOWN

#define TEMP_MAX 50 // 온도 최대치
#define TEMP_MIN 20 // 온도 최저치
#define HUMI_MAX 90 // 습도 최대치
#define HUMI_MIN 40 // 습도 최저치

#define TEMP_CONST_MAX 5 // 습도 조정갑 최대치
#define HUMI_CONST_MAX 5 // 온도 조정값 최대치


LiquidCrystal_I2C lcd(0x27,20,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  lcd.init();                      // initialize the lcd 
  //lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Temp & Humid CTL");
  lcd.setCursor(0,1);
  lcd.print("by winchild");

  delay (1000);

  // 히터 릴레이 초기화
  pinMode (TEMP_PIN, OUTPUT);
  pinMode (HUMI_PIN, OUTPUT);

  // 온도 및 습도설정용 스위치, 아두이노 풀업저항 사용
  pinMode(TEMP_UP,INPUT_PULLUP);
  pinMode(TEMP_DOWN,INPUT_PULLUP);
  pinMode(HUMI_UP,INPUT_PULLUP);
  pinMode(HUMI_DOWN,INPUT_PULLUP);


  Serial.begin(115200); 
  Serial.println("Temperature Humidity Controller start!!!");
 
  dht.begin();
}

int dh = 80; // target humidity
int dt = 26; // target temperature

int setting = 0; //1;
int toggle = 0;
//int humi_opr = 0; //1;
//int temp_opr = 1; //0;

int t_const = 2;  // 온도 보정수치 - 가열식이므로 잠열에 의한 온도 상승 보정치
int h_const = 3;  // 가습 보정수치 - 가열식이므로 잠열에 의한 온도 상승 보정치
 
void loop() {

  int temp_up, temp_down;
  int humi_up, humi_down;
  
  Serial.print("Setting: "); Serial.println(setting);

  // 센서의 온도와 습도를 읽어온다.
  int h = dht.readHumidity();
  int t = dht.readTemperature();

  Serial.print("Temperature:"); Serial.print(t); Serial.print("% Target: "); Serial.print(dt);
  Serial.print("% Adjust: "); Serial.println(t_const);
  Serial.print("Humidity:"); Serial.print(h); Serial.print("% Target: "); Serial.print(dh);
  Serial.print("% Adjust: "); Serial.println(h_const);
 
  if (isnan(t) || isnan(h)) {
    //값 읽기 실패시 시리얼 모니터 출력
    Serial.println("Failed to read from DHT");
    lcd.setCursor (0,0); lcd.print("Value Get Error");
  } else {

    // 온도 표시 LCD 출력    
    lcd.setCursor (0,0); lcd.print("Temp: ");lcd.print(t);lcd.print("/");
    if (setting) {
        lcd.print(t_const);
        lcd.print (" ");
    }
    else lcd.print(dt);
    lcd.print ("C [");
    if ((t+t_const) < dt) {
      digitalWrite (TEMP_PIN, LOW); // ON
      if (toggle) lcd.print("*");
      else lcd.print (" ");
    }
    else {
      digitalWrite (TEMP_PIN, HIGH); // OFF
      lcd.print(" ");
    }
    lcd.print ("]");     

    // 습도 표시 LCD 출력
    lcd.setCursor (0,1); lcd.print("Humi: ");lcd.print(h);lcd.print("/");
    if (setting) {
        lcd.print(h_const);
        lcd.print(" ");
    }
    else lcd.print(dh);
    lcd.print ("% [");
    if ((h+h_const) < dh) {
      digitalWrite (HUMI_PIN, LOW);  // ON
      if (toggle) lcd.print("*");
      else lcd.print(" ");
    }
    else {
      digitalWrite (HUMI_PIN, HIGH);  // OFF
      lcd.print(" ");
    }
    lcd.print ("]");

  }


  // 깜박임을 표시하기 위한 토글 플래그
  if (toggle) toggle = 0;
  else toggle = 1;
  
  humi_up = digitalRead (HUMI_UP);
  humi_down = digitalRead (HUMI_DOWN);
  Serial.print("HUMI UP:"); Serial.print(humi_up); Serial.print(" DOWN: "); Serial.println(humi_down);
 
  temp_up = digitalRead (TEMP_UP);
  temp_down = digitalRead (TEMP_DOWN);
  Serial.print("TEMP UP:"); Serial.print(temp_up); Serial.print(" DOWN: "); Serial.println(temp_down);
  
  if (temp_up == LOW && humi_up == LOW) {
    if (setting) setting = 0;
    else setting = 1;
  }
  else
  {
    if (setting)
    {
      if (temp_up == LOW && t_const < TEMP_CONST_MAX) t_const++;
      if (temp_down == LOW && t_const > 0) t_const--;
      if (humi_up == LOW && h_const < HUMI_CONST_MAX) h_const++;
      if (humi_down == LOW && h_const > 0) h_const--;     
    }
    else
    {
      if (temp_up == LOW && dt < TEMP_MAX) dt++;
      if (temp_down == LOW && dt > TEMP_MIN) dt--;
      if (humi_up == LOW && dh < HUMI_MAX) dh++;
      if (humi_down == LOW && dh > HUMI_MIN) dh--;
    }


  }
  
  delay(500);
}



