import RPi.GPIO as GPIO
import time
import serial

# COMUNICAÇÃO UART ENTRE RASPBERRY PI 3B E ESP32CAM

#PROGRAM

#FTDI <---> ESP32CAM

#TX <--> RX(3)
#RX <--> TX(1)
#GND <--> GND

#CONNECT

#RPI3B <---> ESP32CAM

#TX(14) <--> RX(13)
#RX(15) <--> TX(12)
#GND(6) <--> GND

#Configura o uso do terminal serial e da taxa de transmissão de dados
ser = serial.Serial('/dev/ttyS0', 9600)

#Configuração inicial dos terminais GPIO
GPIO.setwarnings(True)

#GPIO.setmode(GPIO.BOARD) # usar números dos pinos, melhor para vários RPis no mesmo projeto. Número de GPIO muda de versão para versão.
#GPIO.setup(8, GPIO.OUT) # 14 TX PIN
#GPIO.setup(10, GPIO.IN) # 15 RX PIN
GPIO.setmode(GPIO.BCM) # usar números dos GPIOs
#GPIO.setup(14, GPIO.OUT) # 14 TX PIN
#GPIO.setup(15, GPIO.IN) # 15 RX PIN

beg_pt = '@'
end_pt = '#'

umid = '89.173'
temp = '35'
char = 'FMW'


while True:
    
    temp_st = str(beg_pt + temp + end_pt)    
    umid_st = str(beg_pt + umid + end_pt)
    char_st = str(beg_pt + char + end_pt)

 
    ser.write(temp_st.encode('ascii'))

    ser.write(umid_st.encode('utf-8'))

    ser.write(str.encode(char_st))

    time.sleep(0.2)
