#pragma once

#include "mc/_HeaderOutputPredefine.h"

namespace mce {

class Blob {
public:
    struct Deleter;

    using value_type = uchar;

    using delete_function = void (*)(uchar*);

    struct Deleter {
    public:
        uchar* mUnk90cf01;

    public:
        void operator()(uchar* ptr) const { delete[] ptr; }
    };

    using pointer_type = ::std::unique_ptr<uchar[0], ::mce::Blob::Deleter>;

    using reference = uchar&;

    using const_reference = uchar const&;

    using pointer = uchar*;

    using const_pointer = uchar const*;

    using iterator = uchar*;

    using const_iterator = uchar const*;

    using difference_type = int64;

    using size_type = uint64;

public:
    std::unique_ptr<uchar[], mce::Blob::Deleter> mBlob;
    uint64                                        mSize;

public:
    Blob() : mSize(0) {}
    Blob(Blob&& rhs) noexcept : mBlob(std::move(rhs.mBlob)), mSize(rhs.mSize) { rhs.mSize = 0; }

    Blob(size_t input_length) : mSize(input_length) {
        mBlob = std::unique_ptr<uchar[], Deleter>(new uint8_t[input_length], Deleter{});
    }

    Blob(uint8_t const* input, size_t input_length) : Blob(input_length) {
        std::memcpy(this->mBlob.get(), input, input_length);
    }

    Blob& operator=(Blob&& rhs) noexcept {
        if (&rhs != this) {
            mBlob     = std::move(rhs.mBlob);
            mSize     = rhs.mSize;
            rhs.mSize = 0;
        }
        return *this;
    }

    uint8_t*       begin() { return this->mBlob.get(); }
    uint8_t*       end() { return this->mBlob.get() + this->mSize; }
    uint8_t const* cbegin() const { return this->mBlob.get(); }
    uint8_t const* cend() const { return this->mBlob.get() + this->mSize; }
    uint8_t*       data() { return this->mBlob.get(); }
    uint8_t const* data() const { return this->mBlob.get(); }
    bool           empty() const { return (this->mSize == 0); }
    size_t         size() const { return this->mSize; }
    
    Blob           clone() const { return {this->data(), this->size()}; }
};

} // namespace mce
