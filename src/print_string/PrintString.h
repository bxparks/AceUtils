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

#ifndef PRINT_STRING_PRINT_STRING_H
#define PRINT_STRING_PRINT_STRING_H

#include <stddef.h> // size_t
#include <Print.h>

namespace print_string {

/**
 * Base class for all template instances of the PrintString<SIZE> class. A
 * common base class reduces the code size because only one copy of the core
 * code is included across all possible template instances. Otherwise,
 * PrintString<10> is a different class than PrintString<20> and would pull in
 * duplicate copies of the code.
 *
 * Usually the `Print` base class can be used to accept instances of the
 * `PrintString<SIZE>` objects. However, if you need access to the `lenth()`
 * method, then you need to use the `PrintStringBase` class instead, since the
 * `Print` class does not have a `length()` method.
 *
 * Here are the actual numbers. I modified tests/CommonTest.ino program to use
 * 2 template instances, PrintString<10> and PrintString<20> instead of just
 * PrintString<10>. Without this base class optimization, the sketch uses:
 *
 *    * 10030 bytes (32%) of program storage space,
 *    * 710 bytes (34%) of dynamic memory on an Arduino Nano.
 *
 * After inserting this PrintStringBase class in the class hierarchy, the same
 * sketch uses:
 *
 *    * 9936 bytes (32%) of program storage space,
 *    * 698 bytes (34%) of dynamic memory,
 *
 * So we save 94 bytes of flash memory, and 12 bytes of static RAM. And even
 * better, the program and RAM size was the same as using 2 PrintString<10>
 * instances. In other words, the amount of flash and static RAM remains
 * constant no matter how many template instances we create.
 */
class PrintStringBase: public Print {
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

    /** Return the length of the internal c-string buffer. */
    size_t length() const {
      return index_;
    }

  protected:
    PrintStringBase(uint16_t size, char* buf):
        size_(size),
        buf_(buf) {}

  private:
    uint16_t const size_;
    uint16_t index_ = 0;

    // This is the pointer to actualBuf_ which is defined in the subclasses.
    // Instead of storing it (and taking up precious RAM), maybe there's a way
    // to calculate this pointer by simply extending the pointer from the last
    // element of this object (i.e. &index_), and thereby saving some memory.
    // But I am not convinced that I have the knowledge do that properly
    // without triggering UB (undefined behavior) in the C++ language. Do I
    // define buf_[0] here and will it point exactly where actualBuf_[] is
    // allocated? Or do I use [&PrintStringBase + sizeof(PrintStringBase)] to
    // calculate the pointer to actualBuf_. I just don't know what's actually
    // allowed in the language spec versus something that works by pure luck on
    // a particular microcontroller and compiler. So I'll pay the cost of the 2
    // extra bytes (8-bit) or 4 extra bytes (32-bit processors) of RAM and
    // store the pointer to actualBuf_ explicitly here in the base class.
    char* const buf_;
};

/**
 * An implementation of `Print` that writes to an in-memory buffer supporting
 * strings less than 65535 in length. It is intended to be an alternative to
 * the `String` class to help avoid heap fragmentation due to repeated creation
 * and deletion of small String objects. The 'PrintString' object inherit the
 * methods from the 'Print' interface which can be used to build an internal
 * string representation of various objects. Instead of using the
 * `operator+=()` or the `concat()` method, use the `print()`, `println()` (or
 * sometimes the `printf()` method) of the `Print` class. After the internal
 * string is built, the NUL-terminated c-string representation can be retrieved
 * using `getCstr()`.
 *
 * This object is expected to be created on the stack instead of the heap
 * to avoid heap fragmentation. The `SIZE` parameter is a compile time constant,
 * to allow the internal string buffer to be created on the stack. The object
 * will be destroyed automatically when the stack is unwound after returning
 * from the function where this is used.
 *
 * An instance of `PrintString` can be reused by calling the `flush()` method
 * which causes the internal buffer to be cleared to an empty string. An
 * instance of this object could be created statically and reused across
 * different calling sites, but this was not the main intended use of this
 * class.
 *
 * Warning: The contents of `getCstr()` are valid only as long as the
 * PrintString object is alive. The pointer should never be passed to another
 * part of the program if the PrintString object is destroyed before the
 * pointer is used.
 *
 * Usage:
 *
 * @verbatim
 * #include <print_string.h>
 *
 * using namespace print_string;
 *
 * void fillStringA(PrintString& message) {
 *   message.print("hello, ");
 *   message.println("world!");
 * }
 *
 * void fillStringB(PrintString& message) {
 *   message.print("There are ");
 *   message.print(42);
 *   message.println(" apples");
 * }
 *
 * void someFunction() {
 *   PrintString<32> message;
 *   fillStringA(message)
 *   const char* cstr = message.getCstr();
 *   // do stuff with cstr
 *
 *   message.flush();
 *   fillStringB(message);
 *   cstr = message.getCstr();
 *   // do more stuff with cstr
 * }
 * @endverbatim
 *
 * @tparam SIZE size of internal string buffer including the NUL terminator
 *         character, the maximum is 65535, which means the maximum string
 *         length is 65534 characters.
 */
template <uint16_t SIZE>
class PrintString: public PrintStringBase {
  public:
    PrintString(): PrintStringBase(SIZE, actualBuf_) {}

  private:
    char actualBuf_[SIZE];
};

}

#endif
