/*#include "esp_camera.h"
#include "Arduino.h"
#include "soc/soc.h"           // Disable brownour problems
#include "soc/rtc_cntl_reg.h"  // Disable brownour problems
#include "driver/rtc_io.h"
#include "SerialTransfer.h"



// uint8_t 256 valores inteiros: de 0 a 2^8 (255)
// uint16_t 256 valores inteiros: de 0 a 2^16 (65.536)
// uint32_t 256 valores inteiros: de 0 a 2^32 (4.294.967.296)
// uint64_t 256 valores inteiros: de 0 a 2^64 (18.446.744.073.709.551.616)

sizeof(val) retorna o numero de bytes para armazenar "val"

MAX_PACKET_SIZE = 0xFE;



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
HardwareSerial Comm(1);

// METADADOS DOS PACOTES E DA  IMAGEM ENVIADOS JUNTOS
struct img_meta_data{
  uint16_t counter;
  uint16_t imSize;
  uint16_t numLoops;
  uint16_t sizeLastLoop;
} ImgMetaData;

// TAMANHO DO PACOTE = BYTES DA IMAGEM + METADADOS
const uint16_t PIXELS_PER_PACKET = MAX_PACKET_SIZE - sizeof(ImgMetaData);

void setup(){

  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector

  Serial.begin(115200);                     // Define and start serial monitor
  // 115200,256000,512000,962100
  Comm.begin(962100, SERIAL_8N1,15,14);     //, Comm_Txd_pin, Comm_Rxd_pin); // Define and start Comm serial port
  myTransfer.begin(Comm);

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

  //
  uint16_t startIndex = 0;
  camera_fb_t * fb = NULL;

  // TIRA UMA FOTO
  fb = esp_camera_fb_get();

  // CONTADOR PARA O LOOP DE ENVIOS DE PACOTES
  // ImgMetaData.counter TEM O MESMO VALOR QUE ImgMetaData.numLoops
  // TAMANHO/COMPRIMENTO DA IMAGEM
  ImgMetaData.imSize       = fb->len; //sizeof(myFb);
  // NUMERO DE PACOTES SUFICIENTES PARA TRANSMITIR A IMAGEM
  ImgMetaData.numLoops     = (fb->len / PIXELS_PER_PACKET) + 1; //(sizeof(myFb)/PIXELS_PER_PACKET) + 1;
  // PIXELS RESTANTES A SEREM ENVIADOS NO ULTIMO PACOTE
  ImgMetaData.sizeLastLoop = fb->len % PIXELS_PER_PACKET; //(sizeof(myFb)%PIXELS_PER_PACKET);

  // TRANSMITE A IMAGEM EM PACOTES
  for(ImgMetaData.counter = 1; ImgMetaData.counter <= ImgMetaData.numLoops; ImgMetaData.counter++){

    // INSERE OS METADADOS DA IMAGEM EM TXBUFF (SE ImgMetaData UM TAMANHO DE 5 BYTES, INSERE ESTES 5 BYTES)
    myTransfer.txObj(ImgMetaData, sizeof(ImgMetaData));
    //printStructBuf();

    // INSERE OS PIXELS DA IMAGEM EM TXBUFF
    stuffPixels(fb->buf, startIndex, sizeof(ImgMetaData), PIXELS_PER_PACKET);
    //stuffPixels(myFb, startIndex, sizeof(ImgMetaData), PIXELS_PER_PACKET);
    //printStructBuf();

    // ENVIA, DE FATO, O PACOTE DE DADOS (PREÂMBULO + TXBUFF + PÓS-ÂMBULO)
    myTransfer.sendData(MAX_PACKET_SIZE);

    // X << Y:  SHIFTA PARA A ESQUERDA Y BITS DE X
       // NO CASO myTransfer.txBuff[1] << 8: SHIFTA 8 BITS PARA A ESQUERDA DE myTransfer.txBuff[1]

       // | performs a bitwise OR on the two operands it is passed.
       // byte b = 0x0A | 0x50
       // b é igual a 0x5A
    

    // PRINTA ANATOMIA DO PACOTE:
    pDBGln(F("Sent:"));
    pDBG(F("img.counter: "));      pDBGln((uint16_t)((myTransfer.txBuff[1] << 8) | myTransfer.txBuff[0]));
    pDBG(F("img.imSize: "));       pDBGln((uint16_t)((myTransfer.txBuff[3] << 8) | myTransfer.txBuff[2]));
    pDBG(F("img.numLoops: "));     pDBGln((uint16_t)((myTransfer.txBuff[5] << 8) | myTransfer.txBuff[4]));
    pDBG(F("img.sizeLastLoop: ")); pDBGln((uint16_t)((myTransfer.txBuff[7] << 8) | myTransfer.txBuff[6]));

    startIndex += PIXELS_PER_PACKET;
    delay(100);
    //printBuf();
  }

  // LIMPA O BUFFER DE FRAME DA CAMERA
  esp_camera_fb_return(fb);   //clear camera memory
  delay(10000);

}

// F() PRINTA OS DADOS SEM SALVÁ-LOS TEMPORARIAMENTE NA RAM, USA A MEMÓRIA FLASH PARA ISSO
// https://www.arduino.cc/reference/en/language/variables/utilities/progmem/


// ESTRUTURA DE TXBUFF

// 0            7 8              254
// | ImgMetaData | PIXELS DA IMAGEM |
//     8 BYTES        246 BYTES


void printStructBuf(){

  pDBG(F("Internal Struct: { "));

  // PRINTA OS 7 PRIMEIROS BYTES (BYTES REFERENTES AOS METADADOS)
  for (uint16_t k = 0; k < sizeof(ImgMetaData); k++){
    pDBG(myTransfer.txBuff[k]);
    if (k < (sizeof(ImgMetaData) - 1))
      pDBG(F(", "));
    else
      pDBGln(F(" }"));
  }

}

//
void printBuf(){

  pDBG(F("Pixel Values: { "));

  // PRINTA OS BYTES RESTANTES A PARTIR DO 8o BYTE (BYTES REFERENTES AOS PIXELS DA IMAGEM)
  for (uint16_t k = 8; k < MAX_PACKET_SIZE; k++){
    pDBG(myTransfer.txBuff[k]);
    if (k < (MAX_PACKET_SIZE - 1))
      pDBG(F(", "));
    else
      pDBGln(F(" }"));
  }
  pDBGln();

}

// PREENCHE O TXBUFF COM OS PIXELS DA IMAGEM
void stuffPixels(const uint8_t * pixelBuff, const uint16_t &bufStartIndex, const uint16_t &txStartIndex, const uint16_t &len){

  // CHAMADA:
     // stuffPixels(fb->buf, startIndex, sizeof(ImgMetaData), PIXELS_PER_PACKET);

     // pixelBuff -> fb->buf
     // bufStartIndex -> startIndex (INDEX DO FRAME BUFFER fb->buf DA CÂMERA)
     // txStartIndex -> sizeof(ImgMetaData)
     // len -> PIXELS_PER_PACKET
  

  uint16_t txi = txStartIndex;

  for (uint16_t i = bufStartIndex; i < (bufStartIndex + len); i++)  {
    myTransfer.txBuff[txi] = pixelBuff[i];
    txi++;
  }

}*/