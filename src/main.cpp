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

// サーボモータを使用する場合はこちらをインポートの上、stack-chan-testerを参考に実装
// https://github.com/mongonta0716/stack-chan-tester
// #include <Stackchan_servo.h>

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

  M5.Speaker.setVolume(100);

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

  // バッテリーアイコンの表示／非表示
  avatar.setBatteryIcon(true);

  // フォントの指定
  avatar.setSpeechFont(&fonts::lgfxJapanGothicP_16);

  // すべてのLEDを消す
  pixels.clear();
  pixels.show();
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
