#include <esp_camera.h>
#include <Arduino.h>
#include <soc/soc.h>           // Disable brownour problems
#include <soc/rtc_cntl_reg.h>  // Disable brownour problems
#include <driver/rtc_io.h>
#include <SoftwareSerial.h>
// #include <Packet.h>
#include <SerialTransfer.h>
#include <SD_MMC.h>


// uint8_t 256 valores inteiros: de 0 a 2^8 (255)
// uint16_t 256 valores inteiros: de 0 a 2^16 (65.536)
// uint32_t 256 valores inteiros: de 0 a 2^32 (4.294.967.296)
// uint64_t 256 valores inteiros: de 0 a 2^64 (18.446.744.073.709.551.616)

// sizeof(val) retorna o numero de bytes para armazenar "val"

// MAX_PACKET_SIZE = 0xFE;

//typedef struct {
    //uint8_t * buf;              /*!< Pointer to the pixel data */ <<<--- cada pixel é um uint8_t (um byte)
    //size_t len;                 /*!< Length of the buffer in bytes */
    //size_t width;               /*!< Width of the buffer in pixels */
    //size_t height;              /*!< Height of the buffer in pixels */
    //pixformat_t format;         /*!< Format of the pixel data */
    //struct timeval timestamp;   /*!< Timestamp since boot of the first DMA buffer of the frame */
//} camera_fb_t;

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


// METADADOS DOS PACOTES E DA IMAGEM ENVIADOS JUNTOS

//struct img_meta_data{
  //uint16_t counter;
  //uint16_t imSize;
  //uint16_t numLoops;
  //uint16_t sizeLastLoop;
//} ImgMetaData;

uint16_t counter;
uint16_t imSize;
uint16_t numLoops;
uint16_t sizeLastLoop;

// TAMANHO DO PACOTE = BYTES DA IMAGEM + METADADOS
//const uint16_t PIXELS_PER_PACKET = MAX_PACKET_SIZE - sizeof(ImgMetaData);
const uint16_t PIXELS_PER_PACKET = MAX_PACKET_SIZE - sizeof(counter) - sizeof(imSize) - sizeof(numLoops) - sizeof(sizeLastLoop);
//             PIXELS_PER_PACKET = 254 - 2 - 2 - 2 - 2 = (246)


SerialTransfer myTransfer;

SoftwareSerial UART;
SoftwareSerialConfig swconfig = SWSERIAL_8N1;

// STATUS DE INICIALIZAÇÃO DO SD CARD
bool init_card = false;
// INDEX PARA COMEÇAR A PREENCHER TXBUFF COM METADADOS
uint16_t startIndex = 0;
// INDEX VARIÁVEL PARA PREENCHER TX_BUFF COM METADADOS
uint16_t lastIndex = 0;
// INDEX PARA PREENCHER TX_BUFF COM PIXELS
uint16_t b_StartIndex = 0;
// INDEX DA IMAGEM A SER SEGMENTADA EM TX_BUFF COM PIXELS
uint16_t i_StartIndex = 0;


void printStructBuf(uint16_t len){

  Serial.print("Internal Struct: { ");

  // PRINTA OS 7 PRIMEIROS BYTES (BYTES REFERENTES AOS METADADOS)
  // for (uint16_t k = 0; k < sizeof(ImgMetaData); k++){
  for (uint16_t k = 0; k < len-1; k++){
    
    // Serial.print(myTransfer.txBuff[k]); // erro: "SerialTransfer has no member named txBuff"
    //Serial.print(myTransfer.packet.txBuff[k]);
    
  }

  Serial.print(" }");
  Serial.println(" ");

}

