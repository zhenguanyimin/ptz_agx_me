#ifndef __HELPER_BUFFER_H__
#define __HELPER_BUFFER_H__
#include <cassert>
#include <vector>
#include <string>
#include <array>
#include <algorithm>
#include <atomic>
#include <functional>
#ifdef ENABLE_BIG_ENDIAN_BYTE_ORDER
#include <WinSock2.h>
#endif
#include <codehelper/include/general.h>
HELPER_BEGIN_NAMESPACE(HELPER)

class CBaseBuffer
{
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024 * 4;

    CBaseBuffer(size_t initialSize)
        :readerIndex_(kCheapPrepend),
        writerIndex_(kCheapPrepend) {}

    virtual size_t getSize() const = 0;
    virtual const char* begin() const = 0;
    virtual char* begin() = 0;
    virtual size_t internalCapacity() const = 0;
    virtual void ensureWritableBytes(size_t len) =0;
    virtual size_t Capacity() const = 0;
    virtual const char* buf() const = 0;
    virtual char* buf() = 0;

    size_t readableBytes() const{
        return writerIndex_ - readerIndex_;
    }

    size_t writableBytes() const{
        return getSize() - writerIndex_;
    }

    size_t prependableBytes() const{
        return readerIndex_;
    }

    //const char* peek() const
    //{
    //    return begin() + readerIndex_;
    //}

    const void* peek() const{
        return begin() + readerIndex_;
    }

    void retrieve(size_t len){
        assert(len <= readableBytes());
        if (len < readableBytes()){
            readerIndex_ += len;
        }
        else{
            retrieveAll();
        }
    }

    void retrieveInt64()
    {
        retrieve(sizeof(int64_t));
    }

    void retrieveInt32()
    {
        retrieve(sizeof(int32_t));
    }

    void retrieveInt16()
    {
        retrieve(sizeof(int16_t));
    }

    void retrieveInt8()
    {
        retrieve(sizeof(int8_t));
    }

    void retrieveAll()
    {
        readerIndex_ = kCheapPrepend;
        writerIndex_ = kCheapPrepend;
    }

    void append(const char* /*restrict*/ data, size_t len) {
        if (NULL == data) return;
        ensureWritableBytes(len);
        std::copy(data, (data + len), beginWrite());
        //memcpy(beginWrite(), data, len);
        //byte* p = (byte*)beginWrite();
        hasWritten(len);
    }

    void append(const void* /*restrict*/ data, size_t len)
    {
        append(static_cast<const char*>(data), len);
    }

    char* beginWrite()
    {
        return begin() + writerIndex_;
    }

    const char* beginWrite() const
    {
        return begin() + writerIndex_;
    }

    void hasWritten(size_t len)
    {
        assert(len <= writableBytes());
        writerIndex_ += len;
    }

    void unwrite(size_t len)
    {
        assert(len <= readableBytes());
        writerIndex_ -= len;
    }

#ifdef ENABLE_BIG_ENDIAN_BYTE_ORDER
    void appendUInt64Ex(uint64_t x)
    {
        uint64_t be64 = static_cast<uint64_t>(htonll(x));
        append(&be64, sizeof be64);
    }

    void appendUInt32Ex(uint32_t x)
    {
        uint32_t be32 = static_cast<uint32_t>(htonl(x));
        append(&be32, sizeof be32);
    }

    void appendUInt16Ex(uint16_t x)
    {
        uint16_t be16 = static_cast<uint16_t>(htons(x));
        append(&be16, sizeof be16);
    }

    void appendDoubleEx(double x)
    {
        uint64_t be64 = htond(x);
        append(&be64, sizeof be64);
    }

    void appendFloatEx(float x)
    {
        uint32_t be32 = htonf(x);
        append(&be32, sizeof be32);
    }

    void appendInt64Ex(int64_t x)
    {
        appendUInt64Ex(static_cast<uint64_t>(x));
    }

    void appendInt32Ex(int32_t x)
    {
        appendUInt32Ex(static_cast<uint32_t>(x));
    }

    void appendInt16Ex(int16_t x)
    {
        appendUInt16Ex(static_cast<uint16_t>(x));
    }
#endif
    void appendUInt64(uint64_t x)
    {
        append(&x, sizeof x);
    }

    void appendUInt32(uint32_t x)
    {
        append(&x, sizeof x);
    }

