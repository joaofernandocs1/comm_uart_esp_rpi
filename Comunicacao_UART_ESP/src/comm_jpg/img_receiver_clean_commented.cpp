/*Receives picture via serial and saves to ftp*/

/*#include "SerialTransfer.h"
SerialTransfer myTransfer;

struct img_meta_data{
  uint16_t counter;
  uint16_t imSize;
  uint16_t numLoops;
  uint16_t sizeLastLoop;
} ImgMetaData;

uint16_t packetCounter = 1;
uint16_t bufferPointer = 0;
char tempImageBuffer[32000];

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;

// picture name
String picPrefix = "";
String pic_name;

// Connection timeout;
#define CON_TIMEOUT   10*1000                     // milliseconds

void setup(){

  //Serial.begin(Baud Rate, Data Protocol, Txd pin, Rxd pin);
  Serial.begin(115200);                                                     // Define and start serial monitor
  // 115200,256000,512000,962100
  Serial2.begin(962100, SERIAL_8N1); //Receiver_Txd_pin, Receiver_Rxd_pin); // Define and start Receiver serial port
  myTransfer.begin(Serial2);

}

void loop(){

  if(myTransfer.available())  {

    // LÊ OS BYTES DE METADADOS E PÕE NO BUFFER DE RECEPÇÃO
    myTransfer.rxObj(ImgMetaData, sizeof(ImgMetaData));

    pDBG("Struct Data: ");
    pDBG(ImgMetaData.counter);
    pDBG(", ");
    pDBG(ImgMetaData.imSize);
    pDBG(", ");
    pDBG(ImgMetaData.numLoops);
    pDBG(", ");
    pDBG(ImgMetaData.sizeLastLoop);
    pDBG(", PacketCounter: ");
    pDBGln(packetCounter);

    // SE FOR O PRIMEIRO PACOTE
    if(ImgMetaData.counter == 1){

      //
      copyToImageBuff(MAX_PACKET_SIZE-sizeof(ImgMetaData));

    }else{

      // ENTRA AQUI A PARTIR DO SEGUNDO PACOTE
      if(ImgMetaData.counter == packetCounter){

        if(packetCounter<ImgMetaData.numLoops){

          copyToImageBuff(MAX_PACKET_SIZE - sizeof(ImgMetaData));

        // RECEBE O ÚLTIMO PACOTE
        }else if(ImgMetaData.counter == packetCounter){

          copyToImageBuff(ImgMetaData.sizeLastLoop);
        }
      }
    }

    // FINALIZAÇÃO DO RECEBIMENTO
    if(packetCounter > ImgMetaData.numLoops){

      // NOME DA IMAGEM É GERADO A PARTIR DE METADADOS DO CLIENTE NTP
      pic_name  = picPrefix;
      pic_name += getDateTime() + ".jpg";
      FTP_upload();
      packetCounter = 1;
      bufferPointer = 0;
      delay(2000);
      //while(1){}
    }

  }else if(myTransfer.status < 0) {

    pDBG("ERROR: ");
    if(myTransfer.status == -1)

      pDBGln(F("CRC_ERROR"));

    else if(myTransfer.status == -2)

      pDBGln(F("PAYLOAD_ERROR"));

    else if(myTransfer.status == -3)

      pDBGln(F("STOP_BYTE_ERROR"));

  }
}


void copyToImageBuff(uint16_t dataLenght){

  // uint16_t packetCounter = 1;
  // uint16_t bufferPointer = 0;
  // char tempImageBuffer[32000];


  // PREENCHE O BUFFER DE RECEPÇÃO COM PAYLOAD APENAS DE PIXELS...
  for(int y = 0; y < dataLenght; y++){

    tempImageBuffer[bufferPointer + y] = myTransfer.rxBuff[y + sizeof(ImgMetaData)];

  }

  // ... ATÉ QUE A IMAGEM SEJA TODA TRANSFERIDA (bufferPointer == 32000)
  bufferPointer += dataLenght;
  packetCounter++;

  pDBG("dataLenght: ");
  pDBG(dataLenght);
  pDBG(", bufferPointer: ");
  pDBGln(bufferPointer);

}

void printBuf(char localBuff){

  pDBG(F("Pixel Values: { "));
  for (uint16_t k = 0; k < sizeof(myTransfer.rxBuff); k++){

    pDBG(myTransfer.rxBuff[k]);

    if (k < (sizeof(myTransfer.rxBuff) - 1))

      pDBG(F(", "));

    else

      pDBGln(F(" }"));

  }
}

String getDateTime(){

  String curDateTime = "";

  while(!timeClient.update()) {

    timeClient.forceUpdate();
  }
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();

  #ifdef DEGUB_ESP
    // Extract date
    int splitT = formattedDate.indexOf("T");
    dayStamp = formattedDate.substring(0, splitT);
    // Extract time
    timeStamp = formattedDate.substring(splitT + 1, formattedDate.length() - 1);

    pDBGln(formattedDate);
    pDBG("DATE: ");
    pDBGln(dayStamp);
    pDBG("HOUR: ");
    pDBGln(timeStamp);
  #endif

  curDateTime  = formattedDate.substring(8,10);  //day
  curDateTime += formattedDate.substring(5,7);   //month
  curDateTime += formattedDate.substring(2,4);   //year
  curDateTime += formattedDate.substring(11,13); //hour
  curDateTime += formattedDate.substring(14,16); //minute
  curDateTime += formattedDate.substring(17,19); //seconds
  return curDateTime;

}*/