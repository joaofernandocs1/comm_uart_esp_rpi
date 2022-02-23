/*#include <Arduino.h>
#include <HardwareSerial.h>

// PARAR CÓDIGO DE ENVIO PARA UPAR ESTE NO ESP32

// CONEXAO

//RPi3B <---> ESP32

//PIN 8 (TXD) <--> GPIO 16 (RX2) AZUL
//PIN 10 (RXD)<--> GPIO 17 (TX2) CINZA
//PIN 16      <--> GPIO 4        LARANJA
//GND(6)      <--> GND           MARROM
//VCC (VIA USB)


int i;

float num = 2037.146;

char start = '@';
char end = '#';

String data_num = "89136403";
String data_char = "%r&un!ao";

void setup() {

  Serial.begin(115200); // Porta Serial monitor
  Serial2.begin(115200); // Porta Serial comunicacao 13 RX 12 TX | | 

  pinMode(4, OUTPUT);

  delay(100); // não receber lixo via UART

}

void loop () {

  // ENVIAR DADOS VIA JSON????? AVALIAR!!!!

  int len = data_num.length();
  char buff[len+1];

  strcpy(buff, data_char.c_str());

  //ciclo de escrita
  digitalWrite(4, HIGH);
  Serial.println("INICIA UART");
  //delay(50); // esperar possível loop do RPi terminar para ler o sinal de escrita

  Serial2.println(start);
  //Serial.println(start);

  for (i=0; i<=7; i++) {

    Serial2.println(char(buff[i]));
    //Serial.println(char(buff[i]));

  }

  Serial2.println(end);
  //Serial.println(end);
  //delay(50); // terminar o ciclo de escrita

  digitalWrite(4, LOW);
  Serial.println("TERMINA UART");

  delay(100);

}*/