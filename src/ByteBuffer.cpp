/**
ByteBuffer
ByteBuffer.cpp
Copyright 2011 - 2013 Ramsey Kant
Copyright 2015 Scott Stark

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

#include "ByteBuffer.h"

/**
* ByteBuffer constructor
* Reserves specified size in internal vector
*
* @param size Size (in bytes) of space to preallocate internally. Default is set in DEFAULT_SIZE
*/
ByteBuffer::ByteBuffer(uint32_t size) {
    buf.reserve(size);
    clear();
#ifdef BB_UTILITY
    name = "";
#endif
}

/**
* ByteBuffer constructor
* Consume an entire uint8_t array of length len in the ByteBuffer
*
* @param arr uint8_t array of data (should be of length len)
* @param size Size of space to allocate
*/
ByteBuffer::ByteBuffer(uint8_t* arr, uint32_t size) {
    // If the provided array is NULL, allocate a blank buffer of the provided size
    if(arr == NULL) {
        buf.reserve(size);
        clear();
    } else { // Consume the provided array
        buf.reserve(size);
        clear();
        putBytes(arr, size);
    }

#ifdef BB_UTILITY
    name = "";
#endif
}

/**
* ByteBuffer Deconstructor
*
*/
ByteBuffer::~ByteBuffer() {
}

/**
* Bytes Remaining
* Returns the number of bytes from the current read position till the end of the buffer
*
* @return Number of bytes from rpos to the end (size())
*/
uint32_t ByteBuffer::bytesRemaining() {
    return size()-rpos;
}

/**
* Clear
* Clears out all data from the internal vector (original preallocated size remains), resets the positions to 0
*/
void ByteBuffer::clear() {
    rpos = 0;
    wpos = 0;
    buf.clear();
}

/**
* Clone
* Allocate an exact copy of the ByteBuffer on the heap and return a pointer
*
* @return A pointer to the newly cloned ByteBuffer. NULL if no more memory available
*/
ByteBuffer* ByteBuffer::clone() {
    ByteBuffer* ret = new ByteBuffer(buf.size());

    // Copy data
    for(uint32_t i = 0; i < buf.size(); i++) {
        ret->put((uint8_t)get(i));
    }

    // Reset positions
    ret->setReadPos(0);
    ret->setWritePos(0);

    return ret;
}

/**
* Equals, test for data equivilancy
* Compare this ByteBuffer to another by looking at each byte in the internal buffers and making sure they are the same
*
* @param other A pointer to a ByteBuffer to compare to this one
* @return True if the internal buffers match. False if otherwise
*/
bool ByteBuffer::equals(ByteBuffer* other) {
    // If sizes aren't equal, they can't be equal
    if(size() != other->size())
        return false;

    // Compare byte by byte
    uint32_t len = size();
    for(uint32_t i = 0; i < len; i++) {
        if((uint8_t)get(i) != (uint8_t)other->get(i))
            return false;
    }

    return true;
}

/**
* Resize
* Reallocates memory for the internal buffer of size newSize. Read and write positions will also be reset
*
* @param newSize The amount of memory to allocate
*/
void ByteBuffer::resize(uint32_t newSize) {
    buf.resize(newSize);
    rpos = 0;
    wpos = 0;
}

/**
* Size
* Returns the size of the internal buffer...not necessarily the length of bytes used as data!
*
* @return size of the internal buffer
*/
uint32_t ByteBuffer::size() {
    return buf.size();
}

// Read Functions

uint8_t ByteBuffer::peek() {
    return read<uint8_t>(rpos);
}

uint8_t ByteBuffer::get() {
    return read<uint8_t>();
}

uint8_t ByteBuffer::get(uint32_t index) {
    return read<uint8_t>(index);
}

void ByteBuffer::getBytes(uint8_t* buf, uint32_t len) {
    for(uint32_t i = 0; i < len; i++) {
        buf[i] = read<uint8_t>();
    }
}

char ByteBuffer::getChar() {
    return read<char>();
}

double ByteBuffer::getDouble() {
    return read<double>();
}

float ByteBuffer::getFloat() {
    return read<float>();
}

int32_t ByteBuffer::getInt() {
    int ch1 = read();
    int ch2 = read();
    int ch3 = read();
    int ch4 = read();
    return ((ch1 << 24) + (ch2 << 16) + (ch3 << 8) + (ch4 << 0));
}