void stuffPixels(const uint8_t * imgPixelBuff, const uint16_t &imgStartIndex, const uint16_t &buffStartIndex, const uint16_t &numPixels){

  // CHAMADAS:
     // stuffPixels(fb->buf, startIndex, b_StartIndex, PIXELS_PER_PACKET); -->> loops normais
     // stuffPixels(fb->buf, startIndex, b_StartIndex, sizeLastLoop); -->> último loop

     // imgPixelBuff       -> fb->buf
     // imgStartIndex      -> startIndex (INDEX DO FRAME BUFFER fb->buf DA CÂMERA)
     // buffStartIndex     -> sizeof(counter) + sizeof(imSize) + sizeof(numLoops) + sizeof(sizeLastLoop)
     // len                -> PIXELS_PER_PACKET
  

  uint16_t txi = buffStartIndex; // (lastIndex = 8)

  // BUFFER TEM 254 VALORES [0 a 253] // IMAGEM +- 35000 PIXELS
  // 0 a 7 = metadados
  // 8 a 253 = pixels                 // 0 a 34999

  for (uint16_t i = imgStartIndex; i < (imgStartIndex + numPixels); i++)  {
    myTransfer.packet.txBuff[txi] = imgPixelBuff[i];
    //Serial.print(" ");
    //Serial.print(imgPixelBuff[i]);
    //Serial.print(" ");
    txi++;
  }


}

