// ----------------------------------------------
// for M5Stack
// #include <Arduino.h>
#include <M5Unified.h>
#include <M5StackUpdater.h>

// ----------------------------------------------
// for Stackchan
#include <Stackchan_system_config.h>

// CoreS3を使用する場合は#defineを有効化
#define ARDUINO_M5STACK_CORES3
#ifdef ARDUINO_M5STACK_CORES3
#include <gob_unifiedButton.hpp>
goblib::UnifiedButton unifiedButton;
#endif

// ----------------------------------------------
// for Avatar
#include <Avatar.h>
#include "face/SacabambaspisFace.h"
using namespace m5avatar;
Avatar avatar;
ColorPalette *cp;

// ----------------------------------------------
// for RGB Bottom
#include <Adafruit_NeoPixel.h>
#define PIN        5  // ネオピクセルピンデフォ S3の場合
#define NUMPIXELS  4  // ネオピクセルの数

// NeoPixelオブジェクトを作成
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// NeoPixelを順に色を変えつつ点灯
void doIllumination() {
  // 赤色
  for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 0, 0));
    pixels.show();
    delay(500);
  }
  
  // 緑色
  for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 255, 0));
    pixels.show();
    delay(500);
  }
  
  // 青色
  for(int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 255));
    pixels.show();
    delay(500);
  }
  
  // すべてのLEDを消す
  pixels.clear();
  pixels.show();
}

// ----------------------------------------------
// for Servo
#include <Stackchan_servo.h>
#define USE_SERVO

StackchanSERVO servo;
StackchanSystemConfig system_config; // Use for Servo and SD

#ifdef USE_SERVO
#if defined(ARDUINO_M5STACK_CORES3)
  // #define SERVO_PIN_X 18  //CoreS3 PORT C
  // #define SERVO_PIN_Y 17  //CoreS3 PORT C
  #define SERVO_PIN_X 1  //CoreS3 PORT A
  #define SERVO_PIN_Y 2  //CoreS3 PORT A
#endif
#define START_DEGREE_VALUE_X 90
#define START_DEGREE_VALUE_Y 90 
ServoEasing servo_x;
ServoEasing servo_y;
int servo_offset_x = 0;  // X軸サーボのオフセット（90°からの+-で設定）
int servo_offset_y = 0;  // Y軸サーボのオフセット（90°からの+-で設定）
#endif

// サーボの初期セットアップ用
void Servo_setup() {
#ifdef USE_SERVO
  if (servo_x.attach(SERVO_PIN_X, START_DEGREE_VALUE_X, DEFAULT_MICROSECONDS_FOR_0_DEGREE, DEFAULT_MICROSECONDS_FOR_180_DEGREE)) {
    Serial.print("Error attaching servo x");
  }
  if (servo_y.attach(SERVO_PIN_Y, START_DEGREE_VALUE_Y, DEFAULT_MICROSECONDS_FOR_0_DEGREE, DEFAULT_MICROSECONDS_FOR_180_DEGREE)) {
    Serial.print("Error attaching servo y");
  }
  servo_x.setEasingType(EASE_QUADRATIC_IN_OUT);
  servo_y.setEasingType(EASE_QUADRATIC_IN_OUT);
  setSpeedForAllServos(30);

  servo_x.setEaseTo(START_DEGREE_VALUE_X); 
  servo_y.setEaseTo(START_DEGREE_VALUE_Y);
  synchronizeAllServosStartAndWaitForAllServosToStop();
#endif
}

// X軸動作用
void moveX(int x, uint32_t millis_for_move = 0) {
#ifdef USE_SERVO
  if (millis_for_move == 0) {
    servo_x.easeTo(x + servo_offset_x);
  } else {
    servo_x.easeToD(x + servo_offset_x, millis_for_move);
  }
#endif
}

// Y軸動作用
void moveY(int y, uint32_t millis_for_move = 0) {
#ifdef USE_SERVO
  if (millis_for_move == 0) {
    servo_y.easeTo(y + servo_offset_y);
  } else {
    servo_y.easeToD(y + servo_offset_y, millis_for_move);
  }
#endif
}