int64_t ByteBuffer::getLong() {
    int64_t ch1 = read();
    int64_t ch2 = read();
    int64_t ch3 = read();
    int64_t ch4 = read();
    int64_t ch5 = read();
    int64_t ch6 = read();
    int64_t ch7 = read();
    int64_t ch8 = read();
    int64_t value = ((ch1 << 56) + (ch2 << 48) + (ch3 << 40) + (ch4 << 32) + (ch5 << 24) + (ch6 << 16) + (ch7 << 8) + (ch8 << 0));
    return value;
}

int16_t ByteBuffer::getShort() {
    int ch1 = read();
    int ch2 = read();
    return ((ch1 << 8) + (ch2 << 0));
}

std::string ByteBuffer::readString() {
    uint32_t len = readInt();
    char str[len+1];
    getBytes((uint8_t *)str, len);
    str[len] = 0;
    return std::string(str);
}

// Write Functions

void ByteBuffer::put(ByteBuffer* src) {
    uint32_t len = src->size();
    for(uint32_t i = 0; i < len; i++)
        append<uint8_t>(src->get(i));
}

void ByteBuffer::put(uint8_t b) {
    append<uint8_t>(b);
}

void ByteBuffer::putBytes(uint8_t* b, uint32_t len) {
    // Insert the data one byte at a time into the internal buffer at position i+starting index
    for(uint32_t i = 0; i < len; i++)
        write(b[i]);
}
void ByteBuffer::writeBytes(std::string str) {
    uint32_t len = str.size();
    writeInt(len);
    for(uint32_t i = 0; i < len; i++) {
        write(str[i]);
    }
}
void ByteBuffer::putChar(char value) {
    append<char>(value);
}

void ByteBuffer::putDouble(double value) {
    append<double>(value);
}

void ByteBuffer::putFloat(float value) {
    append<float>(value);
}

void ByteBuffer::putInt(uint32_t v) {
    write((u_int8_t) ((v >> 24) & 0x0FF));
    write((u_int8_t) ((v >> 16) & 0x0FF));
    write((u_int8_t) ((v >>  8) & 0x0FF));
    write((u_int8_t) ((v >>  0) & 0x0FF));
}

void ByteBuffer::putLong(uint64_t v) {
    write((u_int8_t) ((v >> 56) & 0x0FF));
    write((u_int8_t) ((v >> 48) & 0x0FF));
    write((u_int8_t) ((v >>  40) & 0x0FF));
    write((u_int8_t) ((v >>  32) & 0x0FF));
    write((u_int8_t) ((v >> 24) & 0x0FF));
    write((u_int8_t) ((v >> 16) & 0x0FF));
    write((u_int8_t) ((v >>  8) & 0x0FF));
    write((u_int8_t) ((v >>  0) & 0x0FF));
}

void ByteBuffer::putShort(uint16_t v) {
    write((u_int8_t) ((v >>  8) & 0x0FF));
    write((u_int8_t) ((v >>  0) & 0x0FF));
}

// Utility Functions
#ifdef BB_UTILITY
void ByteBuffer::setName(std::string n) {
    name = n;
}

std::string ByteBuffer::getName() {
    return name;
}

void ByteBuffer::printInfo() {
    uint32_t length = buf.size();
    std::cout << "ByteBuffer " << name.c_str() << " Length: " << length << ". Info Print" << std::endl;
}

void ByteBuffer::printAH() {
    uint32_t length = buf.size();
    std::cout << "ByteBuffer " << name.c_str() << " Length: " << length << ". ASCII & Hex Print" << std::endl;
    for(uint32_t i = 0; i < length; i++) {
        printf("0x%02x ", buf[i]);
    }
    printf("\n");
    for(uint32_t i = 0; i < length; i++) {
        printf("%c ", buf[i]);
    }
    printf("\n");
}

void ByteBuffer::printAscii() {
    uint32_t length = buf.size();
    std::cout << "ByteBuffer " << name.c_str() << " Length: " << length << ". ASCII Print" << std::endl;
    for(uint32_t i = 0; i < length; i++) {
        printf("%c ", buf[i]);
    }
    printf("\n");
}

void ByteBuffer::printHex() {
    uint32_t length = buf.size();
    std::cout << "ByteBuffer " << name.c_str() << " Length: " << length << ". Hex Print" << std::endl;
    for(uint32_t i = 0; i < length; i++) {
        printf("0x%02x ", buf[i]);
    }
    printf("\n");
}

void ByteBuffer::printPosition() {
    uint32_t length = buf.size();
    std::cout << "ByteBuffer " << name.c_str() << " Length: " << length << " Read Pos: " << rpos << ". Write Pos: " << wpos << std::endl;
}
#endif