void setup(){

  Serial.begin(9600);                     // Define and start serial monitor

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

  // 9600, 38400, 115200, 230400, 460800, 576000, 921600
  UART.begin(9600, swconfig, 16, 13); // Porta Serial comunicacao 13 RX 12 TX | | 
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

    config.frame_size = FRAMESIZE_SVGA; // 800 x 600 = 480.000 (4:3)
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

  // DEFINE UM PONTEIRO PARA A STRUCT camera_fb_t
  camera_fb_t * fb = NULL;

  // TIRA UMA FOTO
  fb = esp_camera_fb_get();

  // CONTADOR PARA O LOOP DE ENVIOS DE PACOTES
  // ImgMetaData.counter TEM O MESMO VALOR QUE ImgMetaData.numLoops
  // TAMANHO/COMPRIMENTO DA IMAGEM
  //ImgMetaData.imSize = fb->len; //sizeof(myFb);
  imSize = fb->len;

  //Serial.print("imSize: ");
  //Serial.print(imSize);
  //Serial.print(imSize);
  //Serial.print(" ");
  //Serial.print(sizeof(imSize));
  //Serial.print(" ");

  // NUMERO DE PACOTES SUFICIENTES PARA TRANSMITIR A IMAGEM
  numLoops = (fb->len / PIXELS_PER_PACKET) + 1; //(sizeof(myFb)/PIXELS_PER_PACKET) + 1;

  //Serial.print("numLoops: ");
  //Serial.print(numLoops);
  //Serial.print(" ");
  //Serial.print(sizeof(numLoops));
  //Serial.print(" ");

  // PIXELS RESTANTES A SEREM ENVIADOS NO ULTIMO PACOTE
  sizeLastLoop = fb->len % PIXELS_PER_PACKET; //(sizeof(myFb)%PIXELS_PER_PACKET);

  //Serial.print("sizeLastLoop: ");
  //Serial.print(sizeLastLoop);
  //Serial.println(" ");
  //Serial.println(" ");
  //Serial.print(sizeof(sizeLastLoop));
  //Serial.println(" ");

  for (counter = 1; counter <= numLoops; counter++) {

    // INSERE OS METADADOS DA IMAGEM EM TXBUFF (SE ImgMetaData UM TAMANHO DE 5 BYTES, INSERE ESTES 5 BYTES)
    // myTransfer.txObj(ImgMetaData, sizeof(ImgMetaData)); // NÃO ESTÁ EMPACOTANDO OS DADOS EM TXBUFF
    // myTransfer.packet.txObj(ImgMetaData, sizeof(ImgMetaData)); // dados indo errados com zeros no meio
    
    //Serial.print("lastIndex: ");
    //Serial.print(lastIndex); // 0
    //Serial.print(" ");
    lastIndex = myTransfer.packet.txObj(counter, startIndex, sizeof(counter));
    //Serial.print("counter: ");
    Serial.print("ct: ");
    Serial.print(counter);
    Serial.print("   ");
    //Serial.print("counter PACKED ");
    //Serial.print("lastIndex: ");
    //Serial.print(lastIndex); // 2
    //Serial.print(" ");
    //lastIndex += sizeof(counter);
    //Serial.print("lastIndex: ");
    //Serial.print(lastIndex);
    //Serial.print(" ");
    //buffIndex += sizeof(counter);

    //Serial.print("lastIndex: ");
    //Serial.print(lastIndex); 
    //Serial.print(" ");
    lastIndex = myTransfer.packet.txObj(imSize, lastIndex, sizeof(imSize));
    //Serial.print(imSize);
    //Serial.print(" ");
    //Serial.print("imSize PACKED ");
    //Serial.print("lastIndex: ");
    //Serial.print(lastIndex); // 4
    //Serial.print(" ");
    //lastIndex += sizeof(imSize);
    //Serial.print("lastIndex: ");
    //Serial.print(lastIndex);
    //Serial.print(" ");
    //buffIndex += sizeof(imSize);

    //Serial.print("lastIndex: ");
    //Serial.print(lastIndex);
    //Serial.print(" ");
    myTransfer.packet.txObj(numLoops, lastIndex, sizeof(numLoops));
    Serial.print("nL: ");
    Serial.println(numLoops);
    //Serial.print(" ");
    // A FUNÇÃO NÃO RETORNA A POSIÇÃO DE TXBUFF, RETORNA O TAMANHO DA VARIÁVEL A SER ARAMAZENADA
    //Serial.print("numLoops PACKED ");
    lastIndex += sizeof(numLoops);
    //Serial.print("lastIndex: ");
    //Serial.print(lastIndex); // 6
    //Serial.print(" ");
    //lastIndex += sizeof(numLoops);
    //Serial.print("lastIndex: ");
    //Serial.print(lastIndex);
    //Serial.println(" ");

    //buffIndex += sizeof(numLoops);

    myTransfer.packet.txObj(sizeLastLoop, lastIndex, sizeof(sizeLastLoop));
    //Serial.print(sizeLastLoop);
    //Serial.print(" ");
    //Serial.print("sizeLastLoop PACKED ");
    lastIndex += sizeof(sizeLastLoop);
    //Serial.print("lastIndex: ");
    //Serial.print(lastIndex); // 8
    //Serial.println(" ");

    // ENVIA, DE FATO, O PACOTE DE DADOS (PREÂMBULO + TXBUFF + PÓS-ÂMBULO)
    // myTransfer.sendData(MAX_PACKET_SIZE);

    // COLOCAR SEGMENTOS DA IMAGEM NO BUFFER PARA MANDAR (PAREI AQUI!!!!!!)

    b_StartIndex = lastIndex;

    if (counter < numLoops) {

      stuffPixels(fb->buf, i_StartIndex, b_StartIndex, PIXELS_PER_PACKET);
      i_StartIndex += lastIndex + PIXELS_PER_PACKET;
      // 1o loop: i_StartIndex += 0
      // 2o loop: i_StartIndex += 246
      // 3o loop: i_StartIndex += 246 + 246

    }
    else if (counter == numLoops) {

      stuffPixels(fb->buf, i_StartIndex, b_StartIndex, sizeLastLoop);
      i_StartIndex = 0;

    }

    //Serial.println(" PACKET SENT!");

    myTransfer.sendData(MAX_PACKET_SIZE);

    //myTransfer.sendData(sizeof(numLoops));

    //myTransfer.sendData(sizeof(sizeLastLoop));

    //delay(100);

  } 

  Serial.println(" # IMAGE SENT! # ");
  //Serial.println("#");

  // LIMPA O BUFFER DE FRAME DA CAMERA
  esp_camera_fb_return(fb);   //clear camera memory
  delay(2000);

  startIndex = 0;
  lastIndex = 0;

}
