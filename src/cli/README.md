# Command Line Interface (CLI)

These classes implement a non-blocking command line interface on the Serial
port. In other words, you can implement a primitive "shell" for the Arduino.

These classes were initially an experiment to validate the `AceRoutine` macros
and classes but they seem to be useful as an independent library. They may be
moved to a separate project/repository later.

Version: (2019-07-23)

## Usage

The basic steps for adding a command line interface to an Arduino sketch
using the `cli/` library is the following:

1. Create a `CommandHandler` class for each command, defining its
   `name` and `helpString`.
1. Create a static array of `CommandHandler*` pointers with all the commands
   that you would like to support.
1. Create a `StreamChannelManager` object, giving it the `CommandHandler*` array,
   and a number of size parameters for various internal buffers (maximum line
   buffer length, and maximum number of `argv` parameters for a command).
1. Call `CoroutineScheduler::setup()` in the global `setup()` function.
1. Run the `CoroutineScheduler::loop()` in the global `loop()` function to
   run the `StreamChannelManager` as a coroutine.

The dependency diagram looks like this:

```
        StreamChannelManager
          /     |       \
    ------      v        v
   /   ChannelDispatcher StreamLineReader
   |        |         \        |
   |        |          ---\    |
   v        v              v   v
CommandDispatcher       InputLine
       |
       v
  CommandHandler
```

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

### StreamChannelManager

The `StreamChannelManager` is a templatized convenience class that creates all the
helper objects and buffers needed to read and parse the command line input.
It includes:

* a `StreamLineReader` coroutine that reads the input lines from `Serial`
* a `ChannelDispatcher` coroutine that parses the input lines
* a `Channel<InputLine>` from `StreamLineReader` to `ChannelDispatcher`
* a `CommandDispatcher` instance that knows how to tokenize a string line
  and call the matching `CommandHandler`
* a line buffer for each input line
* a array of `(const char*)` to hold the command line arguments of the command

You don't have to use the `StreamChannelManager`, but it greatly simplifies the
creation and usage of the `ChannelDispatcher`.

### CommandHandler Definitions and Setup

An Arduino `.ino` file that uses the CLI classes to implement a command line
shell will look something like this:

```C++
#include <AceUtils.h>
#include <cli/cli.h> // CommandHandler from AceUtils

using ace_utils::cli::CommandHandler;
using ace_utils::cli::StreamChannelManager;

class FooCommand: public CommandHandler {
  FooCommand():
    CommandHandler("{fooName}", "{helpString}") {}

  void run(Print& printer, int argc, const char* const* argv) const override {
    ...
  }
};

class BarCommand: public CommandHandler {
  BarCommand():
    CommandHandler(F("{barCommand}"), F("{helpString}")) {}

  void run(Print& printer, int argc, const char* const* argv) const override {
    ...
  }
};

FooCommand fooCommand;
BarCommand barCommand;

static const CommandHandler* const COMMANDS[] = {
  &fooCommand,
  &barCommand,
};
static uint8_t const NUM_COMMANDS = sizeof(COMMANDS) / sizeof(CommandHandler*);

uint8_t const BUF_SIZE = 64; // maximum size of an input line
uint8_t const ARGV_SIZE = 10; // maximum number of tokens in command
char const PROMPT[] = "$ ";

StreamChannelManager<BUF_SIZE, ARGV_SIZE> commandManager(
    COMMANDS, NUM_COMMANDS, Serial, PROMPT);

void setup() {
  ...
  commandManager.setupCoroutine("commandManager");
  CoroutineScheduler::setup();
}

void loop() {
  CoroutineScheduler::loop();
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

See [examples/CommandLineShell/](../../examples/CommandLineShell/)
for an demo program that implements 5 commands:

* `help [command]`
* `list`
* `free`
* `echo [args ...]`
* `delay [(on | off) millis]`
