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

#ifndef ACE_UTILS_CLI_CHANNEL_PROCESSOR_COROUTINE_H
#define ACE_UTILS_CLI_CHANNEL_PROCESSOR_COROUTINE_H

#include <AceRoutine.h> // Coroutine, Channel
#include "InputLine.h"

class Print;
class __FlashStringHelper;

namespace ace_utils {
namespace cli {

class CommandDispatcher;

/**
 * A coroutine that reads lines from a Channel (e.g. written by
 * StreamReaderCoroutine), then sends the command to the CommandDispatcher for
 * processing.
 */
class ChannelProcessorCoroutine: public ace_routine::Coroutine {
  public:
    /**
     * Constructor.
     *
     * @param channel A channel from the StringLineReader to this coroutine.
     * @param printer The output object, normally the global Serial object.
     * @param commandDispatcher instance of CommandDispatcher
     * @param prompt Print this prompt just before accepting character inputs.
     *        If null, don't print the prompt.
     */
    ChannelProcessorCoroutine(
        ace_routine::Channel<InputLine>& channel,
        const CommandDispatcher& commandDispatcher,
        Print& printer,
        const char* prompt
    ) :
        mChannel(channel),
        mCommandDispatcher(commandDispatcher),
        mPrinter(printer),
        mPrompt(prompt)
    {}

    int runCoroutine() override;

    /** Return the ChannelProcessorCoroutine. VisibleForTesting. */
    const CommandDispatcher& getDispatcher() const {
      return mCommandDispatcher;
    }

  private:
    // Disable copy-constructor and assignment operator
    ChannelProcessorCoroutine(const ChannelProcessorCoroutine&) = delete;
    ChannelProcessorCoroutine& operator=(const ChannelProcessorCoroutine&) =
        delete;

    void printLineError(const char* line, uint8_t statusCode) const;

  private:
    ace_routine::Channel<InputLine>& mChannel;
    const CommandDispatcher& mCommandDispatcher;
    Print& mPrinter;
    const char* const mPrompt;
};

} // cli
} // ace_utils

#endif
