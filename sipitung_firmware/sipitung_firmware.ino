// Inisialisasi Libraries(Gunakan depedency yang diperlukan)
#include <WiFi.h>
#include <OneWire.h>
#include <WiFiManager.h> 
#include <SoftwareSerial.h>
#include <DallasTemperature.h>
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

// Set-up Pin (Gunakan define alih-alih const!)
#define RX_DF 25
#define TX_DF 26
#define BUTTON_PIN 27
#define TEMP_PIN 32

// inisialisasi Libraries
SoftwareSerial DFPlayerSerial(RX_DF, TX_DF); // Inisialisasi DFPlayer
DFRobotDFPlayerMini myDFPlayer;
OneWire oneWire(TEMP_PIN); // Inisialisasi DS18B20
DallasTemperature suhu(&oneWire);

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
  DFPlayerSerial.begin(9600);

  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  if (!myDFPlayer.begin(DFPlayerSerial)) {  // Komunikasi dengan DFPlayer
    Serial.println(F("DFPlayer Gagal Terhubung!"));
    while(true){
      delay(0);
    }
  }
  
}

void init_serial(){
  Serial.begin(115200);
}

void init_suhu(){
  suhu.begin();
}

float getSuhu() {
  Serial.print("Requesting temperatures...");
  suhu.requestTemperatures(); // Send the command to get temperatures
  return suhu.getTempCByIndex(0);
}

void sendToServer(){ 
  
}

// ============= PROGRAM UTAMA ============

void setup() {
  // PROGRAM SEKALI JALAN
  init_serial();
  connectWiFi();
  init_df();
  init_suhu();
}

void loop() {
  // PROGRAM YANG BERULANG SELAMANYA (GUNAKAN FUNCTION)
  getSuhu();
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
