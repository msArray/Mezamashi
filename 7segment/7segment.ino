#include <Arduino.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

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

int CheckBright = 100;

// セグメントの値
uint8_t segChars[4] = { 0, 0, 0, 0 };
uint8_t segSelector = 3;

// アラーム時刻
uint8_t alarmTime[4] = { 0, 0, 0, 0 };

// アラームが鳴っているか
bool isAlarm = true;

// データ取得用
int recieveByte = 0;
String bufferStr = "";
//int connectionDelay = 0;

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
  for (int i = 0; i < 4; i++)
    pinMode(KetaSegments[i], OUTPUT);

  for (int i = 0; i < 4; i++)
    digitalWrite(KetaSegments[i], LOW);

  /* 時刻調整用ボタン */
  pinMode(ConfirmBtn, INPUT_PULLUP);
  pinMode(SelectBtn, INPUT_PULLUP);
  pinMode(StopAlarmBtn, INPUT_PULLUP);

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

  for (int i = 0; i < 4; i++)
    digitalWrite(KetaSegments[i], LOW);
  if (num != 10)
    digitalWrite(target, HIGH);

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

int GetTimeVal() {
  String TimeValue;
  int TimeNum = -1;
  TimeValue = Serial.readString();
  TimeNum = TimeValue.toInt();
  return TimeNum;
}

int ctoi(char c) {
  if(c == '0') return 0;
  if (c >= '0' && c <= '9') {
    return c - '0';
  }
  return 0;
}

