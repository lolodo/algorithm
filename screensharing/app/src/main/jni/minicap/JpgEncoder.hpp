#ifndef MINICAP_JPG_ENCODER_HPP
#define MINICAP_JPG_ENCODER_HPP

#include <turbojpeg.h>



class JpgEncoder {
public:

enum Format {
    FORMAT_NONE          = 0x01,
    FORMAT_CUSTOM        = 0x02,
    FORMAT_TRANSLUCENT   = 0x03,
    FORMAT_TRANSPARENT   = 0x04,
    FORMAT_OPAQUE        = 0x05,
    FORMAT_RGBA_8888     = 0x06,
    FORMAT_RGBX_8888     = 0x07,
    FORMAT_RGB_888       = 0x08,
    FORMAT_RGB_565       = 0x09,
    FORMAT_BGRA_8888     = 0x0a,
    FORMAT_RGBA_5551     = 0x0b,
    FORMAT_RGBA_4444     = 0x0c,
    FORMAT_UNKNOWN       = 0x00,
};

struct Frame {
    void const* data;
    Format format;
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    uint32_t bpp;
    size_t size;
};



  JpgEncoder(unsigned int prePadding, unsigned int postPadding);

  ~JpgEncoder();

  bool
  encode(Frame* frame, unsigned int quality);

  int
  getEncodedSize();

  unsigned char*
  getEncodedData();

  bool
  reserveData(uint32_t width, uint32_t height);

private:
  tjhandle mTjHandle;
  int mSubsampling;
  unsigned int mPrePadding;
  unsigned int mPostPadding;
  unsigned int mMaxWidth;
  unsigned int mMaxHeight;
  unsigned char* mEncodedData;
  unsigned long mEncodedSize;

  static int
  convertFormat(Format format);
};

#endif
