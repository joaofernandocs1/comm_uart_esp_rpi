import RPi.GPIO as GPIO
import time 
import serial

# COMUNICAÇÃO UART ENTRE RASPBERRY PI 3B E ESP32CAM

#PROGRAM

#FTDI <---> ESP32CAM

#TX      <--> UOR/RX(3)
#RX      <--> UOT/TX(1)
#GND     <--> GND
#VCC(5V) <--> 5V

# CONEXAO

# CONEXAO

#RPi3B <---> ESP32

#PIN 8 (TXD) <--> GPIO 16 (RX2) AZUL
#PIN 10 (RXD)<--> GPIO 17 (TX2) CINZA
#PIN 16      <--> GPIO 4        LARANJA
#GND(6)      <--> GND           MARROM
#VCC (VIA USB)

#Configura o uso do terminal serial e da taxa de transmissão de dados
ser = serial.Serial('/dev/ttyUSB0', 115200)
#ser = serial.Serial('/dev/ttyAMA0', 115200)
#ser = serial.Serial('/dev/ttyS0', 115200)

#Configuração inicial dos terminais GPIO
GPIO.setwarnings(True)

GPIO.setmode(GPIO.BOARD) # usar números dos pinos, melhor para vários RPis no mesmo projeto. Número de GPIO muda de versão para versão.
GPIO.setup(16, GPIO.IN, pull_up_down=GPIO.PUD_DOWN) # PIN 16 (GPIO 23) 
#GPIO.setup(10, GPIO.IN) # 15 RX PIN

#GPIO.setmode(GPIO.BCM) # usar números dos GPIOs
#GPIO.setup(14, GPIO.OUT) # 14 TX PIN
#GPIO.setup(15, GPIO.IN) # 15 RX PIN

buff = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0] # 8 elementos a receber
pos = 0

rec = False

val = ""

#loop 
while True:

    #ciclo de leitura
    while GPIO.input(16):

        data = ser.read()
        dec_data = data.decode('ISO-8859-1') 
        #print("Chegou: ")
        #print(dec_data)

        if dec_data != " ": ### AVALIAR RSTRIP() + READLINE() PARA RETIRAR OS DOIS ESPAÇOS VAZIOS

            if dec_data == '@' or rec:

                if pos%3 == 0 and pos != 0:

                    buff[pos//3] = dec_data ### AVALIAR RSTRIP() + READLINE() PARA RETIRAR OS DOIS ESPAÇOS VAZIOS
                    #print("Armazenou: ")
                    #print(buff[pos//3])

                elif pos == 0:

                    buff[pos] = dec_data
                    #print(buff[pos])

                rec = True
                #print("Pos: ")
                #print(pos)
                pos = pos + 1
                    
        if dec_data == '#':
            
            rec = False
            #print("Recebido! ")

    #printar dados
    if (pos > 0 and rec is False):

        for i in range(1, 9):

            val += str(buff[i])
            buff[i] = 0

        print("Val: ");
        print(val);
        #print("len: ");
        #print(pos-1);

        pos = 0
        val = ""
        