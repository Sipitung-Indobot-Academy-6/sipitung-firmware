// Inisialisasi Libraries(Gunakan depedency yang diperlukan)
#include <WiFi.h>
#include <OneWire.h>
#include <HTTPClient.h>
#include <WiFiManager.h> 
#include <SoftwareSerial.h>
#include <DallasTemperature.h>
#include <BlynkSimpleEthernet.h>
#include <DFRobotDFPlayerMini.h>
DFRobotDFPlayerMini player;
WiFiClient client;

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

String sendToServer(){ 
    // Inisialisasi 
    HTTPClient http;
    String postData, pelanggan, suhu, kelembapan, link, res;

    suhu = String(getSuhu());
    kelembapan = String(0);
    pelanggan = "B";

    // Pengiriman Data via POST Method
    postData = "&status1=" + suhu + "&status2=" + kelembapan + "&pelanggan=" + pelanggan;
    link = "http://panicbutton.my.id/view/suhu/konek.php";

    http.begin(client, link);                                            // Specify request destination
    http.addHeader("Content-Type", "application/x-www-form-urlencoded"); // Specify content-type header

    int httpCode = http.POST(postData); // Kirim Request sekaligus mendapatkan Res Code
    String payload = http.getString();  // Mendapatkan Catatan Respons

    // Memastikan Hasil Response 
    if (httpCode == 200) {
        res = "Data Berhasil Dikirim!";
    } else {
        res = "Data Gagal Terkirim! Error log : \n";
        res += "[";
        res += String(httpCode);
        res += "] ";
        res += payload;
    }
    
    http.end(); // Tutup Koneksi

    return res;
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
  // Jalankan program Blynk
  Blynk.run();

  // Jalankan program Button (Mode Standby)
  cek_button();

  // Setting Waktu Saat ini
  unsigned long currentMillis = millis();
  
  // Periksa apakah sudah waktunya untuk melakukan sesuatu berdasarkan interval
  if (currentMillis - previousMillis >= interval) {
    // Simpan waktu terbaru
    previousMillis = currentMillis;

    // Kirim ke Server
    sendToServer();
  } 
}


/* Projek ini menggunakan ESP32 Devkit V3 (Huge App Mode) Sebagai Mikrokontroller
 * Ukuran Program : 814.693 / 3.145.728 bytes (25%)
 * Penggunaan Variable Global : 39.332 / 327.680 bytes (12%)
 * Terakhir diupdate : 5 Mei 2024 14:44 WIB (Sedang sering diutak atik hehe)
 *
 * Kode ini digunakan untuk:
 * Sistem Panic Button untuk Kampung (SIPITUNG) : Inovasi Sistem Keamanan Desa Berbasis Internet Of Things
 * Coded by SIPITUNG TEAM
 * Copyright 2024
*/
