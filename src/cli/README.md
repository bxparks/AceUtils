# Command Line Interface (CLI)

These classes implement a non-blocking command line interface on the Serial
port. In other words, you can implement a primitive "shell" for the Arduino.

These classes were initially an experiment to validate the `AceRoutine` macros
and classes but they seem to be useful as an independent library. They may be
moved to a separate project/repository later.

Version: (2021-07-19)

## Usage

### DirectProcessorManager

This class is a thin wrapper around a `DirectProcessor` and a
`CommandDispatcher. The `Stream` input is scanned using a non-blocking loop
without using coroutines. This is the simplest option if you do not want to
depend on the AceRoutine library.

The basic steps for adding a command line interface to an Arduino sketch
using the `cli/` library is the following:

1. Create a `CommandHandler` class for each command, defining its
   `name` and `helpString`.
1. Create a static array of `CommandHandler*` pointers with all the commands
   that you would like to support.
1. Create a `DirectProcessorManager` object, giving it the `CommandHandler*`
   array, and a number of size parameters for various internal buffers (maximum
   line buffer length, and maximum number of `argv` parameters for a command).
1. Call `CoroutineScheduler::setup()` in the global `setup()` function.
1. Run the `CoroutineScheduler::loop()` in the global `loop()` function to
   run the `StreamProcessorCoroutine` that was created inside the
   `StreamProcessorManager`.

The dependency diagram looks like this:

```
DirectProcessorManager
      |
      v
 DirectProcessor
      |
      v
 CommandDispatcher
      |
      v
 CommandHandler
```

The `StreamProcessorManager` is a templatized convenience class that creates
all the helper objects and buffers needed to read and parse the command line
input. It includes:

* a `StreamProcessorCoroutine` coroutine that reads lines from the given
  `Stream` object.
* a `CommandDispatcher` instance that knows how to tokenize a string line
  and call the matching `CommandHandler`
* a line buffer for each input line
* a array of `(const char*)` to hold the command line arguments of the command

You don't have to use the `StreamProcessorManager`, but it greatly simplifies
the creation and usage of the `StreamProcessorCoroutine`.

### StreamProcessorManager

This class is a thin wrapper around a `StreamProcessoCoroutine` and a
`CommandDispatcher. The `Stream` input is scanned using an AceRoutine coroutine.
This may be an option if your application already uses AceRoutine and you want
everything handled as a coroutine.

The basic steps for adding a command line interface to an Arduino sketch
using the `cli/` library is the following:

1. Create a `CommandHandler` class for each command, defining its
   `name` and `helpString`.
1. Create a static array of `CommandHandler*` pointers with all the commands
   that you would like to support.
1. Create a `StreamProcessorManager` object, giving it the `CommandHandler*`
   array, and a number of size parameters for various internal buffers (maximum
   line buffer length, and maximum number of `argv` parameters for a command).
1. Call `CoroutineScheduler::setup()` in the global `setup()` function.
1. Run the `CoroutineScheduler::loop()` in the global `loop()` function to
   run the `StreamProcessorCoroutine` that was created inside the
   `StreamProcessorManager`.

The dependency diagram looks like this:

```
 StreamProcessorManager
       |
       v
StreamProcessorCoroutine
       |
       v
  CommandDispatcher
       |
       v
  CommandHandler
```

The `StreamProcessorManager` is a templatized convenience class that creates
all the helper objects and buffers needed to read and parse the command line
input. It includes:

* a `StreamProcessorCoroutine` coroutine that reads lines from the given
  `Stream` object.
* a `CommandDispatcher` instance that knows how to tokenize a string line
  and call the matching `CommandHandler`
* a line buffer for each input line
* a array of `(const char*)` to hold the command line arguments of the command

You don't have to use the `StreamProcessorManager`, but it greatly simplifies
the creation and usage of the `StreamProcessorCoroutine`.

### ChannelProcessorManager

**Deprecated**: This uses the experimental `ace_routine::Channel` class to allow
passing the input string from the `StreamReaderCoroutine` to the
`ChannelProcessorCoroutine`. It turns out that the direct approach of
`StreamProcessorCoroutine` (above) is simpler with the same functionality,
without using the `Channel` class.

The basic steps for adding a command line interface to an Arduino sketch
using the `cli/` library is the following:

1. Create a `CommandHandler` class for each command, defining its
   `name` and `helpString`.
1. Create a static array of `CommandHandler*` pointers with all the commands
   that you would like to support.
1. Create a `ChannelProcessorManager` object, giving it the `CommandHandler*`
   array, and a number of size parameters for various internal buffers (maximum
   line buffer length, and maximum number of `argv` parameters for a command).
1. Call `CoroutineScheduler::setup()` in the global `setup()` function.
1. Run the `CoroutineScheduler::loop()` in the global `loop()` function to
   run the `ChannelProcessorCoroutine` and `StreamReaderCoroutine`
   which were created inside the `ChannelProcessorManager`.

The dependency diagram looks like this:

```
        ChannelProcessorManager
          /     |       \
    ------      v        v
   /   ChannelProcessor  StreamReader
        Coroutine         Coroutine
   |        |      \          |
   |        |       ---\      |
   v        v           v     v
CommandDispatcher       InputLine
       |
       v
  CommandHandler
```

The `ChannelProcessorManager` is a templatized convenience class that creates
all the helper objects and buffers needed to read and parse the command line
input. It includes:

