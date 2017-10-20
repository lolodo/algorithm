#include <stdexcept>

#include "JpgEncoder.hpp"
#include "util/debug.h"
#include <android/log.h>

JpgEncoder::JpgEncoder(unsigned int prePadding, unsigned int postPadding)
  : mTjHandle(tjInitCompress()),
    mSubsampling(TJSAMP_420),
    mEncodedData(NULL),
    mPrePadding(prePadding),
    mPostPadding(postPadding),
    mMaxWidth(0),
    mMaxHeight(0)
{
}

JpgEncoder::~JpgEncoder() {
  tjFree(mEncodedData);
}

bool
JpgEncoder::encode(Frame* frame, unsigned int quality) {
  unsigned char* offset = getEncodedData();

/*
  __android_log_print(ANDROID_LOG_ERROR, "minicap",
                     "%s:%d frame:%x data:%x, width:%d, stride:%d, bpp:%d, h:%d, o:%x, mE:%d, q:%d, sub:%d", __func__, __LINE__, frame,
                     frame->data, frame->width,
                     frame->stride, frame->bpp,
                     frame->height, offset, mEncodedSize,
                     quality, mSubsampling);
                     */
  return 0 == tjCompress2(
    mTjHandle,
    (unsigned char*) frame->data,
    frame->width,
    frame->stride * frame->bpp,
    frame->height,
    convertFormat(frame->format),
    &offset,
    &mEncodedSize,
    mSubsampling,
    quality,
    TJFLAG_FASTDCT | TJFLAG_NOREALLOC
  );
}

int
JpgEncoder::getEncodedSize() {
  return mEncodedSize;
}

unsigned char*
JpgEncoder::getEncodedData() {
  return mEncodedData + mPrePadding;
}

bool
JpgEncoder::reserveData(uint32_t width, uint32_t height) {
  if (width == mMaxWidth && height == mMaxHeight) {
    return 0;
  }

  tjFree(mEncodedData);

  unsigned long maxSize = mPrePadding + mPostPadding + tjBufSize(
    width,
    height,
    mSubsampling
  );


  mEncodedData = tjAlloc(maxSize);

  if (mEncodedData == NULL) {
    return false;
  }

  mMaxWidth = width;
  mMaxHeight = height;
  __android_log_print(ANDROID_LOG_ERROR, "minicap", "Allocating %ld bytes for JPG encoder, data:%p", maxSize, mEncodedData);

  return true;
}

int
JpgEncoder::convertFormat(Format format) {
  switch (format) {
  case FORMAT_RGBA_8888:
    return TJPF_RGBA;
  case FORMAT_RGBX_8888:
    return TJPF_RGBX;
  case FORMAT_RGB_888:
    return TJPF_RGB;
  case FORMAT_BGRA_8888:
    return TJPF_BGRA;
  default:
    throw std::runtime_error("Unsupported pixel format");
  }
}
