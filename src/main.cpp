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
// for Servo
// サーボモータを使用する場合はこちらをインポートの上、stack-chan-testerを参考に実装
// https://github.com/mongonta0716/stack-chan-tester
#include <Stackchan_servo.h>

#define START_DEGREE_VALUE_X 90
#define START_DEGREE_VALUE_Y 90

StackchanSERVO servo;
StackchanSystemConfig system_config; // Use for Servo and SD
bool core_port_a = false;         // Core1のPortAを使っているかどうか

// ----------------------------------------------
// for SD Card
// SDカードを使用する場合はこちらをインポートの上、stack-chan-testerを参考に実装
// #include <SD.h>

// ----------------------------------------------
// for Avatar
#include <Avatar.h> // https://github.com/meganetaaan/m5stack-avatar
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
// Any Functions
#define USE_SERVO
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

void Servo_setup() {
#ifdef USE_SERVO

  if(SERVO_PIN_X == 1) {
      // M5Stack CoreS3でPort.Aを使う場合は内部I2CをOffにする必要がある。
      // M5.In_I2C.release();
      // バッテリーアイコンも表示不可となるため非表示
      avatar.setBatteryIcon(false);
  }

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

void moveX(int x, uint32_t millis_for_move = 0) {
#ifdef USE_SERVO
  if (millis_for_move == 0) {
    servo_x.easeTo(x + servo_offset_x);
  } else {
    servo_x.easeToD(x + servo_offset_x, millis_for_move);
  }
#endif
}

void moveY(int y, uint32_t millis_for_move = 0) {
#ifdef USE_SERVO
  if (millis_for_move == 0) {
    servo_y.easeTo(y + servo_offset_y);
  } else {
    servo_y.easeToD(y + servo_offset_y, millis_for_move);
  }
#endif
}

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

  // ログ設定
  M5.Log.setLogLevel(m5::log_target_display, ESP_LOG_NONE);
  M5.Log.setLogLevel(m5::log_target_serial, ESP_LOG_INFO);
  M5.Log.setEnableColor(m5::log_target_serial, false);
  M5_LOGI("Hello World");

  // スピーカーのボリューム設定(0 - 255)
  M5.Speaker.setVolume(100);

  Servo_setup();

  // if (M5.getBoard() == m5::board_t::board_M5Stack) {
  //   if (system_config.getServoInfo(AXIS_X)->pin == 22) {
  //     // M5Stack CoreS3の場合、Port.Aを使う場合は内部I2CをOffにする必要がある。バッテリー表示は不可。
  //     // バッテリーアイコンを非表示
  //     avatar.setBatteryIcon(false);
  //     M5.In_I2C.release();
  //     core_port_a = true;
  //   }
  // } else {
  //   // バッテリーアイコンを表示
  //   avatar.setBatteryIcon(true);
  // }
  // // for Servo
  // servo.begin(system_config.getServoInfo(AXIS_X)->pin, system_config.getServoInfo(AXIS_X)->start_degree,
  //             system_config.getServoInfo(AXIS_X)->offset,
  //             system_config.getServoInfo(AXIS_Y)->pin, system_config.getServoInfo(AXIS_Y)->start_degree,
  //             system_config.getServoInfo(AXIS_Y)->offset,
  //             (ServoType)system_config.getServoType());

  M5.Power.setExtOutput(!system_config.getUseTakaoBase()); // 設定ファイルのTakaoBaseがtrueの場合は、Groveポートの5V出力をONにする。

  // M5_LOGI("ServoType: %d\n", system_config.getServoType());

  // 顔のサイズやポジション変更
  // 例：m5atomS3用
  // M5.Lcd.setRotation(0);
  // avatar.setScale(0.50);
  // avatar.setPosition(-60, -96);

  // 顔の色変更
  // TFT_WHITEなど既定の色が多数用意されている
  // オリジナルの色は以下のコードで定義可能
  // uint16_t customColor = 0;
  // customColor = M5.Lcd.color565(255,140,50);
  cp = new ColorPalette();
  cp->set(COLOR_PRIMARY, TFT_WHITE);
  cp->set(COLOR_BACKGROUND, TFT_DARKGRAY);
  avatar.setColorPalette(*cp);

  avatar.init(8);

  // フォントの指定
  avatar.setSpeechFont(&fonts::lgfxJapanGothicP_16);

  // すべてのLEDを消す
  pixels.clear();
  pixels.show();

  // サーボテスト挙動
  // testServo();
    moveX(90);
    moveY(90);

}

// ----------------------------------------------
char speechText[100];  // フォーマットされた文字列を格納するためのバッファ
void loop() {

// CoreS3のボタン処理有効化（画面=タッチパネルを[右][中央][左]に3等分した領域がそれぞれBtnA, BtnB, BtnCに対応）
// 関数内でボタン処理を行う場合は、各関数の中でM5.update();とセットで必ず宣言
#ifdef ARDUINO_M5STACK_CORES3
  unifiedButton.update(); // M5.update() よりも前に呼ぶ事
#endif
  M5.update();

  // ボタンA
  // スピーカーを鳴らす、M5Stack-Avatarの表情変更、M5Stack-Avatarの台詞表示
  if (M5.BtnA.wasPressed()) {
      M5.Speaker.tone(1000, 200);
      avatar.setExpression(Expression::Happy);
      avatar.setSpeechText("御用でしょうか？");
  }

  // ボタンB
  // M5Stack-Avatarの台詞をテキスト変数で渡して表示、変数をログに出力
  if (M5.BtnB.wasPressed()) {
      M5.Speaker.tone(1500, 200);
      avatar.setExpression(Expression::Neutral);
      char buff[100];
      sprintf(buff,"こんにちわ！");
      avatar.setSpeechText(buff);
      for (int i=0; i<2; i++) {
        moveY(50);
        moveY(90);
      }
      M5_LOGI("SpeechText: %c\n", buff);
  }

  // ボタンC
  // M5Stack-Avatarの顔変更
  if (M5.BtnC.wasPressed()) {
      M5.Speaker.tone(2000, 200);
      cp = new ColorPalette();
      cp->set(COLOR_PRIMARY, TFT_BLACK);
      cp->set(COLOR_BACKGROUND, TFT_WHITE);
      avatar.setColorPalette(*cp);
      avatar.setSpeechText("ばすぴすー");
      avatar.setFace(new SacabambaspisFace());
      doIllumination();
  }

  delay(1);
}
