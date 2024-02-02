#include <Arduino.h>
#include <stdio.h>
#include <stdint.h>

const int ASeg = 11;
const int BSeg = 7;
const int CSeg = 3;
const int DSeg = 5;
const int ESeg = 6;
const int FSeg = 10;
const int GSeg = 2;
const int DotSeg = 4;

const int KetaSegments[4] = { 12, 9, 8, 13 };

const int ConfirmBtn = A0;
const int SelectBtn = A1;

const int Speaker = A3;

const int BrightnessPIN = A2;

const int StopAlarmBtn = A4;

const int LED_ON = LOW;
const int LED_OFF = HIGH;

const int LED_KETA_ON = HIGH;
const int LED_KETA_OFF = LOW;
// delay(ms) /
const int TIMER_DELAY = 2;  // セグメントの文字を切り替える遅延なのでなるべく小さく

// セグメントの値
uint8_t segChars[4] = { 0, 0, 0, 0 };
uint8_t segSelector = 3;

// アラーム時刻
uint8_t alarmTime[4] = { 0, 0, 0, 0 };

// アラームが鳴っているか
bool isAlarm = true;

int recieveByte = 0;
String bufferStr = "";
String okStr = "OK";

/* initial setting 初期設定 */
void setup() {
  /* ログ用のシリアル通信 */

  Serial.begin(9600);

  /* 7Seg 使用ピン定義 */
  pinMode(ASeg, OUTPUT);
  pinMode(BSeg, OUTPUT);
  pinMode(CSeg, OUTPUT);
  pinMode(DSeg, OUTPUT);
  pinMode(ESeg, OUTPUT);
  pinMode(FSeg, OUTPUT);
  pinMode(GSeg, OUTPUT);
  pinMode(DotSeg, OUTPUT);

  /* 7Seg 桁決定用 */
  for (int i = 0; i < 4; i++) pinMode(KetaSegments[i], OUTPUT);

  for (int i = 0; i < 4; i++) digitalWrite(KetaSegments[i], LOW);

  /* 時刻調整用ボタン */
  pinMode(ConfirmBtn, INPUT);
  pinMode(SelectBtn, INPUT);
  pinMode(StopAlarmBtn, INPUT);

  pinMode(Speaker, INPUT);

  /* 明るさ取得用 */
  pinMode(BrightnessPIN, INPUT);
}

