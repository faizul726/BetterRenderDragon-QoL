#pragma once

#include "mc/_HeaderOutputPredefine.h"


#include "mc/deps/core/container/Blob.h"
#include "mc/deps/core/image/ImageFormat.h"
#include "mc/deps/core/image/ImageUsage.h"


namespace mce {

struct Image {
public:
    using Storage = ::mce::Blob;

public:
    mce::ImageFormat mImageFormat;
    uint             mWidth;
    uint             mHeight;
    uint             mDepth = 0;
    mce::ImageUsage  mUsage;
    mce::Blob        mImageBytes;

public:
    Image() : mImageFormat(ImageFormat::Unknown), mWidth(0), mHeight(0), mUsage(ImageUsage::Unknown) {}
    Image(ImageFormat format, uint32_t width, uint32_t height, ImageUsage usage, Blob&& data)
    : mImageFormat(format),
      mWidth(width),
      mHeight(height),
      mUsage(usage),
      mImageBytes(std::move(data)) {}
    Image(Blob&& data) : mImageBytes(std::move(data)) {}
    Image(uint32_t width, uint32_t height, ImageFormat format, ImageUsage usage)
    : mImageFormat(format),
      mWidth(width),
      mHeight(height),
      mUsage(usage) {}

    Image& operator=(Image&& rhs) {
        this->mImageFormat = rhs.mImageFormat;
        this->mWidth       = rhs.mWidth;
        this->mHeight      = rhs.mHeight;
        this->mUsage       = rhs.mUsage;
        this->mDepth       = rhs.mDepth;
        this->mImageBytes  = std::move(rhs.mImageBytes);
        return *this;
    }
    mce::Image(const mce::Image& rhs)
    : mImageFormat(rhs.mImageFormat),
      mWidth(rhs.mWidth),
      mHeight(rhs.mHeight),
      mUsage(rhs.mUsage),
      mDepth(rhs.mDepth),
      mImageBytes(rhs.mImageBytes.clone()) {}


    inline Image clone() const {
        return {this->mImageFormat, this->mWidth, this->mHeight, this->mUsage, this->mImageBytes.clone()};
    }
    inline void copyRawImage(Blob const& blob) { this->mImageBytes = blob.clone(); }
    inline bool isEmpty() const { return this->mImageBytes.empty(); }
    inline void resizeImageBytesToFitImageDescription() {
        this->mImageBytes = Blob{this->mWidth * this->mHeight * (int)this->mImageFormat};
    }
    inline void setImageDescription(uint32_t width, uint32_t height, ImageFormat format, ImageUsage usage) {
        this->mWidth       = width;
        this->mHeight      = height;
        this->mImageFormat = format;
        this->mUsage       = usage;
    }
    inline void setRawImage(Blob&& buffer) { this->mImageBytes = std::move(buffer); }
};

} // namespace mce
