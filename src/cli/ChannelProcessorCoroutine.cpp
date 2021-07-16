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

#include <Arduino.h> // Print
#include "ChannelProcessorCoroutine.h"
#include "CommandDispatcher.h"

namespace ace_utils {
namespace cli {

int ChannelProcessorCoroutine::runCoroutine() {
  InputLine input;
  COROUTINE_LOOP() {
    if (mPrompt != nullptr) {
      mPrinter.print(mPrompt);
    }
    COROUTINE_CHANNEL_READ(mChannel, input);

    if (input.status == InputLine::kStatusOverflow) {
      printLineError(input.line, input.status);
      continue;
    }

    if (mPrompt != nullptr) {
      mPrinter.print(input.line); // line includes the \n
    }
    mCommandDispatcher.runCommand(mPrinter, input.line);
  }
}

void ChannelProcessorCoroutine::printLineError(
    const char* line, uint8_t statusCode) const {
  if (statusCode == InputLine::kStatusOverflow) {
    mPrinter.print(F("BufferOverflow: "));
    mPrinter.println(line);
  } else {
    mPrinter.print(F("UnknownError: "));
    mPrinter.print(statusCode);
    mPrinter.print(F(": "));
    mPrinter.println(line);
  }
}

}
}
