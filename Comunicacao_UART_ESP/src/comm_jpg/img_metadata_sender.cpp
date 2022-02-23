/*#include <esp_camera.h>
#include <Arduino.h>
#include <soc/soc.h>           // Disable brownour problems
#include <soc/rtc_cntl_reg.h>  // Disable brownour problems
#include <driver/rtc_io.h>
#include <SoftwareSerial.h>
#include <SerialTransfer.h>
#include <SD_MMC.h>
//#include <Packet.h>


// uint8_t 256 valores inteiros: de 0 a 2^8 (255)
// uint16_t 256 valores inteiros: de 0 a 2^16 (65.536)
// uint32_t 256 valores inteiros: de 0 a 2^32 (4.294.967.296)
// uint64_t 256 valores inteiros: de 0 a 2^64 (18.446.744.073.709.551.616)

// sizeof(val) retorna o numero de bytes para armazenar "val"

// MAX_PACKET_SIZE = 0xFE;



// Pin definition for CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

SerialTransfer myTransfer;

SoftwareSerial UART;
SoftwareSerialConfig swconfig = SWSERIAL_8N1;

// METADADOS DOS PACOTES E DA  IMAGEM ENVIADOS JUNTOS
struct img_meta_data{
  uint16_t counter;
  uint16_t imSize;
  uint16_t numLoops;
  uint16_t sizeLastLoop;
} ImgMetaData;

bool init_card = false;

// TAMANHO DO PACOTE = BYTES DA IMAGEM + METADADOS
const uint16_t PIXELS_PER_PACKET = MAX_PACKET_SIZE - sizeof(ImgMetaData);

uint16_t buffIndex = 0;

void printStructBuf(uint16_t len){

  Serial.print("Internal Struct: { ");

  // PRINTA OS 7 PRIMEIROS BYTES (BYTES REFERENTES AOS METADADOS)
  // for (uint16_t k = 0; k < sizeof(ImgMetaData); k++){
  for (uint16_t k = 0; k < len; k++){
    
    // Serial.print(myTransfer.txBuff[k]); // erro: "SerialTransfer has no member named txBuff"
    Serial.print(myTransfer.packet.txBuff[k]);
    
  }

  Serial.println(" }");

}

void setup(){

  Serial.begin(115200);                     // Define and start serial monitor

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

  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector

  // 115200, 256000, 512000, 962100
  UART.begin(115200, swconfig, 16, 13); // Porta Serial comunicacao 13 RX 12 TX | | 
  myTransfer.begin(UART);

  // CONFIGURAÇÕES DA CÂMERA
  camera_config_t config;

  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

//  if(psramFound()){
//    config.frame_size = FRAMESIZE_UXGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
//    config.jpeg_quality = 10;
//    config.fb_count = 2;
//  } else {
//    config.frame_size = FRAMESIZE_SVGA;
//    config.jpeg_quality = 12;
//    config.fb_count = 1;
//  }

    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;

  // INICIA A CAMERA
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK){
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
}

void loop(){

  camera_fb_t * fb = NULL;

  // TIRA UMA FOTO
  fb = esp_camera_fb_get();

  // CONTADOR PARA O LOOP DE ENVIOS DE PACOTES
  // ImgMetaData.counter TEM O MESMO VALOR QUE ImgMetaData.numLoops
  // TAMANHO/COMPRIMENTO DA IMAGEM
  ImgMetaData.imSize       = fb->len; //sizeof(myFb);
  Serial.print("imSize: ");
  Serial.print(ImgMetaData.imSize);
  Serial.print(" ");
  // NUMERO DE PACOTES SUFICIENTES PARA TRANSMITIR A IMAGEM
  ImgMetaData.numLoops     = (fb->len / PIXELS_PER_PACKET) + 1; //(sizeof(myFb)/PIXELS_PER_PACKET) + 1;
  Serial.print("numLoops: ");
  Serial.print(ImgMetaData.numLoops);
  Serial.print(" ");
  // PIXELS RESTANTES A SEREM ENVIADOS NO ULTIMO PACOTE
  ImgMetaData.sizeLastLoop = fb->len % PIXELS_PER_PACKET; //(sizeof(myFb)%PIXELS_PER_PACKET);
  Serial.print("sizeLastLoop: ");
  Serial.print(ImgMetaData.sizeLastLoop);
  Serial.println(" ");

  // INSERE OS METADADOS DA IMAGEM EM TXBUFF (SE ImgMetaData UM TAMANHO DE 5 BYTES, INSERE ESTES 5 BYTES)
  // myTransfer.txObj(ImgMetaData, sizeof(ImgMetaData)); // NÃO ESTÁ EMPACOTANDO OS DADOS EM TXBUFF
  // myTransfer.packet.txObj(ImgMetaData, sizeof(ImgMetaData)); // dados indo errados com zeros no meio
  
  myTransfer.packet.txObj(ImgMetaData.imSize, buffIndex, sizeof(ImgMetaData.imSize));
  buffIndex += sizeof(ImgMetaData.imSize) - 1;

  myTransfer.packet.txObj(ImgMetaData.numLoops, buffIndex, sizeof(ImgMetaData.numLoops));
  buffIndex += sizeof(ImgMetaData.numLoops) - 1;

  myTransfer.packet.txObj(ImgMetaData.sizeLastLoop, buffIndex, sizeof(ImgMetaData.sizeLastLoop));
  buffIndex += sizeof(ImgMetaData.sizeLastLoop) - 1;

  printStructBuf(buffIndex);

  // ENVIA, DE FATO, O PACOTE DE DADOS (PREÂMBULO + TXBUFF + PÓS-ÂMBULO)
  // myTransfer.sendData(MAX_PACKET_SIZE);
  myTransfer.sendData(sizeof(ImgMetaData));

  delay(100);
  //printBuf();

  // LIMPA O BUFFER DE FRAME DA CAMERA
  esp_camera_fb_return(fb);   //clear camera memory
  delay(3000);

  buffIndex = 0;

}*/