* a `StreamReaderCoroutine` coroutine that reads the input lines from `Serial`
* a `ChannelProcessorCoroutine` coroutine that parses the input lines
  inside `Channel<InputLine>`, from `StreamReaderCoroutine` to
  `ChannelProcessorCoroutine`
* a `CommandDispatcher` instance that knows how to tokenize a string line
  and call the matching `CommandHandler`
* a line buffer for each input line
* a array of `(const char*)` to hold the command line arguments of the command

You don't have to use the `ChannelProcessorManager`, but it greatly simplifies
the creation and usage of the `ChannelProcessorCoroutine`.

### Command Line Over MQTT

(TBD: Add documentation or example of a command line shell over MQTT messages.)

### Command Handler and Arguments

The `CommandHandler` class defines the command's `name`, `helpString` and
the `run()` method that implements the command. It takes the following
parameters:

* `printer` is the output device, which will normally be the global `Serial`
  object
* `argc` is the number of `argv` arguments
* `argv` is the array of `cont char*` pointers, each pointing to the words
  of the command line delimited by whitespaces. These are identical to
  the `argc` and `argv` parameters passed to the C-language `main(argc, argv)`
  function. For example, `argv[0]` is the name of the command, and `argv[1]`
  is the first argument after the command (if it exists).

### CommandHandler Definitions and Setup

An Arduino `.ino` file that uses the CLI classes to implement a command line
shell will look something like this:

```C++
#include <AceUtils.h>
#include <cli/cli.h> // CommandHandler from AceUtils

using ace_utils::cli::CommandHandler;
using ace_utils::cli::ChannelProcessorManager;
using ace_utils::cli::StreamProcessorManager;

class FooCommand: public CommandHandler {
  FooCommand():
    CommandHandler("{fooName}", "{helpString}")
  {}

  void run(Print& printer, int argc, const char* const* argv) const override {
    ...
  }
};

class BarCommand: public CommandHandler {
  BarCommand():
    CommandHandler(F("{barCommand}"), F("{helpString}"))
  {}

  void run(Print& printer, int argc, const char* const* argv) const override {
    ...
  }
};

// Instantiate each command
FooCommand fooCommand;
BarCommand barCommand;

// Create an array of command handlers.
static const CommandHandler* const COMMANDS[] = {
  &fooCommand,
  &barCommand,
};
static uint8_t const NUM_COMMANDS = sizeof(COMMANDS) / sizeof(CommandHandler*);

uint8_t const BUF_SIZE = 64; // maximum size of an input line
uint8_t const ARGV_SIZE = 10; // maximum number of tokens in command
char const PROMPT[] = "$ ";

// Create a command manager, using one of the following:
//
// 1) Simplest, direct polling of the Serial port.
DirectProcessorManager<BUF_SIZE, ARGV_SIZE> commandManager(
    Serial, COMMANDS, NUM_COMMANDS, Serial, PROMPT);
//
// 2) Poll Serial port using a coroutine:
//StreamProcessorManager<BUF_SIZE, ARGV_SIZE> commandManager(
//    Serial, COMMANDS, NUM_COMMANDS, Serial, PROMPT);
//
// 3) Deprecated. Poll the Serial port using 2 coroutines and a Channel.
//ChannelProcessorManager<BUF_SIZE, ARGV_SIZE> commandManager(
//    Serial, COMMANDS, NUM_COMMANDS, Serial, PROMPT);

void setup() {
  ...
  CoroutineScheduler::setup();
}

void loop() {
  CoroutineScheduler::loop();
  ...
}
```

### Argc and Argv Parsing

Within the `CommandHandler`, there are several helper routines which are useful
for processing the `argc` and `argv` arguments:

* `shiftArgcArgv(argc, argv)` shifts the input tokens by 1 token to the left,
  by incrementing `argv` and decrementing `argc`
* `bool isArgEqual(const char*, const char*);`
* `bool isArgEqual(const char*, const __FlashHelperString*);`

Here is the sketch of the Command that will parse a command whose syntax is
`delay [(on | off) {millis}]`, where the `on {millis}` and `off {millis}`
arguments to the `delay` command are optional:

```C++
class DelayCommand: public CommandHandler {
  DelayCommand():
    CommandHandler(F("delay"), F("[(on | off) {millis}")) {}

  void run(Print& printer, int argc, const char* const* argv) const override {
    if (argc == 1) {
      printer.println(F("'delay' typed with no arguments"));
      return;
    }

    if (argc != 3) {
      printer.println(F("Incorrect number of arguments to 'delay' command"));
      return;
    }

    shiftArgcArgv(argc, argv);
    if (isArgEqual(argv[0], "on")) {
      int delay = atoi(argv[1]));
      printer.print(F("Executing: delay on "));
      printer.println(delay);
    } else if (isArgEqual(argv[0], F("off"))) {
      int delay = atoi(argv[1]));
      printer.print(F("Executing: delay off "));
      printer.println(delay);
    } else {
      printer.println(F("Unknown argument to 'delay'"));
    }
  }
};
```

## Example

Each of the following examples implements 5 commands:
* [examples/ChannelCommandLineShell/](../../examples/ChannelCommandLineShell/)
* [examples/StreamCommandLineShell/](../../examples/StreamCommandLineShell/)
* [examples/DirectCommandLineShell/](../../examples/DirectCommandLineShell/)

All 3 implement the following commands:

* `help [command]`
* `list`
* `free`
* `echo [args ...]`
* `delay [(on | off) millis]`