// Display function 0
void light(int target, int num) {

  digitalWrite(DotSeg, LOW);

  for (int i = 0; i < 12; i++) {
    digitalWrite(i, LOW);
  }

  for (int i = 0; i < 4; i++) digitalWrite(KetaSegments[i], LOW);
  if (num != 10) digitalWrite(target, HIGH);

  switch (num) {
    case 0:
      digitalWrite(ASeg, LED_ON);
      digitalWrite(BSeg, LED_ON);
      digitalWrite(CSeg, LED_ON);
      digitalWrite(DSeg, LED_ON);
      digitalWrite(ESeg, LED_ON);
      digitalWrite(FSeg, LED_ON);
      digitalWrite(GSeg, LED_OFF);
      break;
    case 1:
      digitalWrite(ASeg, LED_OFF);
      digitalWrite(BSeg, LED_ON);
      digitalWrite(CSeg, LED_ON);
      digitalWrite(DSeg, LED_OFF);
      digitalWrite(ESeg, LED_OFF);
      digitalWrite(FSeg, LED_OFF);
      digitalWrite(GSeg, LED_OFF);
      break;
    case 2:
      digitalWrite(ASeg, LED_ON);
      digitalWrite(BSeg, LED_ON);
      digitalWrite(CSeg, LED_OFF);
      digitalWrite(DSeg, LED_ON);
      digitalWrite(ESeg, LED_ON);
      digitalWrite(FSeg, LED_OFF);
      digitalWrite(GSeg, LED_ON);
      break;
    case 3:
      digitalWrite(ASeg, LED_ON);
      digitalWrite(BSeg, LED_ON);
      digitalWrite(CSeg, LED_ON);
      digitalWrite(DSeg, LED_ON);
      digitalWrite(ESeg, LED_OFF);
      digitalWrite(FSeg, LED_OFF);
      digitalWrite(GSeg, LED_ON);
      break;
    case 4:
      digitalWrite(ASeg, LED_OFF);
      digitalWrite(BSeg, LED_ON);
      digitalWrite(CSeg, LED_ON);
      digitalWrite(DSeg, LED_OFF);
      digitalWrite(ESeg, LED_OFF);
      digitalWrite(FSeg, LED_ON);
      digitalWrite(GSeg, LED_ON);
      break;
    case 5:
      digitalWrite(ASeg, LED_ON);
      digitalWrite(BSeg, LED_OFF);
      digitalWrite(CSeg, LED_ON);
      digitalWrite(DSeg, LED_ON);
      digitalWrite(ESeg, LED_OFF);
      digitalWrite(FSeg, LED_ON);
      digitalWrite(GSeg, LED_ON);
      break;
    case 6:
      digitalWrite(ASeg, LED_ON);
      digitalWrite(BSeg, LED_OFF);
      digitalWrite(CSeg, LED_ON);
      digitalWrite(DSeg, LED_ON);
      digitalWrite(ESeg, LED_ON);
      digitalWrite(FSeg, LED_ON);
      digitalWrite(GSeg, LED_ON);
      break;
    case 7:
      digitalWrite(ASeg, LED_ON);
      digitalWrite(BSeg, LED_ON);
      digitalWrite(CSeg, LED_ON);
      digitalWrite(DSeg, LED_OFF);
      digitalWrite(ESeg, LED_OFF);
      digitalWrite(FSeg, LED_OFF);
      digitalWrite(GSeg, LED_OFF);
      break;
    case 8:
      digitalWrite(ASeg, LED_ON);
      digitalWrite(BSeg, LED_ON);
      digitalWrite(CSeg, LED_ON);
      digitalWrite(DSeg, LED_ON);
      digitalWrite(ESeg, LED_ON);
      digitalWrite(FSeg, LED_ON);
      digitalWrite(GSeg, LED_ON);
      break;
    case 9:
      digitalWrite(ASeg, LED_ON);
      digitalWrite(BSeg, LED_ON);
      digitalWrite(CSeg, LED_ON);
      digitalWrite(DSeg, LED_ON);
      digitalWrite(ESeg, LED_OFF);
      digitalWrite(FSeg, LED_ON);
      digitalWrite(GSeg, LED_ON);
      break;
    case 10:
      digitalWrite(ASeg, LED_OFF);
      digitalWrite(BSeg, LED_OFF);
      digitalWrite(CSeg, LED_OFF);
      digitalWrite(DSeg, LED_OFF);
      digitalWrite(ESeg, LED_OFF);
      digitalWrite(FSeg, LED_OFF);
      digitalWrite(GSeg, LED_OFF);
      break;
    case 11:
      digitalWrite(ASeg, LED_OFF);
      digitalWrite(BSeg, LED_OFF);
      digitalWrite(CSeg, LED_OFF);
      digitalWrite(DSeg, LED_ON);
      digitalWrite(ESeg, LED_OFF);
      digitalWrite(FSeg, LED_OFF);
      digitalWrite(GSeg, LED_OFF);
      break;
  }
  digitalWrite(DotSeg, LED_OFF);
}

int ctoi(char c) {
  if (c >= '0' && c <= '9') {
    return c - '0';
  }
  return 0;
}
int segset = 0;
int values[4] = { 0, 0, 0, 0 };

/* Mail loop 通常処理（繰り返し) */
void loop() {

  for (int i = 0; i < 4; i++) {
    light(KetaSegments[i], values[i]);
    delay(2);
  }

  bufferStr = "";
  while (Serial.available() > 0) {
    recieveByte = Serial.read();
    if (recieveByte == (int)'\n') break;
    bufferStr.concat((char)recieveByte);

    if (bufferStr.length() == 4) {
      Serial.print("I received: ");
      Serial.println(bufferStr);
      Serial.print("Length: ");
      Serial.println(bufferStr.length());
      for (int i = 0; i < 4; i++) {
        values[i] = ctoi(bufferStr[i]);
      }
    }
  }
}