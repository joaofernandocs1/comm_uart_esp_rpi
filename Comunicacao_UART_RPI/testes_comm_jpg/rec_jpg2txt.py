import RPi.GPIO as GPIO
from time import sleep
import numpy as np
import struct
from pySerialTransfer import pySerialTransfer as txfer
import cv2
from PIL import Image
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


# criar a variável a cada recebimento pode atrasar o código
buffSize = 36000 # heurística
buffPos = 0

meta_size = 4
default_packet_size = 246

meta_array = np.zeros(meta_size, dtype=np.uint16)

huff_bytes = bytearray(buffSize)

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

        buff_array[i] = Comm.rx_obj(obj_type='B', start_pos=pixelCount)
        pixelCount += txfer.STRUCT_FORMAT_LENGTHS['B']

        #buff_array[i] = Comm.rx_obj(obj_type='c', start_pos=pixelCount)
        #pixelCount += txfer.STRUCT_FORMAT_LENGTHS['c']

        #print(buff_array[i])
        #printPixel(buff_array[i], pixelCount)

    #print(buff_array)
    #print("sizePacket: ")
    #print(sizePacket)

    print("RC")  

    return buff_array, sizePacket


def buff2Img(rec_array, buff_length, img_last_index):

    #print("img_pos: ")
    #print(img_pos)
    #print("buff_length: ")
    #print(buff_length)
    #img_last_index += buff_length # ESTA NO LUGAR ERRADO
    #print("img_last_index: ")
    #print(img_last_index)

    #print("buff_array: ")
    #print(rec_array)
    #print(len(buff_array))
    #print("img_array: ")
    #print(len(img_array))

    for j in range(buff_length):

        img_array[j + img_last_index] = rec_array[j]
        #huff_bytes[j + img_last_index] = rec_array[j]

        #print("j + img_last_index: ")
        #print(j + img_last_index)

        #print("img_array[j + img_last_index]: ")
        #print(img_array[j + img_last_index])

        #print("rec_array[j]: ")
        #print(rec_array[j])

    img_last_index += buff_length

    return img_array, img_last_index
    #return huff_bytes, img_last_index

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
            
            buff_array = np.zeros(default_packet_size, dtype=np.uint8)
            img_array = np.zeros(buffSize, dtype=np.uint8)

        if count == 1 or recvng:

            recvng = True

            if count == numLoops:

                rec_array, rec_size = recPixels(last_packet_size)
                fin_array, img_pos = buff2Img(rec_array, rec_size, img_pos)
                
                #print(fin_array)
                print("IMG_RC")

                file = open("out_jpg2txt.txt", "w")

                for k in range(buffSize):

                    file.write(str(fin_array[k]))
                    #file.write("\r\n")
                    file.write(" ")

                #print(fin_array)

                file.close()
                print("FW")

                #img = Image.fromarray(fin_array)

                img_pos = 0
                recvng = False

            elif count < numLoops:

                rec_array, rec_size = recPixels(default_packet_size)
                fin_array, img_pos = buff2Img(rec_array, rec_size, img_pos)

                #print(len(fin_array)) # 36.000: OK!
                #print(fin_array)