    void appendUInt16(uint16_t x)
    {
        append(&x, sizeof x);
    }

    void appendUInt8(uint8_t x)
    {
        append(&x, sizeof x);
    }

    void appendDouble(double x)
    {
        append(&x, sizeof x);
    }

    void appendFloat(float x)
    {
        append(&x, sizeof x);
    }

    void appendInt64(int64_t x)
    {
        append(&x, sizeof x);
    }

    void appendInt32(int32_t x)
    {
        append(&x, sizeof x);
    }

    void appendInt16(int16_t x)
    {
        append(&x, sizeof x);
    }

    void appendInt8(int8_t x)
    {
        append(&x, sizeof x);
    }

    void appendString(const char* v, int len)
    {
        append(v, len);
    }

#ifdef ENABLE_BIG_ENDIAN_BYTE_ORDER
    int64_t readInt64Ex()
    {
        int64_t result = peekInt64Ex();
        retrieveInt64();
        return result;
    }

    int32_t readInt32Ex()
    {
        int32_t result = peekInt32Ex();
        retrieveInt32();
        return result;
    }

    int16_t readInt16Ex()
    {
        int16_t result = peekInt16Ex();
        retrieveInt16();
        return result;
    }

    uint64_t readUInt64Ex()
    {
        uint64_t result = peekUInt64Ex();
        retrieveInt64();
        return result;
    }

    uint32_t readUInt32Ex()
    {
        uint32_t result = peekUInt32Ex();
        retrieveInt32();
        return result;
    }

    uint16_t readUInt16Ex()
    {
        uint16_t result = peekUInt16Ex();
        retrieveInt16();
        return result;
    }

    double readDoubleEx()
    {
        double result = peekDoubleEx();
        retrieveInt64();
        return result;
    }

    float readFloutEx()
    {
        float result = peekFloatEx();
        retrieveInt32();
        return result;
    }
#endif
    int64_t readInt64()
    {
        int64_t result = peekInt64();
        retrieveInt64();
        return result;
    }

    int32_t readInt32()
    {
        int32_t result = peekInt32();
        retrieveInt32();
        return result;
    }

    int16_t readInt16()
    {
        int16_t result = peekInt16();
        retrieveInt16();
        return result;
    }

    uint64_t readUInt64()
    {
        uint64_t result = peekUInt64();
        retrieveInt64();
        return result;
    }

    uint32_t readUInt32()
    {
        uint32_t result = peekUInt32();
        retrieveInt32();
        return result;
    }

    uint16_t readUInt16()
    {
        uint16_t result = peekUInt16();
        retrieveInt16();
        return result;
    }

    int8_t readInt8()
    {
        int8_t result = peekInt8();
        retrieveInt8();
        return result;
    }

    uint8_t readUInt8()
    {
        uint8_t result = peekUInt8();
        retrieveInt8();
        return result;
    }

    double readDouble()
    {
        double result = peekDouble();
        retrieveInt64();
        return result;
    }

    float readFloat()
    {
        float result = peekFloat();
        retrieveInt32();
        return result;
    }

    void readString(char* v, int len)
    {
        if (v != NULL) {
            size_t length = (size_t)len < readableBytes() ? (size_t)len : readableBytes();
            memcpy(v, peek(), length);
            retrieve(length);
        }
    }

#ifdef ENABLE_BIG_ENDIAN_BYTE_ORDER
    /// <summary>
    ///  从大端读取
    /// </summary>
    /// <returns></returns>
    int64_t peekInt64Ex() const
    {
        assert(readableBytes() >= sizeof(int64_t));
        int64_t be64 = 0;
        ::memcpy(&be64, peek(), sizeof be64);
        return ntohll(be64);
    }

    int32_t peekInt32Ex() const
    {
        assert(readableBytes() >= sizeof(int32_t));
        int32_t be32 = 0;
        ::memcpy(&be32, peek(), sizeof be32);
        return ntohl(be32);
    }

    int16_t peekInt16Ex() const
    {
        assert(readableBytes() >= sizeof(int16_t));
        int16_t be16 = 0;
        ::memcpy(&be16, peek(), sizeof be16);
        return ntohs(be16);
    }

    double peekDoubleEx() const
    {
        assert(readableBytes() >= sizeof(double));
        int64_t be64 = 0;
        ::memcpy(&be64, peek(), sizeof be64);
        return ntohd(be64);
    }

