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

#ifndef ACE_UTILS_CLI_STREAM_PROCESSOR_MANAGER_H
#define ACE_UTILS_CLI_STREAM_PROCESSOR_MANAGER_H

#include <AceRoutine.h> // Coroutine
#include "StreamProcessorCoroutine.h"

namespace ace_utils {
namespace cli {

/**
 * A convenience wrapper around a StreamProcessorCoroutine that hides complexity
 * of creating, initializing and injecting the resources needed by the
 * StreamProcessorCoroutine. It is not strictly necessary to use this, but the
 * setup is much easier using this class.
 *
 * The StreamProcessorCoroutine auto-registers itself into the
 * CoroutineScheduler. If you aren't using the CoroutineScheduler, then you need
 * to explicitly obtain the reference to StreamProcessorCoroutine using
 * getStreamProcessor() and call the StreamProcessorCoroutine::runCoroutine()
 * method manually from the global loop() function.
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
 *   &commandA,
 *   &commandB
 * };
 * static const uint8_t NUM_COMMANDS =
 *    sizeof(COMMANDS) / sizeof(CommandHandler*);
 *
 * const uint8_t BUF_SIZE = 64;
 * const uint8_t ARGV_SIZE = 5;
 * const char PROMPT[] = "$ ";
 *
 * StreamProcessorManager<BUF_SIZE, ARGV_SIZE> commandManager(
 *     Serial, COMMANDS, NUM_COMMANDS, Serial, PROMPT);
 *
 * void setup() {
 *   ...
 *   CoroutineScheduler::setup();
 * }
 *
 * void loop() {
 *   CoroutineScheduler::loop();
 * }
 * @endcode
 *
 * @param BUF_SIZE Size of the input line buffer.
 * @param ARGV_SIZE Size of the command line argv token list.
 */
template<uint8_t BUF_SIZE, uint8_t ARGV_SIZE>
class StreamProcessorManager {
  public:

    /**
     * Constructor.
     *
     * @param stream The serial port used to read commands and send output,
     *        will normally be 'Serial', but can be set to something else.
     * @param commands Array of (CommandHandler*).
     * @param numCommands Number of commands in 'commands'.
     * @param printer output stream, often the same as `stream` but not always
     * @param prompt If not null, print a prompt and echo the command entered
     *        by the user. If null, don't print the prompt and don't echo the
     *        input from the user. (not implemented yet)
     */
    StreamProcessorManager(
        Stream& stream,
        const CommandHandler* const* commands,
        uint8_t numCommands,
        Print& printer,
        const char* prompt = nullptr
    ) :
        mCommandDispatcher(commands, numCommands, mArgv, ARGV_SIZE),
        mStreamProcessor(
            stream, mCommandDispatcher, printer, mLineBuffer, BUF_SIZE, prompt)
    {}

    /** Return the underlying StreamProcessorCoroutine. */
    StreamProcessorCoroutine& getStreamProcessor() {
      return mStreamProcessor;
    }

  private:
    // Disable copy-constructor and assignment operator
    StreamProcessorManager(const StreamProcessorManager&) = delete;
    StreamProcessorManager& operator=(const StreamProcessorManager&) = delete;

  private:
    CommandDispatcher mCommandDispatcher;
    StreamProcessorCoroutine mStreamProcessor;
    char mLineBuffer[BUF_SIZE];
    const char* mArgv[ARGV_SIZE];
};

} // cli
} // ace_utils

#endif
