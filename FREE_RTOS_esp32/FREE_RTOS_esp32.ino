#include <Arduino.h>
#include <SHT3x.h>
SHT3x Sensor;

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#include <WiFiManager.h>
#include <ArduinoJson.h>

#include "FirebaseESP32.h"
#define FIREBASE_HOST "doanchuyennganh-37767-default-rtdb.firebaseio.com/" //2 cái này lấy trong FB nhé
#define FIREBASE_AUTH "9rUcE6LlTwmjlDkybL0e7bvUYzrMwDYVSI8OKKef"
FirebaseData fb;

unsigned long t1 = 0;
float temp = 25.2;
float humi = 60.5;
int stt_update;
int stt_dv[4] = {0, 0, 0, 0};
int output_dv[] = {13, 26, 14, 27};
int input_btn[] = {25, 33, 32, 35};
int stt_input[] = {1, 1, 1, 1};
//Wifi client;
String path = "/";
FirebaseJson json;
// Prototype của các task
void task1(void *parameter);
void task2(void *parameter);
void task3(void *parameter);
void task4(void *parameter);

void setup() {
  pinMode(13, OUTPUT);
  pinMode(26, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(27, OUTPUT);
  pinMode(25, INPUT_PULLUP);
  pinMode(33, INPUT_PULLUP);
  pinMode(32, INPUT_PULLUP);
  pinMode(35, INPUT_PULLUP);
  //
  for (int i = 0; i < 4; i++) {
    digitalWrite(output_dv[i], stt_dv[i]);
  }
  Serial.begin(115200); // Khởi tạo Serial để in ra Serial Monitor
  //lcd
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("ESP 32 ");
  Serial.begin(115200);
  // Khởi động WiFiManager
  WiFiManager wifiManager;
  // reset và erase cài đặt trước đó
  // wifiManager.resetSettings();
  // tên thiết bị, để tạo SSID mặc định
  wifiManager.autoConnect("ESP32_MH ");
  // Sau khi kết nối thành công
  Serial.println("Kết nối WiFi thành công!");
  lcd.setCursor(0, 1);                                                                                                                                    
  lcd.print("connect wifi success!");
  //firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.getInt(fb, path + "/Data_device/user1/dv1"); //lấy gtri từ Firebase về gán vào biến x
  stt_dv[0] = fb.intData(); //lay gtri tu fb ve esp
  Firebase.getInt(fb, path + "/Data_device/user1/dv2"); //lấy gtri từ Firebase về gán vào biến x
  stt_dv[1] = fb.intData(); //lay gtri tu fb ve esp
  Firebase.getInt(fb, path + "/Data_device/user1/dv3"); //lấy gtri từ Firebase về gán vào biến x
  stt_dv[2] = fb.intData(); //lay gtri tu fb ve esp
  Firebase.getInt(fb, path + "/Data_device/user1/dv4"); //lấy gtri từ Firebase về gán vào biến x
  stt_dv[3] = fb.intData(); //lay gtri tu fb ve esp
  // Tạo các task với các ưu tiên khác nhau
  xTaskCreatePinnedToCore(task1, "Task 1", 10000, NULL, 3, NULL, 0);
  xTaskCreatePinnedToCore(task2, "Task 2", 10000, NULL, 3, NULL, 1);
  xTaskCreatePinnedToCore(task3, "Task 3", 50000, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(task4, "Task 4", 10000, NULL, 3, NULL, 1);
}

void loop() {
  // Không cần có code trong loop khi sử dụng FreeRTOS
}

// Task 1
void task1(void *parameter) {
  for (;;) {
    Serial.println("Task 1 is running");
    get_data_Sensor();
    print_lcd();
    vTaskDelay(2000 / portTICK_PERIOD_MS); // Delay 2s
  }
}

// Task 2
void task2(void *parameter) {
  for (;;) {
    Serial.println("Task 2 is running");
    for (int i = 0; i < 4; i++) {
      digitalWrite(output_dv[i], stt_dv[i]);
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay 1s
  }
}

// Task 3
void task3(void *parameter) {
  for (;;) {
    Serial.println("Task 3 is running");
    if (millis() - t1 > 1000) {
      Firebase.setString(fb, path + "/Data/Data_sensor/hummid", (String)(int(humi)));
      Firebase.setString(fb, path + "/Data/Data_sensor/temp", (String)(int(temp))) ;
      t1 = millis();
    } if (stt_update == 0) {
      Firebase.getInt(fb, path + "/Data_device/user1/dv1"); //lấy gtri từ Firebase về gán vào biến x
      stt_dv[0] = fb.intData(); //lay gtri tu fb ve esp
      Firebase.getInt(fb, path + "/Data_device/user1/dv2"); //lấy gtri từ Firebase về gán vào biến x
      stt_dv[1] = fb.intData(); //lay gtri tu fb ve esp
      Firebase.getInt(fb, path + "/Data_device/user1/dv3"); //lấy gtri từ Firebase về gán vào biến x
      stt_dv[2] = fb.intData(); //lay gtri tu fb ve esp
      Firebase.getInt(fb, path + "/Data_device/user1/dv4"); //lấy gtri từ Firebase về gán vào biến x
      stt_dv[3] = fb.intData(); //lay gtri tu fb ve esp
    }
    // gửi dữ liệu button lên Firebase
    if (stt_update == 1) {
      Firebase.setInt(fb, path + "/Data_device/user1/dv1", stt_dv[0]);
      Firebase.setInt(fb, path + "/Data_device/user1/dv2", stt_dv[1]);
      Firebase.setInt(fb, path + "/Data_device/user1/dv3", stt_dv[2]);
      Firebase.setInt(fb, path + "/Data_device/user1/dv4", stt_dv[3]);
      stt_update = 0;
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay 1s
  }
}

// Task 4
void task4(void *parameter) {
  for (;;) {
    Serial.println("Task 4 is running");
    read_button();
    vTaskDelay(500 / portTICK_PERIOD_MS); // Delay 500ms
  }
}
void print_lcd() {
  //Hiển thị lên LCD
  lcd.setCursor(0, 0);
  lcd.print("Temp  : ");
  lcd.print(abs(temp));
  lcd.print((char)223);
  lcd.print("C ");
  lcd.setCursor(0, 1);
  lcd.print("Humid : ");
  lcd.print(humi);
  lcd.print("%H   ");
}
void get_data_Sensor()
{ Sensor.UpdateData();
  temp = Sensor.GetTemperature();
  temp = round(temp * 10) / 10;
  humi = Sensor.GetRelHumidity();
  humi = round(humi * 10) / 10;
}
void read_button() {
  for (int i = 0; i < 4; i++) {
    if (digitalRead(input_btn[i]) == LOW) {
      stt_input[i] = 1;
    }
    else {
      if (stt_input[i] == 1) {
        stt_dv[i] = !digitalRead(output_dv[i]);
        digitalWrite(output_dv[i], stt_dv[i]);
        stt_input[i] = 0;
        stt_update = 1;
      }
    }
  }
}
