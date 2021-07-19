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

#ifndef ACE_UTILS_CLI_STREAM_PROCESSOR_COROUTINE_H
#define ACE_UTILS_CLI_STREAM_PROCESSOR_COROUTINE_H

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
class StreamProcessorCoroutine : public ace_routine::Coroutine {
  private:
    static uint8_t const kBufferOk = 0;
    static uint8_t const kBufferOverflow = 1;

  public:
    /**
     * Constructor.
     *
     * @param stream input stream, usually the global Serial object
     * @param commandDispatcher CommandDispatcher for processing the command
     * @param printer output stream, often the same as `stream` but not always
     * @param buffer input character buffer
     * @param bufferSize size of the buffer. Should be set large enough to
     *        hold the longest expected line without triggering buffer overflow.
     * @param prompt Print this prompt just before accepting character inputs.
     *        If null, don't print the prompt.
     */
    StreamProcessorCoroutine(
        Stream& stream,
        const CommandDispatcher& commandDispatcher,
        Print& printer,
        char* buffer,
        uint8_t bufferSize,
        const char* prompt = nullptr
    ):
        mCommandDispatcher(commandDispatcher),
        mStream(stream),
        mPrinter(printer),
        mBuf(buffer),
        mBufSize(bufferSize),
        mPrompt(prompt)
    {}

    /**
     * Main body of the coroutine that reads a character from the input stream
     * and writes it into the output channel.
     */
    int runCoroutine() override {
      COROUTINE_LOOP() {
        if (mPrompt && mShouldPrompt) {
          mPrinter.print(mPrompt);
          mPrinter.flush();
          mShouldPrompt = false;
        }
        COROUTINE_AWAIT(mStream.available() > 0);

        // There could be multiple bytes waiting, so loop to get all of them.
        while (mStream.available() > 0) {
          char c = mStream.read();
          mBuf[mIndex] = c;
          mIndex++;

          if (mIndex >= mBufSize - 1) {
            resetBuffer();
            mPrinter.println(
                F("Error: Buffer overflow... flushing until Newline"));
            mBufStatus = kBufferOverflow;
          } else if (c == '\n' || c == '\r') {
            resetBuffer();
            mShouldPrompt = true;

            if (mBufStatus == kBufferOk) {
              mCommandDispatcher.runCommand(mPrinter, mBuf);
            } else {
              // If the buffer had previously overflown, then flush all input
              // until the \n or \r.
              mPrinter.println(
                  F("Error: Buffer overflow... flushed after Newline"));
              mBufStatus = kBufferOk;
            }
          }
        }
      }
    }

  private:
    // Disable copy-constructor and assignment operator
    StreamProcessorCoroutine(const StreamProcessorCoroutine&) = delete;
    StreamProcessorCoroutine& operator=(const StreamProcessorCoroutine&) =
        delete;

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
    Print& mPrinter;
    char* const mBuf;
    uint8_t const mBufSize;
    const char* const mPrompt;

    uint8_t mIndex = 0;
    uint8_t mBufStatus = kBufferOk;
    bool mShouldPrompt = true;
};

} // cli
} // ace_utils

#endif
