/*
MIT License

Copyright (c) 2020 Brian T. Park

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef MEM_PRINT_MEM_PRINT_H
#define MEM_PRINT_MEM_PRINT_H

#include <stddef.h> // size_t
#include <Print.h>

namespace mem_print {

/**
 * Base class for all the MemPrint<SIZE> and MemPrintLarge<SIZE> classes. A
 * common base class reduces the code size because only one copy of the core
 * code is included across all possible template instances. Otherwise,
 * MemPrint<10> is a different class than MemPrint<20> and would pull in
 * duplicate copies of the code.
 *
 * Here are the actual numbers. I modified tests/CommonTest.ino program to use
 * 2 template instances, MemPrint<10> and MemPrint<20> instead of just
 * MemPrint<10>. Without this base class optimization, the sketch uses:
 *
 *    * 10030 bytes (32%) of program storage space,
 *    * 710 bytes (34%) of dynamic memory on an Arduino Nano.
 *
 * After inserting this MemPrintBase class in the class hierarchy, the same
 * sketch uses:
 *
 *    * 9936 bytes (32%) of program storage space,
 *    * 698 bytes (34%) of dynamic memory,
 *
 * So we save 94 bytes of flash memory, and 12 bytes of static RAM. And even
 * better, the program and RAM size was the same as using 2 MemPrint<10>
 * instances. In other words, the amount of flash and static RAM remains
 * constant no matter how many template instances we create.
 */
template<typename TYPE>
class MemPrintBase: public Print {
  public:
    size_t write(uint8_t c) override {
      if (index_ < size_ - 1) {
        buf_[index_] = c;
        index_++;
        return 1;
      } else {
        return 0;
      }
    }

    size_t write(const uint8_t *buf, size_t size) override {
      if (buf == nullptr) return 0;

      size_t n = 0;
      while (size-- > 0) {
        size_t ret = write(*buf++);
        if (ret == 0) break;
        n++;
      }
      return n;
    }

// ESP32 version of Print class does not define a virtual flush() method so
// we can't use the 'override' keyword.
#ifdef ESP32
    void flush() {
      index_ = 0;
    }
#else
    void flush() override {
      index_ = 0;
    }
#endif

    /**
     * Return the NUL terminated c-string buffer. After the buffer is no longer
     * needed, the flush() method should be called to reset the internal buffer
     * index to 0.
     */
    const char* getCstr() const {
      buf_[index_] = '\0';
      return buf_;
    }

  protected:
    MemPrintBase(TYPE size, char* buf):
        size_(size),
        buf_(buf) {}

  private:
    TYPE const size_;
    TYPE index_ = 0;

    // This is the pointer to actualBuf_ which is defined in the subclasses.
    // Instead of storing it (and taking up precious RAM), maybe there's a way
    // to calculate this pointer by simply extending the pointer from the last
    // element of this object (i.e. &index_), and thereby saving some memory.
    // But I am not convinced that I have the knowledge do that properly
    // without triggering UB (undefined behavior) in the C++ language. Do I
    // define buf_[0] here and will it point exactly where actualBuf_[] is
    // allocated? Or do I use [&MemPrintBase + sizeof(MemPrintBase)] to
    // calculate the pointer to actualBuf_. I just don't know what's actually
    // allowed in the language spec versus something that works by pure luck on
    // a particular microcontroller and compiler. So I'll pay the cost of the 2
    // extra bytes (8-bit) or 4 extra bytes (32-bit processors) of RAM and
    // store the pointer to actualBuf_ explicitly here in the base class.
    char* const buf_;
};

/**
 * An implementation of `Print` that writes to an in-memory buffer supporting
 * string size less than 255. The NUL-terminated c-string representation can be
 * retrieved using `getCstr()`.
 *
 * This object is expected to be created on the stack. The desired information
 * will be written into it, and then extracted using the `getCstr()`. The
 * object will be destroyed automatically when the stack is unwound after
 * returning from the function where this is used. It is possible to create an
 * instance of this object statically and reused across different calling
 * sites, but the programmer must handle the memory management properly.
 *
 * Warning: The contents of `getCstr()` is valid only as long as the MemPrint
 * object is alive. It should never be passed to another part of the program if
 * the getCstr() pointer outlives its underlying MemPrint object.
 *
 * Usage:
 *
 * @verbatim
 * #include <AceUtils.h>
 *
 * using namespace mem_print;
 *
 * void someFunction() {
 *   MemPrint<32> memPrint;
 *   memPrint.print("hello, ");
 *   memPrint.println("world!");
 *   const char* cstr = memPrint.getCstr();
 *
 *   // do stuff with cstr
 *   // ...
 *
 *   memPrint.flush(); // needed only if this will be used again
 * }
 * @endverbatim
 *
 * @tparam SIZE size of internal string buffer including the NUL terminator
 *         character, the maximum is 255, which means a maximum string length of
 *         254 characters.
 */
template <uint8_t SIZE>
class MemPrint: public MemPrintBase<uint8_t> {
  public:
    MemPrint(): MemPrintBase<uint8_t>(SIZE, actualBuf_) {}

  private:
    char actualBuf_[SIZE];
};

/**
 * A version of MemPrint that uses `uint16_t` to keep its internal size
 * parameters, so that a maximum of 65534 character can be stored.
 *
 * @tparam SIZE size of internal string buffer including the NUL terminator
 *         character, the maximum is 65535, which means a maximum string length
 *         of 65534 characters.
 */
template <uint16_t SIZE>
class MemPrintLarge: public MemPrintBase<uint16_t> {
  public:
    MemPrintLarge(): MemPrintBase<uint16_t>(SIZE, actualBuf_) {}

  private:
    char actualBuf_[SIZE];
};

}

#endif
