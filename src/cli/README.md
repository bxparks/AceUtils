# Command Line Interface (CLI)

These classes implement a non-blocking command line interface on the Serial
port. In ther words, you can implement a primitive "shell" for the Arduino.

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
   that you would like to suport.
1. Create a `CommandManager` object, giving it the `CommandHandler*` array,
   and a number of size parameters for various internal buffers (maximum line
   buffer length, and maximum number of `argv` parameters for a command).
1. Insert the `CommandManager` into the `CoroutineScheduler` by calling
   `commandManager.setupCoroutine()` just before `CoroutineScheduler::setup()`.
1. Run the `CoroutineScheduler::loop()` in the global `loop()` method to
   run the `CommandManager` as a coroutine.

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

### CommandManager

The `CommandManager` is a templatized convenience class that creates all the
helper objects and buffers needed to read and parse the command line input.
It includes:

* a `StreamLineReader` coroutine that reads the input lines from `Serial`
* a `CommandDispatcher` coroutine that parses the input lines
* a `Channel<InputLine>` from `StreamLineReader` to `CommandDispatcher`
* a line buffer for the input lines
* a array of `(const char*)` to hold the command line arguments of the command

You don't have to use the `CommandManager`, but it greatly simplies the creation
and usage of the `CommandDispatcher`.

### CommandHandler Definitions and Setup

An Arduino `.ino` file that uses the CLI classes to implement a commmand line
shell will look something like this:

```C++
#include <AceUtilsCli.h>

using namespace ace_utils::cli;

class FooCommand: public CommandHandler {
  FooCommand():
    CommandHandler("{fooName}", "{helpString}") {}

  virtual void run(Print& printer, int argc, const char* const* argv)
      const override {
    ...
  }
};

class BarCommand: public CommandHandler {
  BarCommand():
    CommandHandler(F("{barCommand}"), F("{helpString}")) {}

  virtual void run(Print& printer, int argc, const char* const* argv)
      const override {
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

CommandManager<BUF_SIZE, ARGV_SIZE> commandManager(
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

  virtual void run(Print& printer, int argc, const char* const* argv)
      const override {
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
