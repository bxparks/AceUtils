/*
MIT License

Copyright (c) 2021 Brian T. Park

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

#ifndef ACE_UTILS_CLI_STREAM_DISPATCHER_H
#define ACE_UTILS_CLI_STREAM_DISPATCHER_H

#include <Arduino.h> // Stream, Print
#include <AceRoutine.h>

namespace ace_utils {
namespace cli {

/**
 * An AceRoutine coroutine that reads lines (terminated by '\\n' or '\\r' from
 * the Stream device, and calls CommandDispatcher directly to execute the
 * command. The Stream will normally be the global Serial object. If the input
 * line is too long to overflow the line buffer, the rest of the line is flushed
 * until the next '\\n' or '\\r', and an error message is printed on the given
 * Print output.
 */
class StreamDispatcher: public ace_routine::Coroutine {
  private:
    static uint8_t const kBufferOk = 0;
    static uint8_t const kBufferOverflow = 1;

  public:
    /**
     * Constructor.
     *
     * @param stream input stream, usually the global Serial object
     * @param commandDispatcher CommandDispatcher for processing the command
     * @param buffer input character buffer
     * @param bufferSize size of the buffer. Should be set large enough to
     *        hold the longest expected line without triggering buffer overflow.
     */
    StreamDispatcher(
        Stream& stream,
        const CommandDispatcher& commandDispatcher,
        char* buffer,
        uint8_t bufferSize
    ):
        mCommandDispatcher(commandDispatcher),
        mStream(stream),
        mBuf(buffer),
        mBufSize(bufferSize)
    {}

    /**
     * Main body of the coroutine that reads a character from the input stream
     * and writes it into the output channel.
     */
    int runCoroutine() override {

      COROUTINE_LOOP() {
        COROUTINE_AWAIT(mStream.available() > 0);

        // There may be multiple bytes waiting, so loop to get all of them.
        while (mStream.available() > 0) {
          char c = mStream.read();
          mBuf[mIndex] = c;
          mIndex++;

          if (mIndex >= mBufSize - 1) {
            resetBuffer();
            mStream.println(
                F("Error: Buffer overflow... flushing until Newline"));
            mBufStatus = kBufferOverflow;
          } else if (c == '\n' || c == '\r') {
            // If the buffer had previously overflown, then flush all input
            // until the \n or \r.
            resetBuffer();
            if (mBufStatus == kBufferOk) {
              mCommandDispatcher.runCommand(mStream, mBuf);
            } else {
              mStream.println(
                  F("Error: Buffer overflow... flushed after Newline"));
              mBufStatus = kBufferOk;
            }
          }
        }
      }
    }

  private:
    // Disable copy-constructor and assignment operator
    StreamDispatcher(const StreamDispatcher&) = delete;
    StreamDispatcher& operator=(const StreamDispatcher&) = delete;

    /**
     * Terminate the current buffer with the NUL character (so that the current
     * string can be retrieved), and reset the character index to the beginning
     * of the buffer to prepare for the next mStream.read();
     */
    void resetBuffer() {
      mBuf[mIndex] = '\0';
      mIndex = 0;
    }

  private:
    const CommandDispatcher& mCommandDispatcher;
    Stream& mStream;
    char* const mBuf;
    uint8_t const mBufSize;

    uint8_t mIndex = 0;
    uint8_t mBufStatus = kBufferOk;
};

} // cli
} // ace_utils

#endif
