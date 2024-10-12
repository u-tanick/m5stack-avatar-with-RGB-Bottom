#include <M5Unified.h>
#include <Avatar.h>
#include <Adafruit_NeoPixel.h>

using namespace m5avatar;
Avatar avatar;

#define PIN        5  // ネオピクセルピンデフォ S3の場合
#define NUMPIXELS  4  // ネオピクセルの数

// NeoPixelオブジェクトを作成
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
	auto cfg = M5.config();		/// 設定用の構造体を取得。
	M5.begin(cfg);				/// M5Unifiedを使用する準備をする。
  pixels.begin(); // NeoPixelの初期化

  avatar.init(8);
}

void loop() {
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
  delay(1000);
}