    float peekFloatEx() const
    {
        assert(readableBytes() >= sizeof(float));
        uint32_t be32 = 0;
        ::memcpy(&be32, peek(), sizeof be32);
        return ntohf(be32);
    }

    uint64_t peekUInt64Ex() const
    {
        assert(readableBytes() >= sizeof(uint64_t));
        uint64_t be64 = 0;
        ::memcpy(&be64, peek(), sizeof be64);
        return ntohll(be64);
    }

    uint32_t peekUInt32Ex() const
    {
        assert(readableBytes() >= sizeof(uint32_t));
        uint32_t be32 = 0;
        ::memcpy(&be32, peek(), sizeof be32);
        return ntohl(be32);
    }

    uint16_t peekUInt16Ex() const
    {
        assert(readableBytes() >= sizeof(uint16_t));
        uint16_t be16 = 0;
        ::memcpy(&be16, peek(), sizeof be16);
        return ntohs(be16);
    }
#endif
    int64_t peekInt64() const
    {
        assert(readableBytes() >= sizeof(int64_t));
        int64_t x = *static_cast<const int64_t*>(peek());
        return x;
    }

    int32_t peekInt32() const
    {
        assert(readableBytes() >= sizeof(int32_t));
        int32_t x = *static_cast<const int32_t*>(peek());
        return x;
    }

    int16_t peekInt16() const
    {
        assert(readableBytes() >= sizeof(int16_t));
        int16_t x = *static_cast<const int16_t*>(peek());
        return x;
    }

    double peekDouble() const
    {
        assert(readableBytes() >= sizeof(double));
        double x = *static_cast<const double*>(peek());
        return x;
    }

    float peekFloat() const
    {
        assert(readableBytes() >= sizeof(float));
        float x = *static_cast<const float*>(peek());
        return x;
    }

    uint64_t peekUInt64() const
    {
        assert(readableBytes() >= sizeof(uint64_t));
        uint64_t x = *static_cast<const uint64_t*>(peek());
        return x;
    }

    uint32_t peekUInt32() const
    {
        assert(readableBytes() >= sizeof(uint32_t));
        uint32_t x = *static_cast<const uint32_t*>(peek());
        return x;
    }

    uint16_t peekUInt16() const
    {
        assert(readableBytes() >= sizeof(uint16_t));
        uint16_t x = *static_cast<const uint16_t*>(peek());
        return x;
    }

    int8_t peekInt8() const
    {
        assert(readableBytes() >= sizeof(int8_t));
        int8_t x = *static_cast<const int8_t*>(peek());
        return x;
    }

    uint8_t peekUInt8() const
    {
        assert(readableBytes() >= sizeof(uint8_t));
        uint8_t x = *static_cast<const uint8_t*>(peek());
        return x;
    }

    void prependInt64(int64_t x) {
        prepend(&x, sizeof x);
    }

    void prependInt32(int32_t x) {
        prepend(&x, sizeof x);
    }

    void prependInt16(int16_t x) {
        prepend(&x, sizeof x);
    }

    void prependInt8(int8_t x) {
        prepend(&x, sizeof x);
    }

    void prepend(const void* /*restrict*/ data, size_t len) {
        assert(len <= prependableBytes());
        readerIndex_ -= len;
        const char* d = static_cast<const char*>(data);
        memcpy(begin() + readerIndex_, d, len);
        //std::copy_n(d, len, begin() + readerIndex_);
    }

protected:
    size_t readerIndex_;
    size_t writerIndex_;
};

class CBuffer : public CBaseBuffer
{
public:
    explicit CBuffer(size_t initialSize = kInitialSize)
        :CBaseBuffer(kInitialSize)
        , buffer_(kCheapPrepend + initialSize)
    {
        assert(readableBytes() == 0);
        assert(writableBytes() == initialSize);
        assert(prependableBytes() == kCheapPrepend);
    }


    virtual size_t getSize() const
    {
        return buffer_.size();
    }
    virtual const char* begin() const
    {
        return &*buffer_.begin();
    }
    virtual char* begin()
    {
        return &*buffer_.begin();
    }
    virtual size_t Capacity() const
    {
        return buffer_.capacity();
    }
    virtual const char* buf() const
    {
        return begin() + kCheapPrepend;
    }

