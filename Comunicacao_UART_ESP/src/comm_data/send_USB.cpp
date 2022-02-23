/*#include <Arduino.h>
#include <SD_MMC.h>

#define RX_PIN 13
#define TX_PIN 12

// RETIRAR SOFTWARE SERIAL DESTE CÓDIGO

// CONEXAO

//RPi3B <---> ESP32

//PIN 8 (TXD) <--> GPIO 16 (RX2) AZUL
//PIN 10 (RXD)<--> GPIO 17 (TX2) CINZA
//PIN 16      <--> GPIO 4        LARANJA
//GND(6)      <--> GND           MARROM
//VCC (VIA USB)

bool init_card = false;

int i;

float num = 2037.146;

char start = '@';
char end = '#';

String data_num = "89136403";
String data_char = "%r&un!ao";

void setup() {

  init_card = SD_MMC.begin("/sdcard", true); // initialize SD Card in slow 1bit mode

  //Serial.println("Starting SD Card");
  //if( !SD_MMC.begin() ) {DBG_OUTPUT_PORT.println( "SD card init failed" );} // fast 4bit mode
  
  if (!init_card) { 
    Serial.println("SD Card Mount Failed");
  }
  
  uint8_t cardType = SD_MMC.cardType();
  
  if(cardType == CARD_NONE){
    Serial.println("No SD Card attached");
  }

  Serial.begin(115200); // Porta Serial monitor
  Serial2.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN); // Porta Serial comunicacao 13 RX 12 TX | | 

  delay(100); // não receber lixo via UART

}

void loop () {

  // ENVIAR DADOS VIA JSON????? AVALIAR!!!!

  int len = data_num.length();
  char buff[len+1];

  strcpy(buff, data_char.c_str());

  //ciclo de escrita
  Serial.println("INICIA UART");
  //delay(50); // esperar possível loop do RPi terminar para ler o sinal de escrita

  Serial.println(start);
  Serial2.println(start);

  for (i=0; i<=7; i++) {

    Serial.println(char(buff[i]));
    Serial2.println(char(buff[i]));

  }

  Serial.println(end);
  Serial2.println(end);
  //delay(50); // terminar o ciclo de escrita

  Serial.println("TERMINA UART");

  delay(100);

}*/