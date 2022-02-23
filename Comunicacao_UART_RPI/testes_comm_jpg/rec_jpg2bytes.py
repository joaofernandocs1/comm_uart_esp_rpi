import RPi.GPIO as GPIO
from time import sleep
import numpy as np
import struct
import cv2
import io
import chardet
from PIL import Image
from pySerialTransfer import pySerialTransfer as txfer
#import serial

# ser = serial.Serial('/dev/ttyUSB0', 115200)
# 9600, 38400, 115200, 230400, 460800, 576000, 921600

# 'h' = 2 bytes
# 'H' = 2 bytes

#        ANATOMIA DO BUFFER
#
# 0            7 8              254
# | ImgMetaData | PIXELS DA IMAGEM |
#     8 BYTES        246 BYTES

# pixel na verdade não é o pixel da imagem, descobrimos que é apenas um byte do buffer com o código de huffman

Comm = txfer.SerialTransfer('/dev/ttyUSB0', 9600)

class struct(object):
    counter = 0
    imSize = 0
    numLoops = 0 
    sizeLastLoop = 0
    #imgPixel = 0


buffPos = 0

meta_size = 4
default_packet_size = 246

meta_array = np.zeros(meta_size, dtype=np.uint16)
buff_array = np.zeros(default_packet_size, dtype=np.uint8)
#img_array = np.zeros(buffSize, dtype=np.uint8)

img_pos = 0

recvng = False

def printMeta(counter, imSize, numLoops, sizeLastLoop):

    # counter 
    print("counter: ")
    print(counter)
    print("TYPE counter: ")
    print(type(counter))
    print("SIZE counter: ")
    print(txfer.STRUCT_FORMAT_LENGTHS['H'])
    print("TYPE txfer.STRUCT_FORMAT_LENGTHS['H']: ")
    print(type(txfer.STRUCT_FORMAT_LENGTHS['H']))
    print(" / ")

    # imSize 
    print("imSize: ")
    print(imSize)
    print("TYPE imSize: ")
    print(type(imSize))
    print("SIZE imSize: ")
    print(txfer.STRUCT_FORMAT_LENGTHS['H'])
    print("TYPE txfer.STRUCT_FORMAT_LENGTHS['H']: ")
    print(type(txfer.STRUCT_FORMAT_LENGTHS['H']))
    print(" / ")

    # numLoops 
    print("numLoops: ")
    print(numLoops)
    print("TYPE numLoops: ")
    print(type(numLoops))
    print("SIZE numLoops: ")
    print(txfer.STRUCT_FORMAT_LENGTHS['H'])
    print("TYPE txfer.STRUCT_FORMAT_LENGTHS['H']: ")
    print(type(txfer.STRUCT_FORMAT_LENGTHS['H']))
    print(" / ")

    # sizeLastLoop 
    print("sizeLastLoop: ")
    print(sizeLastLoop)
    print("TYPE sizeLastLoop: ")
    print(type(sizeLastLoop))
    print("SIZE sizeLastLoop: ")
    print(txfer.STRUCT_FORMAT_LENGTHS['H'])
    print("TYPE txfer.STRUCT_FORMAT_LENGTHS['H']: ")
    print(type(txfer.STRUCT_FORMAT_LENGTHS['H']))
    print(" / ")


def printPixel(pixel, pixel_count):

    #pixel 
    print("PX: ")
    print(pixel)
    print("TYPE pixel: ")
    print(type(pixel))
    print("COUNT pixel: ")
    print(pixel_count)
    print("SIZE txfer.STRUCT_FORMAT_LENGTHS['H']: ")
    print(txfer.STRUCT_FORMAT_LENGTHS['H'])
    print("TYPE txfer.STRUCT_FORMAT_LENGTHS['H']: ")
    print(type(txfer.STRUCT_FORMAT_LENGTHS['H']))
    print(" / ")


def recMeta():

    metaSize = 0

    struct.counter = Comm.rx_obj(obj_type='H', start_pos=metaSize)
    metaSize += txfer.STRUCT_FORMAT_LENGTHS['H']

    struct.imSize = Comm.rx_obj(obj_type='H', start_pos=metaSize)
    metaSize += txfer.STRUCT_FORMAT_LENGTHS['H']

    struct.numLoops = Comm.rx_obj(obj_type='H', start_pos=metaSize)
    metaSize += txfer.STRUCT_FORMAT_LENGTHS['H']

    struct.sizeLastLoop = Comm.rx_obj(obj_type='H', start_pos=metaSize)
    metaSize += txfer.STRUCT_FORMAT_LENGTHS['H']

    #printMeta(struct.counter, struct.imSize, strcut.numLoops, struct.sizeLastLoop)

    return struct.counter, struct.imSize, struct.numLoops, struct.sizeLastLoop