// サーボのテスト挙動用
void testServo() {
#ifdef USE_SERVO
  for (int i=0; i<2; i++) {
    avatar.setSpeechText("X 90 -> 0");
    moveX(0);
    avatar.setSpeechText("X 0 -> 180");
    moveX(180);
    avatar.setSpeechText("X 180 -> 90");
    moveX(90);
    avatar.setSpeechText("Y 90 -> 50");
    moveY(50);
    avatar.setSpeechText("Y 50 -> 90");
    moveY(90);
  }
  avatar.setSpeechText("Test Complete");
  delay(1000);
  avatar.setSpeechText("");
#endif
}

// ----------------------------------------------
// Any Functions


// ----------------------------------------------
// main Setup
void setup() {
  // 設定用の情報を抽出
  auto cfg = M5.config();
  // Groveポートの出力をしない（m5atomS3用）
  // cfg.output_power = true;
  // M5Stackをcfgの設定で初期化
  M5.begin(cfg);

  // for RGB Bottom
  pixels.begin(); // NeoPixelの初期化

#ifdef ARDUINO_M5STACK_CORES3
  unifiedButton.begin(&M5.Display, goblib::UnifiedButton::appearance_t::transparent_all);
#endif

  // スピーカーのボリューム設定(0 - 255)
  M5.Speaker.setVolume(100);

  // for Servo(初期セットアップ)
  Servo_setup();

  // 設定ファイルのTakaoBaseがtrueの場合は、Groveポートの5V出力をONにする。
  M5.Power.setExtOutput(!system_config.getUseTakaoBase());

  // 顔の色セットを指定
  cp = new ColorPalette();
  cp->set(COLOR_PRIMARY, TFT_WHITE);
  cp->set(COLOR_BACKGROUND, TFT_DARKGRAY);
  avatar.setColorPalette(*cp);

  // アバターの初期設定、多色対応のため引数に 8 をセット
  avatar.init(8);

  // フォントの指定
  avatar.setSpeechFont(&fonts::lgfxJapanGothicP_12);

  // すべてのLEDを消す
  pixels.clear();
  pixels.show();

  // サーボテスト挙動
  // testServo();
  // サーボ初期位置セット
  moveX(90);
  moveY(90);

}

// ----------------------------------------------
// main Loop
void loop() {

// CoreS3のボタン処理有効化（画面=タッチパネルを[右][中央][左]に3等分した領域がそれぞれBtnA, BtnB, BtnCに対応）
// 関数内でボタン処理を行う場合は、各関数の中でM5.update();とセットで必ず宣言
#ifdef ARDUINO_M5STACK_CORES3
  unifiedButton.update(); // M5.update() よりも前に呼ぶ事
#endif
  M5.update();

  // ボタンA
  // 表情とテキストを表示するだけ
  if (M5.BtnA.wasPressed()) {
      M5.Speaker.tone(1000, 200);
      avatar.setExpression(Expression::Happy);
      avatar.setSpeechText("御用でしょうか？");
  }

  // ボタンB
  // 顔を上下に振る
  if (M5.BtnB.wasPressed()) {
      M5.Speaker.tone(1500, 200);
      avatar.setExpression(Expression::Neutral);
      avatar.setSpeechText("こんにちわ！");
      for (int i=0; i<2; i++) {
        moveY(50);
        moveY(90);
      }
  }

  // ボタンC
  // 顔を変更し、RGBボトムを点灯
  if (M5.BtnC.wasPressed()) {
      M5.Speaker.tone(2000, 200);
      cp = new ColorPalette();
      cp->set(COLOR_PRIMARY, TFT_BLACK);
      cp->set(COLOR_BACKGROUND, TFT_WHITE);
      avatar.setColorPalette(*cp);
      avatar.setSpeechText("ばすぴすー");
      avatar.setFace(new SacabambaspisFace());
      // RGBボトムを点灯
      doIllumination();
  }

  delay(1);
}