    virtual char* buf()
    {
        return begin() + kCheapPrepend;
    }
    virtual void ensureWritableBytes(size_t len)
    {
        if (writableBytes() < len)
        {
            makeSpace(len);
        }
        assert(writableBytes() >= len);
    }
protected:
    virtual size_t internalCapacity() const
    {
        return buffer_.capacity();
    }

    virtual void makeSpace(size_t len)
    {
        if (writableBytes() + prependableBytes() < len + kCheapPrepend)
        {
            // FIXME: move readable data
            buffer_.resize(writerIndex_ + len);
        }
        else
        {
            // move readable data to the front, make space inside CBuffer
            assert(kCheapPrepend < readerIndex_);
            size_t readable = readableBytes();
            std::copy(begin() + readerIndex_,
                begin() + writerIndex_,
                begin() + kCheapPrepend);
            readerIndex_ = kCheapPrepend;
            writerIndex_ = readerIndex_ + readable;
            assert(readable == readableBytes());
        }
    }

private:
    std::vector<char> buffer_;
};

class CFixBuffer : public CBaseBuffer
{
public:

    CFixBuffer()
        :CBaseBuffer(1024*4)
    {
        assert(readableBytes() == 0);
        assert(writableBytes() == 1024 * 4- kCheapPrepend);
        assert(prependableBytes() == kCheapPrepend);
    }

    virtual size_t getSize() const
    {
        size_t t = buffer_.max_size();
        return buffer_.max_size();
    }
    virtual const char* begin() const
    {
        return &*buffer_.begin();
    }
    virtual char* begin()
    {
        return &*buffer_.begin();
    }

    virtual const char* buf() const
    {
        return begin() + kCheapPrepend;
    }

    virtual char* buf()
    {
        return begin() + kCheapPrepend;
    }

    virtual size_t Capacity() const
    {
        return getSize();
    }
protected:
    virtual size_t internalCapacity() const
    {
        return buffer_.max_size();
    }

    virtual void ensureWritableBytes(size_t len)
    {
        assert(writableBytes() >= len);
    }

private:
    std::array<char, 1024 * 4> buffer_ = {0};
};

class COnlyReadBuffer : public CBaseBuffer
{
public:

    COnlyReadBuffer(void* ptr, int len)
        :CBaseBuffer(len), buffer_(ptr), buffer_length_(len)
    {
        readerIndex_ = 0;
        writerIndex_ = len;
        assert(buffer_ != NULL);
    }

    virtual size_t getSize() const
    {
        return buffer_length_;
    }
    virtual const char* begin() const
    {
        return (const char*)buffer_;
    }
    virtual char* begin()
    {
        return (char*)buffer_;
    }

    virtual const char* buf() const
    {
        return begin();
    }

    virtual char* buf()
    {
        return begin();
    }

    virtual size_t Capacity() const
    {
        return getSize();
    }
protected:
    virtual size_t internalCapacity() const
    {
        return buffer_length_;
    }

    virtual void ensureWritableBytes(size_t len)
    {
        //assert(false);
        abort();
    }

private:
    void* buffer_ = NULL;
    uint32_t buffer_length_ =0;
};

class CFrameBuffer
{
public:
    explicit CFrameBuffer(size_t initialSize = 1024)
        :buf_(initialSize){}

    bool empty();
    bool hasData();
    void clear();
    int getFrameLength();

    /// <summary>
    /// 
    /// </summary>
    /// <param name="data"></param>
    /// <param name="length"></param>
    /// <param name="flag">0-写入完成 1-后面还有</param>
    int write(const char* data, int length, int flag);

    /// <summary>
    /// 
    /// </summary>
    /// <param name="buf"></param>
    /// <param name="length"></param>
    /// <returns></returns>
    int read(char* buf, int length);

    /// <summary>
    /// 
    /// </summary>
    /// <param name="cb"></param>
    /// <returns></returns>
    int read(std::function<void(char* buf, int length)> cb);

    /// <summary>
    /// 
    /// </summary>
    /// <param name="ptr"></param>
    /// <returns></returns>
    int read(char*& ptr);

    /// <summary>
    /// 
    /// </summary>
    void retrieveFrame();
private:
    CBuffer buf_;
    CBuffer frame_buf_;
    std::atomic_bool wait_frame_finish_ = false;
    const int32_t frame_head_size = sizeof(int32_t);
};

HELPER_END_NAMESPACE(HELPER)
#endif