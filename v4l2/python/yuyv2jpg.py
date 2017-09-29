import vision_definitions
import numpy
import time
import Image

camProxy = ALProxy("ALVideoDevice", NAO_IP, PORT)
t0 = time.time()

# block to capture image from NAO's camera with 640x480 resolution and YUV422 colorspace
client = camProxy.subscribe("client", vision_definitions.kVGA, vision_definitions.kYUV422ColorSpace, 5)
t0 = time.time()
img = camProxy.getImageRemote(client)
t1 = time.time()
print "acquisition delay ", t1 - t0
camProxy.unsubscribe("client")

binImage = img[6]

arr = numpy.fromstring(binImage, dtype=numpy.uint8)
y = arr[0::2]
u = arr[1::4]
v = arr[3::4]

# converting YUV422 to YUV444 format, this can be done is better way but I didn't find any implementation so...
yuv = numpy.ones((len(y))*3, dtype=numpy.uint8)
yuv[::3] = y
yuv[1::6] = u
yuv[2::6] = v
yuv[4::6] = u
yuv[5::6] = v
yuvstr = yuv.tostring()

# Using PIL's fromstring format save image as jpg
Img = Image.fromstring("YCbCr",(img[0],img[1]),yuvstr)
Img.save("Image.jpg")