/* Mail loop 通常処理（繰り返し) */
void loop() {

  /* アラーム設定モードの残り時間 */
  static unsigned int modeSettingAlarm = 0;
  static unsigned int modeFixTime = 0;

  // 前のボタンの状態を記録
  static bool lastConfirmPush = false;
  static bool lastSelectPush = false;
  static bool lastStopAlarm = false;

  static long long alarmStart;

  if (isAlarm) {
    if (millis() % 50 > 25) {
      tone(Speaker, 1480);
    } else {
      tone(Speaker, 932);
    }
  } else {
    noTone(Speaker);
  }

  int Brightness = analogRead(BrightnessPIN);

  for (int i = 0; i < 4; i++) {
    if (segChars[i] >= 10)
      segChars[i] = 0;
    if (segChars[0] >= 3)                       
      segChars[0] = 0;
    if (segChars[0] == 2 && segChars[1] >= 4)
      segChars[1] = 0;
    if (segChars[2] >= 6)
      segChars[2] = 0;
  }
  for (int i = 0; i < 4; i++) {
    if (alarmTime[i] >= 10)
      alarmTime[i] = 0;
    if (alarmTime[0] >= 3)
      alarmTime[0] = 0;
    if (alarmTime[0] == 2 && alarmTime[1] >= 4)
      alarmTime[1] = 0;
    if (alarmTime[2] >= 6)
      alarmTime[2] = 0;
  }
  if (segSelector >= 4)
    segSelector = 0;

  if (modeSettingAlarm > 0) {
    /* TIMER_DELAYごとに各桁を表示 */
    for (int i = 0; i < 4; i++) {
      if ((millis() / 500) % 2 == 0 && i == segSelector && modeSettingAlarm > 0)
        light(KetaSegments[i], 10);
      else
        light(KetaSegments[i], alarmTime[i]);
      delay(TIMER_DELAY);
    };
  } else if (modeFixTime > 0) {
    /* TIMER_DELAYごとに各桁を表示 */
    for (int i = 0; i < 4; i++) {
      if ((millis() / 500) % 2 == 0 && i == segSelector && modeFixTime > 0)
        light(KetaSegments[i], 10);
      else
        light(KetaSegments[i], segChars[i]);
      delay(TIMER_DELAY);
    };
  } else if (Brightness >= CheckBright) {
    /* TIMER_DELAYごとに各桁を表示 */
    for (int i = 0; i < 4; i++) {
      if ((millis() / 500) % 2 == 0 && i == segSelector && modeSettingAlarm > 0)
        light(KetaSegments[i], 10);
      else
        light(KetaSegments[i], segChars[i]);
      delay(TIMER_DELAY);
    };
  } else {
    for (int i = 0; i < 4; i++) {
      light(KetaSegments[i], 10);
      delay(TIMER_DELAY);
    };
  }

  int state_A0 = digitalRead(ConfirmBtn);
  int state_A1 = digitalRead(SelectBtn);
  int state_A4 = digitalRead(StopAlarmBtn);
  const bool isConfirmPush = (state_A0 != 1 ? true : false);
  const bool isSelectPush = (state_A1 != 1 ? true : false);
  const bool isStopAlarm = (state_A4 != 1 ? true : false);

  /* ボタンの連続押しを禁止 */
  /*決定ボタン
  現在時刻やアラームの時間を1ずつ増やす　アラームも止める
  */
  if (isConfirmPush && !lastConfirmPush && modeSettingAlarm > 0) {
    alarmTime[segSelector]++;
    modeSettingAlarm = TIMER_DELAY * 5 * 50;
    isAlarm = false;
  } else if (isConfirmPush && !lastConfirmPush && modeFixTime > 0) {
    segChars[segSelector]++;
    modeFixTime = TIMER_DELAY * 5 * 50;
    isAlarm = false;
  }
  lastConfirmPush = isConfirmPush;

  /* Stopボタン
    アラームが鳴っている場合は停止
    なっていない場合は時刻修正モード
   */
  if (isStopAlarm && !lastStopAlarm) {
    if (isAlarm) {
      isAlarm = false;
    } else {
      segSelector++;  // select use segment
      modeFixTime = TIMER_DELAY * 5 * 50;
    }
  }
  lastStopAlarm = isStopAlarm;

  /* 選択ボタン
  アラームが鳴っていたら止める
  アラームの設定時刻の桁を１づつ増やす
  */
  if (isSelectPush && !lastSelectPush) {
    if (isAlarm) {
      isAlarm = false;
    } else {
      segSelector++;  // select use segment
      modeSettingAlarm = TIMER_DELAY * 5 * 50;
    }
  }
  lastSelectPush = isSelectPush;

  if (modeSettingAlarm > 0)
    modeSettingAlarm--;
  else if (modeFixTime > 0)
    modeFixTime--;
  else
    segSelector = 3;

  static bool isUpdateTime = false;
  if (modeFixTime == 0) {
    if ((millis() / 1000) % 60 == 0 && isUpdateTime == false) {
      segChars[3]++;
      if (segChars[3] > 9) {
        segChars[3] = 0;
        segChars[2]++;
      }
      if (segChars[2] > 5) {
        segChars[2] = 0;
        segChars[1]++;
      }
      if (segChars[0] == 2 && segChars[1] > 3) {
        segChars[1] = 0;
        segChars[0]++;
      }
      if (segChars[1] > 9) {
        segChars[1] = 0;
        segChars[0]++;
      }
      if (segChars[0] > 2) {
        segChars[0] = 0;
        segChars[1] = 0;
        segChars[2] = 0;
        segChars[3] = 0;
      }
      isUpdateTime = true;
      if (segChars[0] == alarmTime[0] && segChars[1] == alarmTime[1] && segChars[2] == alarmTime[2] && segChars[3] == alarmTime[3]) {
        isAlarm = true;
      }
    }
  }

  if ((millis() / 1000) % 60 != 0) {
    isUpdateTime = false;
  }

  bufferStr = "";
  while (Serial.available() > 0) {
    recieveByte = Serial.read();
    if (recieveByte == (int)'\n')
      break;
    bufferStr.concat((char)recieveByte);
    //Serial.println(bufferStr);

    if (bufferStr.length() == 4) {
      if (bufferStr[0] == 'c') {
        Serial.print(ctoi(bufferStr[1]) * 100 + ctoi(bufferStr[2]) * 10 + ctoi(bufferStr[3]));
        Serial.println("hg");
        CheckBright = ctoi(bufferStr[1]) * 100 + ctoi(bufferStr[2]) * 10 + ctoi(bufferStr[3]);
      }
    }

    if (bufferStr.length() == 5) {
      Serial.print("Arduino: Now Bright is ");
      Serial.println(Brightness);
      Serial.print("b");
      Serial.println(CheckBright);
      Serial.print("Arduino: now_alarm_");
      for (int i = 0; i < 4; i++) {
        Serial.print(alarmTime[i]);
      }
      Serial.println();
      if (bufferStr[0] == 't') {
        for (int i = 1; i < 5; i++) {
          segChars[i - 1] = ctoi(bufferStr[i]);
        }
      }
      if (bufferStr[0] == 'a') {
        for (int i = 1; i < 5; i++) {
          alarmTime[i - 1] = ctoi(bufferStr[i]);
        }
      }
    }
  }
}