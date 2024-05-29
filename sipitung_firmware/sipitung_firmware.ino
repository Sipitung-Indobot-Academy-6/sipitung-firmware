#include <WiFi.h>
#include <OneWire.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiManager.h> 
#include <SoftwareSerial.h>
#include <DallasTemperature.h>
#include <DFRobotDFPlayerMini.h>

DFRobotDFPlayerMini player;

// Set-up Pin (Gunakan define alih-alih const!)
#define RX_DF 17
#define TX_DF 16
#define BUTTON_PIN 15
#define TEMP_PIN 4

// inisialisasi Libraries
SoftwareSerial dfPlayer(TX_DF, RX_DF); // Inisialisasi DFPlayer
OneWire oneWire(TEMP_PIN); // Inisialisasi DS18B20
DallasTemperature suhu(&oneWire);

// Inisialisasi Variable 
const short tempInterval PROGMEM = 500;        // Interval dalam milidetik (misalnya, 1000ms = 1 detik)
const short buttonInterval PROGMEM = 1000;       // Interval dalam milidetik (misalnya, 1000ms = 1 detik)
unsigned long tempPrevMillis PROGMEM = 0;       // Waktu sebelumnya yang disimpan
unsigned long buttonPrevMillis PROGMEM = 0;       // Waktu sebelumnya yang disimpan
bool pressed = false; 

WiFiClientSecure client;  // Gunakan WiFiClientSecure untuk HTTPS

// ============ FUNGSI / HELPER ============
void connectWiFi(){
    WiFiManager wm;

    bool res;
    res = wm.autoConnect("SIPITUNG (#selaluaman)","selaluaman"); 
    
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

void play_record(int lagu){
//    player.begin(dfPlayer);

    if (player.begin(dfPlayer)) {
        switch(lagu) {
            case 0:
                player.volume(30);
                player.play(2);
              break;
            case 1:
                player.volume(30);
                player.play(1);
              break;
            case 2:
                player.volume(30);
                player.play(4);
              break;
            case 3:
                player.volume(30);
                player.play(3);
              break;
            default:
                player.volume(30);
                player.play(5);
        }
    } else {
        Serial.println(F("DFMini Player Gagal Diputar!"));
    }
}

void cek_button() {
    bool buttonState = digitalRead(BUTTON_PIN);

    // Jika Tombol ditekan
    if(buttonState == pressed){
        Serial.println("Button Fisik ditekan!");
        play_record(4);
    }
}

// Pengambilan data dari sensor
float getSuhu() {
    suhu.requestTemperatures(); // Send the command to get temperatures
    //  Serial.print("Suhu Saat ini: ");
    return suhu.getTempCByIndex(0);
}

// Pengiriman data suhu (POST Method)
String sendToServer(){ 
    // Inisialisasi 
    HTTPClient http;
    String postData, pelanggan, suhu, kelembapan, link, res;

    suhu = String(getSuhu());
    kelembapan = String(1);
    pelanggan = "B";

    // Pengiriman Data via POST Method
    postData = "&status1=" + suhu + "&status2=" + kelembapan + "&pelanggan=" + pelanggan;
    link = "https://panicbutton.my.id/view/suhu/konek.php";  // Gunakan HTTPS

    client.setInsecure();  // Set client untuk mode insecure (tanpa sertifikat)
    http.begin(client, link);  // Specify request destination
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


// Update Bagian button untuk reset ke 0
String updateButton(String wil){ 
    // Inisialisasi 
    HTTPClient http;
    String postData, link, res;

    // Pengiriman Data via POST Method
    postData = "&wilayah=" + wil ;
    link = "https://panicbutton.my.id/view/suhu/api/update-button.php";  // Gunakan HTTPS

    client.setInsecure();  // Set client untuk mode insecure (tanpa sertifikat)
    http.begin(client, link);  // Specify request destination
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
void getToServer(){
  HTTPClient http;
  http.begin("https://panicbutton.my.id/view/suhu/konek.php");
  int httpResponseCode = http.GET();
    Serial.print("Response Code: ");
    Serial.println(httpResponseCode);

    if (httpResponseCode == 200) {
      char input[500];
      String payload = http.getString();
      payload.toCharArray(input, 500);
      StaticJsonDocument<192> doc;
      DeserializationError error = deserializeJson(doc, input);

      if (error) {
        Serial.print("Decoder JSON Gagal karena : ");
        Serial.println(error.c_str());
        return;
      }

      //Serial.println(payload);

        String arah[] = {"utara", "selatan", "timur", "barat"};
        String nilai[] = {doc["Utara"], doc["Selatan"], doc["Timur"], doc["Barat"]};

        for (int i = 0; i < 4; i++) {
            if (nilai[i] != "0") {
                switch (i) {
                    case 0 :
                        updateButton("utara");
                        play_record(0);
                      break;
                    case 1:
                        updateButton("selatan");
                        play_record(1);
                      break;
                    case 2:
                        updateButton("timur");
                        play_record(2);
                      break;
                    case 3:
                        updateButton("barat");
                        play_record(3);
                      break;
                    default:
                      play_record(4);
                }
                  
                  // Debug Mode
                  Serial.print(arah[i]);
                  Serial.print(": ");
                  Serial.println(nilai[i]);
            }
        }
    }
    else {
      Serial.print("Permintaan Request Gagal!");
      Serial.println(http.errorToString(httpResponseCode));
      Serial.println("");
    }
}

// ============= PROGRAM UTAMA ============

void setup() {
    // PROGRAM SEKALI JALAN
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    init_serial();
    connectWiFi();
    init_df();
    init_suhu();
}

void loop() {
    // Membiarkan program lain berjalan terlebih dahulu ketika terjadi crash
    yield(); 

    // Setting Waktu Saat ini
    unsigned long currentMillis PROGMEM = millis();
  
    // MILLIS SEND TEMP DATA
    if (currentMillis - tempPrevMillis >= tempInterval) {
        // Jalankan program Button (Mode Standby)
        cek_button();
        
        // Simpan waktu terbaru
        tempPrevMillis = currentMillis;

        // Kirim ke Server (Debug Mode)
       Serial.println(sendToServer()); 
       getToServer(); 
    } else {
      
    }

    // MILLIS GET BUTTON DATA
    // } else if (currentMillis - buttonPrevMillis >= buttonInterval) {
    //     // Simpan waktu terbaru
    //     buttonPrevMillis = currentMillis;

    //     // Kirim ke Server (Debug Mode)
    //      getToServer(); 
    // } 
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
