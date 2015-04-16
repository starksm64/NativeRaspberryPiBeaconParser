/**
ByteBuffer
ByteBuffer.h
Copyright 2011 - 2013 Ramsey Kant
Copyright 2015 Scott Stark; Changed to write multi-byte length data in a machine independent ordering to be consistent
with the Java DataInput/DataOutput interfaces. Only the integer based numeric types are handled correctly.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef _ByteBuffer_H_
#define _ByteBuffer_H_

// Default number of uint8_ts to allocate in the backing buffer if no size is provided
#define DEFAULT_SIZE 4096

// If defined, utility functions within the class are enabled
#define BB_UTILITY

#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#ifdef BB_UTILITY
#include <iostream>
#include <stdio.h>
#endif

class ByteBuffer {
private:
    uint32_t rpos, wpos;
    std::vector<uint8_t> buf;

#ifdef BB_UTILITY
    std::string name;
#endif

    template <typename T> T read() {
        T data = read<T>(rpos);
        rpos += sizeof(T);
        return data;
    }

    template <typename T> T read(uint32_t index) const {
        if(index + sizeof(T) <= buf.size())
            return *((T*)&buf[index]);
        return 0;
    }

    template <typename T> void append(T data) {
        uint32_t s = sizeof(data);

        if (size() < (wpos + s))
            buf.resize(wpos + s);
        memcpy(&buf[wpos], (uint8_t*)&data, s);
        //printf("writing %c to %i\n", (uint8_t)data, wpos);

        wpos += s;
    }

    /**
    * Internal write of a byte to the underlying array
    */
    void write(uint8_t data) {
        if (size() < (wpos + 1))
            buf.resize(wpos + 1);
        buf[wpos] = data;
        wpos ++;
    }

    template <typename T> void insert(T data, uint32_t index) {
        if((index + sizeof(data)) > size())
            return;

        memcpy(&buf[index], (uint8_t*)&data, sizeof(data));
        wpos = index+sizeof(data);
    }

public:
    ByteBuffer(uint32_t size = DEFAULT_SIZE);
    ByteBuffer(uint8_t* arr, uint32_t size);
    virtual ~ByteBuffer();

    uint32_t bytesRemaining(); // Number of uint8_ts from the current read position till the end of the buffer
    void clear(); // Clear our the vector and reset read and write positions
    ByteBuffer* clone(); // Return a new instance of a ByteBuffer with the exact same contents and the same state (rpos, wpos)
    bool equals(ByteBuffer* other); // Compare if the contents are equivalent
    void resize(uint32_t newSize);
    uint32_t size(); // Size of internal vector

    // Read

    uint8_t peek(); // Relative peek. Reads and returns the next uint8_t in the buffer from the current position but does not increment the read position
    uint8_t get(); // Relative get method. Reads the uint8_t at the buffers current position then increments the position
    uint8_t get(uint32_t index); // Absolute get method. Read uint8_t at index
    void getBytes(uint8_t* buf, uint32_t len); // Absolute read into array buf of length len
    char getChar(); // Relative
    double getDouble();
    float getFloat();
    int32_t getInt();
    int64_t getLong();
    int16_t getShort();
    inline uint8_t read() {return get();}
    inline uint16_t readShort() {return getShort();}
    inline uint32_t readInt() {return getInt();}
    inline uint64_t readLong() {return getLong();}
    std::string readString();

    // Write

    void put(ByteBuffer* src); // Relative write of the entire contents of another ByteBuffer (src)
    void put(uint8_t b); // Relative write
    void putBytes(uint8_t* b, uint32_t len); // Relative write
    void putChar(char value); // Relative
    void putDouble(double value);
    void putFloat(float value);
    void putInt(uint32_t value);
    void putLong(uint64_t value);
    void putShort(uint16_t value);
    inline void writeInt(uint32_t value) {putInt(value);}
    inline void writeLong(uint64_t value) {putLong(value);}
    void writeBytes(std::string str);
    // Reserve space at the current write locaton and return that location after advancing the write pos by size
    uint32_t reserveBytes(uint32_t bytes) {
        uint32_t currentWpos = wpos;
        wpos += bytes;
        if (size() < (wpos + bytes))
            buf.resize(wpos + bytes);
        return currentWpos;
    }

    // Buffer Position Accessors & Mutators

    void setReadPos(uint32_t r) {
        rpos = r;
    }

    uint32_t getReadPos() {
        return rpos;
    }

    void setWritePos(uint32_t w) {
        wpos = w;
    }

    uint32_t getWritePos() {
        return wpos;
    }

    std::vector<uint8_t>& getData() {
        return buf;
    }

    // Utility Functions
#ifdef BB_UTILITY
    void setName(std::string n);
    std::string getName();
    void printInfo();
    void printAH();
    void printAscii();
    void printHex();
    void printPosition();
#endif

};

#endif
