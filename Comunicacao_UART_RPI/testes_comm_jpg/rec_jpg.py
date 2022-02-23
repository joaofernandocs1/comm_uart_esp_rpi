import RPi.GPIO as GPIO
from time import sleep
#import serial
import struct
from pySerialTransfer import pySerialTransfer as txfer

# ser = serial.Serial('/dev/ttyUSB0', 115200)
# 9600, 38400, 115200, 230400, 460800, 576000, 921600

# 'h' = 2 bytes
# 'H' = 2 bytes
Comm = txfer.SerialTransfer('/dev/ttyUSB0', 9600)

class struct(object):
    counter = 0
    imSize = 0
    numLoops = 0 
    sizeLastLoop = 0
    imgPixel = 0

def recMeta():

    metaSize = 0

    struct.counter = Comm.rx_obj(obj_type='H', start_pos=metaSize)
    metaSize += txfer.STRUCT_FORMAT_LENGTHS['H']
    print("counter: ", struct.counter)
    #print(struct.imSize)

    '''print("SIZE struct.counter: ")
    print(metaSize)
    print("TYPE txfer.STRUCT_FORMAT_LENGTHS['h']: ")
    print(type(txfer.STRUCT_FORMAT_LENGTHS['h']))
    print("TYPE metaSize: ")
    print(type(metaSize))
    print("TYPE struct.counter: ")
    print(type(struct.counter))
    print(" / ")'''

    struct.imSize = Comm.rx_obj(obj_type='H', start_pos=metaSize)
    metaSize += txfer.STRUCT_FORMAT_LENGTHS['H']
    print("imSize: ", struct.imSize)
    #print(struct.imSize)

    '''print("SIZE struct.imSize: ")
    print(metaSize)
    print("TYPE txfer.STRUCT_FORMAT_LENGTHS['h']: ")
    print(type(txfer.STRUCT_FORMAT_LENGTHS['h']))
    print("TYPE metaSize: ")
    print(type(metaSize))
    print("TYPE struct.imSize: ")
    print(type(struct.imSize))
    print(" / ")'''

    struct.numLoops = Comm.rx_obj(obj_type='H', start_pos=metaSize)
    metaSize += txfer.STRUCT_FORMAT_LENGTHS['H']
    print("numLoops: ", struct.numLoops)
    #print(struct.numLoops)

    '''print("SIZE struct.numLoops: ")
    print(metaSize)
    print("TYPE txfer.STRUCT_FORMAT_LENGTHS['h']: ")
    print(type(txfer.STRUCT_FORMAT_LENGTHS['h']))
    print("TYPE metaSize: ")
    print(type(metaSize))
    print("TYPE struct.numLoops: ")
    print(type(struct.numLoops))
    print(" / ")'''

    struct.sizeLastLoop = Comm.rx_obj(obj_type='H', start_pos=metaSize)
    metaSize += txfer.STRUCT_FORMAT_LENGTHS['H']
    print("sizeLastLoop: ", struct.sizeLastLoop)
    #print(struct.sizeLastLoop)

    '''print("SIZE struct.sizeLastLoop: ")
    print(metaSize)
    print("TYPE txfer.STRUCT_FORMAT_LENGTHS['h']: ")
    print(type(txfer.STRUCT_FORMAT_LENGTHS['h']))
    print("TYPE metaSize: ")
    print(type(metaSize))
    print("TYPE struct.sizeLastLoop: ")
    print(type(struct.sizeLastLoop))
    print(" / ")'''

    return struct.counter, struct.numLoops

def recPixels():

    pixelSize = 0

    for i in range(246):

        struct.imgPixel = Comm.rx_obj(obj_type='B', start_pos=pixelSize)
        pixelSize += txfer.STRUCT_FORMAT_LENGTHS['B']
        #print("PX: ", struct.imgPixel)
        #print(struct.imgPixel)

        '''print("SIZE struct.imgPixel: ")
        print(recSize)
        print("TYPE txfer.STRUCT_FORMAT_LENGTHS['h']: ")
        print(type(txfer.STRUCT_FORMAT_LENGTHS['h']))
        print("TYPE pixelSize: ")
        print(type(pixelSize))
        print("TYPE struct.imgPixel: ")
        print(type(struct.imgPixel))
        print(" / ")'''

    print("RC")  


Comm.open()
sleep(2)

while True:

    if Comm.available():

        ord, numLoops = recMeta()

        recPixels()

        if ord == numLoops:

            print("IMG_RC")

        