def recPixels(sizePacket):

    pixelCount = 0

    for i in range(sizePacket):

        #buff_array[i] = Comm.rx_obj(obj_type='B', start_pos=pixelCount)
        huff_bytes[i] = Comm.rx_obj(obj_type='B', start_pos=pixelCount)
        pixelCount += txfer.STRUCT_FORMAT_LENGTHS['B']
        #huff_bytes[i] = (buff_array[i]).to_bytes(1, byteorder='big')

        #print("buff_array: ")
        #print(buff_array[i])
        #print(type(buff_array[i]))

        #print("huff_buff: ")
        #print(huff_bytes[i])
        #print(type(huff_bytes[i]))

    
        
    #print(buff_array)
    #print("sizePacket: ")
    #print(sizePacket)

    print("RC")  
    #return buff_array, sizePacket
    return huff_bytes, sizePacket

# rec_array == huff_buff

def buff2Img(rec_array, buff_length, img_last_index):

    # rec_array == huff_bytes
    # buff_length == sizePacket
    # img_last_index == ultimo byte da imagem preenchido

    #print("img_pos: ")
    #print(img_pos)
    #print("buff_length: ")
    #print(buff_length)
    #img_last_index += buff_length
    #print("img_last_index: ")
    #print(img_last_index)

    #print("buff_array: ")
    #print(rec_array)
    #print(len(buff_array))
    #print("img_array: ")
    #print(len(img_array))

    for j in range(buff_length):

        #breakpoint()

        #print("pacote: ", count)
        #print("total pacotes: ", numLoops)
        #print("index rec_array: ", j)
        #print("tamanho buffer bytes: ", buff_length)
        #print("tamanho ultimo pacote: ", last_packet_size)
        #img_array[j + img_last_index] = rec_array[j]
        img_bytes[j + img_last_index] = rec_array[j]

        #print("tamanho da imagem: ", buffSize)
        #print("index imagem: ", j + img_last_index)
        #print("tamanho buffer imagem: ", len(img_bytes))

        #print("huff_bytes[j + img_last_index]: ")
        #print(huff_bytes[j + img_last_index])

        #print("TYPE huff_bytes[j + img_last_index]: ")
        #print(type(huff_bytes[j + img_last_index]))

        #print("tamanho ultimo pacote: ", last_packet_size)
        
    img_last_index += buff_length

    #return img_array, img_last_index
    return img_bytes, img_last_index

Comm.open()
sleep(2)

#print("img_array len: ")
#print(len(img_array))

while True:

    if Comm.available():

        count, buffSize, numLoops, last_packet_size = recMeta()

        if count == 1:

            #print(buffSize)
            #print(int(buffSize))
            #print(type(int(buffSize)))
            #print(numLoops)
            #print(type(numLoops))
            #print(last_packet_size)
            #print(type(last_packet_size))
            
            huff_bytes = bytearray(default_packet_size)
            img_bytes = bytearray(int(buffSize))

        if count == 1 or recvng:

            recvng = True

            if count == numLoops:
                
                # RECEBE OS BYTES
                rec_array, rec_size = recPixels(last_packet_size)
                # rec_array == huff_buff
                # E BOTA NO BUFFER DA IMAGEM
                fin_array, img_pos = buff2Img(rec_array, rec_size, img_pos)
                
                #print(fin_array)
                print("IMG_RC")

                #file = open("out_jpg2bytes.txt", "w+")
                '''file = open("out_jpg2bytes.jpg", "w")

                for k in range(buffSize):

                    file.write(str(fin_array[k]))
                    #file.write("\r\n")
                    file.write(" ")
                    #print(str(fin_array[k])) printa os numeros

                #print(fin_array) printa o bytearray com os bytes

                #breakpoint()

                file.close()
                print("FW")'''

                stream = io.BytesIO(fin_array)
                img = Image.frombytes("RGB", (800, 600), stream, "JPEG")
                #picture = Image.open(img)
                img.show()

                img_pos = 0
                recvng = False

            elif count < numLoops:

                rec_array, rec_size = recPixels(default_packet_size)
                fin_array, img_pos = buff2Img(rec_array, rec_size, img_pos)

                #print(len(fin_array)) # 36.000: OK!
                #print(fin_array)
