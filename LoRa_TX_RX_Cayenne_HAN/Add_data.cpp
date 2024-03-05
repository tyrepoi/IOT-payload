//#include "Add_data.h"
//#include <stdint.h>
//
////class addData {
////private:
////    uint8_t *buffer;
////    uint8_t maxSize;
////    uint8_t cursor;
////
////public:
////    addData(uint8_t size) {
////        buffer = new uint8_t[size];
////        maxSize = size;
////        reset();
////    }
////
////    void reset(void) {
////        cursor = 0;
////    }
////
////    uint8_t getSize(void) {
////        return cursor;
////    }
////
////    uint8_t *getBuffer(void) {
////        return buffer;
////    }
////
////    // function to add a bit to the payload
////    void addBit(bool value) {
////        if (cursor + 1 <= maxSize) {
////            buffer[cursor++] = static_cast<uint8_t>(value);
////        }
////    }
////
////    // function to add a byte to the payload
////    void addByte(uint8_t value) {
////        if (cursor + 1 <= maxSize) {
////            buffer[cursor++] = value;
////        }
////    }
////
////    // function to add a 16-bit word to the payload
////    void addWord(uint16_t value) {
////        if (cursor + 2 <= maxSize) {
////            buffer[cursor++] = (value >> 8) & 0xFF;
////            buffer[cursor++] = value & 0xFF;
////        }
////    }
////
////    // function to add a 32-bit word to the payload
////    void addLong(uint32_t value) {
////        if (cursor + 4 <= maxSize) {
////            buffer[cursor++] = (value >> 24) & 0xFF;
////            buffer[cursor++] = (value >> 16) & 0xFF;
////            buffer[cursor++] = (value >> 8) & 0xFF;
////            buffer[cursor++] = value & 0xFF;
////        }
////    }
////
////    // function to add a float to the payload
////    void addFloat(float value) {
////        if (cursor + 4 <= maxSize) {
////            uint32_t intValue = *(reinterpret_cast<uint32_t*>(&value));
////            addLong(intValue);
////        }
////    }
////};
