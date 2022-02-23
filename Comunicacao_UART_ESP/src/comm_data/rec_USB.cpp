/*#include <Arduino.h>
#include <HardwareSerial.h>

// PARAR CÓDIGO DE ENVIO PARA UPAR ESTE NO ESP32

// CONEXAO

//RPI3B <---> ESP32
//TX(14) <--> RX2(16) AZUL
//RX(15) <--> TX2(17) CINZA
//GND(6) <--> GND MARROM
//VCC LARANJA

// Declara as variáveis que receberam os dados de temperatura, umidade e
// buffer de dados da serial

//HardwareSerial Serial2();

char data;
char buff[10];

String val = "";

int len = 0;
int i;

bool rec = false;

void setup() {

  Serial.begin(9600); // Porta Serial monitor
  Serial2.begin(9600); // Porta Serial comunicacao 13 RX 12 TX | | 

  delay(100); // não receber lixo via UART

}

void loop() {

  if (Serial2.available() > 0) {

    data = char(Serial2.read()); // isAlphaNumeric() não printa os pontos, obviamente

    if (data == '@' || rec) { 

      buff[len] = data;
      //Serial.println("Recebendo");
      //Serial.println(buff[len]);
      rec = true;
      len++;

    }
    if (data == '#') {

      //Serial.println("Recebido");
      rec = false;

    }

  }

  if (len > 0 && !rec) {

    buff[0] = ' ';
    buff[len] = ' ';

    for (i=1; i<len-1; i++) {

      //Serial.print(buff[i]);
      val += buff[i];
      buff[i] = ' ';
    }

    Serial.print("Val: ");
    Serial.print(val);
    Serial.print(" len: ");
    Serial.println(len-2);

    len = 0;
    val = "";

  }

}*/