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

#ifndef ACE_UTILS_CLI_STREAM_MANAGER_H
#define ACE_UTILS_CLI_STREAM_MANAGER_H

#include <AceRoutine.h> // Coroutine
#include "StreamDispatcher.h"

namespace ace_utils {
namespace cli {

/**
 * A convenience wrapper around a StreamDispatcher that hides complexity of
 * creating, initializing and injecting the resources needed by the
 * StreamDispatcher. It is not strictly necessary to use this, but the setup
 * is much easier using this class.
 *
 * This is a subclass of Coroutine, just like StreamDispatcher. The
 * runCoroutine() method simply delegates to the underlying StreamDispatcher,
 * so this class can be used as a substitute for StreamDispatcher.
 *
 * Example usage:
 *
 * @code
 * class CommandA: public CommandHandler {
 *   ...
 * };
 * class CommandB: public CommandHandler {
 *   ...
 * };
 *
 * CommandA commandA;
 * CommandB commandB;
 *
 * static const CommandHandler* COMMANDS[] = {
 *
 * };
 *
 * const uint8_t BUF_SIZE = 64;
 * const uint8_t ARGV_SIZE = 5;
 * const char PROMPT[] = "$ ";
 *
 * StreamManager<BUF_SIZE, ARGV_SIZE> commandManager(
 *     COMMANDS, NUM_COMMANDS, Serial, PROMPT);
 *
 * void setup() {
 *   ...
 *   CoroutineScheduler::setup();
 * }
 * @endcode
 *
 * @param BUF_SIZE Size of the input line buffer.
 * @param ARGV_SIZE Size of the command line argv token list.
 */
template<uint8_t BUF_SIZE, uint8_t ARGV_SIZE>
class StreamManager: public ace_routine::Coroutine {
  public:

    /**
     * Constructor.
     *
     * @param stream The serial port used to read commands and send output,
     *        will normally be 'Serial', but can be set to something else.
     * @param commands Array of (CommandHandler*).
     * @param numCommands Number of commands in 'commands'.
     * @param prompt If not null, print a prompt and echo the command entered
     *        by the user. If null, don't print the prompt and don't echo the
     *        input from the user.
     */
    StreamManager(
        const CommandHandler* const* commands,
        uint8_t numCommands,
        Stream& stream,
        const char* prompt = nullptr
    ) :
        mCommands(commands),
        mNumCommands(numCommands),
        mPrompt(prompt),
        mCommandDispatcher(mCommands, mNumCommands, mArgv, ARGV_SIZE),
        mStreamDispatcher(stream, mCommandDispatcher, mLineBuffer, BUF_SIZE)
    {}

    /**
     * Main body of coroutine, dispatches to the underlying StreamDispatcher
     * and mChannelDispatcher.
     */
    int runCoroutine() override {
      mStreamDispatcher.runCoroutine();
      return 0;
    }

    /** Return the ChannelDispatcher. VisibleForTesting. */
    const StreamDispatcher& getDispatcher() const {
      return mStreamDispatcher;
    }

  private:
    const CommandHandler* const* const mCommands;
    uint8_t const mNumCommands;
    const char* const mPrompt;

    CommandDispatcher mCommandDispatcher;
    StreamDispatcher mStreamDispatcher;
    char mLineBuffer[BUF_SIZE];
    const char* mArgv[ARGV_SIZE];
};

} // cli
} // ace_utils

#endif
