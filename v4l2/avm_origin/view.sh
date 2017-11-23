gst-launch-1.0  udpsrc  address=127.0.0.1 port=8554 caps="application/x-rtp,encoding-name=JPEG,payload=26" ! rtpjpegdepay ! jpegdec ! autovideosink
