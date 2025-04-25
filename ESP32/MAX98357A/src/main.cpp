#include <AudioFileSourceFS.h>
#include <AudioGeneratorMP3.h>
#include <AudioOutputI2S.h>
#include <SPIFFS.h>

AudioGeneratorMP3 *mp3;
AudioFileSourceFS *file;
AudioFileSourceFS *file2;
AudioOutputI2S *out;

void setup() {
  Serial.begin(9600);
  delay(1000);

  if (!SPIFFS.begin(true)) {  // true = format nếu chưa được định dạng
    Serial.println("❌ Không thể mount SPIFFS.");
    return;
  }

  Serial.println("✅ SPIFFS đã sẵn sàng.");

  // Đảm bảo file tồn tại
  if (!SPIFFS.exists("/green.mp3")) {
    Serial.println("❌ Không tìm thấy file /green.mp3");
    return;
  }

  file = new AudioFileSourceFS(SPIFFS, "/green.mp3");
  out = new AudioOutputI2S();
  
  // Thiết lập chân I2S: BCLK, LRCK, DIN
  out->SetPinout(26, 25, 22);
  out->SetGain(0.4);  // Âm lượng

  mp3 = new AudioGeneratorMP3();
  mp3->begin(file, out);

  if (SPIFFS.exists("/green.mp3")) {
    Serial.println("tìm thấy file /green.mp3");
    return;
  }

  Serial.println("▶️ Bắt đầu phát nhạc MP3...");
}

int number = 0;

void loop() {
  if (mp3 && mp3->isRunning()) {
    if (!mp3->loop()) {
      Serial.print(++number);
      Serial.println("⏹️ Đã phát xong MP3 (theo loop trả false)");
      //mp3->stop();  // Dừng hẳn, giải phóng
      mp3->stop();  // nên gọi stop() trước khi delete
      delete mp3;   // giải phóng bộ nhớ
      mp3 = nullptr;
    }
  } else {
    // Có thể đặt lại nếu muốn phát lại
    // hoặc dừng hẳn
    Serial.print("im here");
    mp3 = new AudioGeneratorMP3();
    file = new AudioFileSourceFS(SPIFFS, "/green.mp3");
    file->seek(0, SEEK_SET);
    mp3->begin(file, out);
  }
}