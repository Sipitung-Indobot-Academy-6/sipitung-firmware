// Inisialisasi Libraries(Gunakan depedency yang diperlukan)
#include <WiFi.h>
#include <OneWire.h>
#include <WiFiManager.h> 
#include <SoftwareSerial.h>
#include <DallasTemperature.h>
#include <BlynkSimpleEthernet.h>
#include <DFRobotDFPlayerMini.h>
DFRobotDFPlayerMini player;

// Set-up Pin (Gunakan define alih-alih const!)
#define RX_DF 17
#define TX_DF 16
#define BUTTON_PIN 15
#define TEMP_PIN 14

// Set-Up Blynk
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID           "TMPL6qNAr_Dl-"
#define BLYNK_TEMPLATE_NAME         "Sipitung Blynk"
#define BLYNK_AUTH_TOKEN            "2Z0UmnxLQr3dRouPx6HggDjWu9LEonTM"

// inisialisasi Libraries
SoftwareSerial dfPlayer(TX_DF, RX_DF); // Inisialisasi DFPlayer
DFRobotDFPlayerMini myDFPlayer;
OneWire oneWire(TEMP_PIN); // Inisialisasi DS18B20
DallasTemperature suhu(&oneWire);

// Inisialisasi Variable
const int interval = 1000;        // Interval dalam milidetik (misalnya, 1000ms = 1 detik)
unsigned long previousMillis = 0;  // Waktu sebelumnya yang disimpan
bool pressed = false; 

// ============ FUNGSI / HELPER ============
void connectWiFi(){
   WiFiManager wm;

    bool res;
    res = wm.autoConnect("Sipitung","selaluaman"); 
    
    if(!res) {
        Serial.println("Gagal Terhubung");
    } 
    else { 
        Serial.println("Terkoneksi");
    }
}

void init_df(){
  dfPlayer.begin(9600);
  Serial.println(F("Menyiapkan DFPlayer ... (3~5 Detik)"));
  player.begin(dfPlayer);

    if (player.begin(dfPlayer)) {
        Serial.println(F("DFMini Player Terhubung!"));
    } else {
        Serial.println(F("DFMini Player Gagal Terhubung!"));
    }
}

void init_serial(){
  Serial.begin(115200);
}

void init_suhu(){
  suhu.begin();
}

void cek_button() {
  bool buttonState = digitalRead(BUTTON_PIN);

  // Jika Tombol ditekan
  if(buttonState == pressed){
    player.begin(dfPlayer);
    
    if (player.begin(dfPlayer)) {
        player.volume(30);
        player.play(1);
        Serial.println(F("Voice Note 1 Diputar!"));
    } else {
        Serial.println(F("DFMini Player Gagal Diputar!"));
    }
  }
}

float getSuhu() {
  Serial.print("Suhu Saat ini: ");
  suhu.requestTemperatures(); // Send the command to get temperatures
  return suhu.getTempCByIndex(0);
}

void sendToServer(){ 
  
}

// Function Untuk Mendapatkan Value VirtualPin
void getVirtualValue(){
  
}

// ============= PROGRAM UTAMA ============

void setup() {
  // PROGRAM SEKALI JALAN
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  Blynk.begin(BLYNK_AUTH_TOKEN);
   
  init_serial();
  connectWiFi();
  init_df();
  init_suhu();
}

void loop() {
  Blynk.run();
  
  unsigned long currentMillis = millis();  // Waktu saat ini
  
  // Periksa apakah sudah waktunya untuk melakukan sesuatu berdasarkan interval
  if (currentMillis - previousMillis >= interval) {
    // Simpan waktu terbaru
    previousMillis = currentMillis;

    // Kode yang ingin Anda jalankan setiap interval tertentu
    Serial.println(getSuhu());
  } else {
    cek_button();
  }
}


/* Projek ini menggunakan ESP32 Devkit V3 (Huge App Mode) Sebagai Mikrokontroller
 * Ukuran Program : 783.381 / 3.145.728 bytes (24%)
 * Penggunaan Variable Global : 38.648 / 327.680 bytes (11%)
 * Terakhir diupdate : - (Sedang sering diutak atik hehe)
 *
 * Kode ini digunakan untuk:
 * Inovasi Sistem Keamanan Desa Berbasis Internet Of Things
 * Coded by SIPITUNG
 * Copyright 2024
*/
