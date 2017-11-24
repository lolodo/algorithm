from PIL import Image
import sys
from struct import *
import array
 
if len(sys.argv) != 5:
        print "***** Usage syntax Error!!!! *****\n"
        print "Usage:"
        print "python <script> <.yuv file uyvy422> <width> <height> "
        sys.exit(1) # exit
else:
        pass
 
image_name = sys.argv[1]
width = int(sys.argv[2])
height = int(sys.argv[3])
name= str(sys.argv[4])

y = array.array('B')
u = array.array('B')
v = array.array('B')
 
f_uyvy = open(image_name, "rb")


for number in range(0, 4):
    start = number * 800
    end = (number + 1) * 800
    fname = name + "_" + str(number) +".png" 
    print "file name is ", fname

    f_uyvy.seek(width * number * 800 * 2, 0)
    image_out = Image.new("RGB", (width, 800))
    pix = image_out.load()
    for i in range(start, end):
        for j in range(0, width/2):
            y1 = ord(f_uyvy.read(1));
            u  = ord(f_uyvy.read(1));
            y2 = ord(f_uyvy.read(1));
            v  = ord(f_uyvy.read(1));
        
            B = 1.164 * (y1-16) + 2.018 * (u - 128)
            G = 1.164 * (y1-16) - 0.813 * (v - 128) - 0.391 * (u - 128)
            R = 1.164 * (y1-16) + 1.596*(v - 128)
            pix[j*2, i - start] = int(R), int(G), int(B)
 
            B = 1.164 * (y2-16) + 2.018 * (u - 128)
            G = 1.164 * (y2-16) - 0.813 * (v - 128) - 0.391 * (u - 128)
            R = 1.164 * (y2-16) + 1.596*(v - 128)
            pix[j*2+1, i - start] = int(R), int(G), int(B)
 
######################################################
# B = 1.164(Y - 16)                   + 2.018(U - 128)
# G = 1.164(Y - 16) - 0.813(V - 128) - 0.391(U - 128)
# R = 1.164(Y - 16) + 1.596(V - 128)
######################################################
 
    image_out.save(fname)
    #image_out.show()
