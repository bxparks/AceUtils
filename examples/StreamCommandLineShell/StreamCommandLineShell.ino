/*
 * A demo sketch for the AceUtils/cli/cli.h classes. It reads the serial port
 * for a command, parses the command line, then calls the appropriate command
 * handler.
 *
 * Similar to examples/ChannelCommandLineShell and
 * examples/DirectCommandLineShell except that it reads the Stream directly
 * using StreamProcessorCoroutine, instead of going through the
 * ace_routine::Channel object.
 *
 * Run the sketch, then type 'help' on the serial port. The following
 * commands are supported:
 *  - `help [command]` - list the known commands
 *  - `list` - list the coroutines managed by the CoroutineScheduler
 *  - `free` - print free memory
 *  - `echo [args ...]` - echo the arguments
 *  - `delay (on | off) millis` - set LED blink on or off delay
 */

#include <Arduino.h>
#include <AceRoutine.h>
#include <AceUtils.h>
#include <cli/cli.h> // from AceUtils
#include <freemem/freemem.h> // from AceUtils

using ace_routine::CoroutineScheduler;
using ace_utils::cli::CommandHandler;
using ace_utils::cli::StreamProcessorManager;
using ace_utils::freemem::freeMemory;

// Every board except ESP32 defines SERIAL_PORT_MONITOR..
#if defined(ESP32)
  #define SERIAL_PORT_MONITOR Serial
#endif

//---------------------------------------------------------------------------

#ifdef LED_BUILTIN
  const int LED = LED_BUILTIN;
#else
  // Some ESP32 boards do not define LED_BUILTIN. Sometimes they have more than
  // 1. Replace this with the proper pin number.
  const int LED = 5;
#endif

const int LED_ON = HIGH;
const int LED_OFF = LOW;

// Use asymmetric delays for blinking to demonstrate how easy it is using
// COROUTINES.
int ledOnDelay = 100;
int ledOffDelay = 500;

COROUTINE(blinkLed) {
  COROUTINE_LOOP() {
    digitalWrite(LED, LED_ON);
    COROUTINE_DELAY(ledOnDelay);
    digitalWrite(LED, LED_OFF);
    COROUTINE_DELAY(ledOffDelay);
  }
}

//---------------------------------------------------------------------------

/** List the coroutines known by the CoroutineScheduler. */
class ListCommand: public CommandHandler {
  public:
    ListCommand():
      CommandHandler(F("list"), nullptr) {}

    void run(Print& printer, int /* argc */, const char* const* /* argv */)
        const override {
      CoroutineScheduler::list(printer);
    }
};

/** Echo the command line arguments. */
class EchoCommand: public CommandHandler {
  public:
    EchoCommand():
      CommandHandler(F("echo"), F("args ...")) {}

    void run(Print& printer, int argc, const char* const* argv) const override {
     for (int i = 1; i < argc; i++) {
        printer.print(argv[i]);
        printer.print(' ');
      }
      printer.println();
    }
};

/** Print amount of free memory between stack and heap. */
class FreeCommand: public CommandHandler {
  public:
    FreeCommand():
        CommandHandler(F("free"), nullptr) {}

    void run(Print& printer, int /* argc */, const char* const* /* argv */)
        const override {
      printer.print(F("Free memory: "));
      printer.println(freeMemory());
    }
};

/**
 * Change the blinking LED on and off delay parameters. If no argument given,
 * simply print out the current values. Demonstrates the use of shiftArgcArgv()
 * and the isArgEqual() helper method.
 */
class DelayCommand: public CommandHandler {
  public:
    DelayCommand():
        CommandHandler(F("delay"), F("[(on | off) millis]")) {}

    void run(Print& printer, int argc, const char* const* argv) const override {
      if (argc == 1) {
        printer.print(F("LED_ON delay: "));
        printer.println(ledOnDelay);
        printer.print(F("LED_OFF delay: "));
        printer.println(ledOffDelay);
        return;
      }

      if (argc < 3) {
        printer.println(F("Invalid number of arguments"));
        return;
      }

      shiftArgcArgv(argc, argv);
      if (isArgEqual(argv[0], F("on"))) {
        shiftArgcArgv(argc, argv);
        ledOnDelay = atoi(argv[0]);
      } else if (isArgEqual(argv[0], F("off"))) {
        shiftArgcArgv(argc, argv);
        ledOffDelay = atoi(argv[0]);
      } else {
        printer.print(F("Unknown argument: "));
        printer.println(argv[0]);
      }
    }
};

DelayCommand delayCommand;
ListCommand listCommand;
FreeCommand freeCommand;
EchoCommand echoCommand;
static const CommandHandler* const COMMANDS[] = {
  &delayCommand,
  &listCommand,
  &freeCommand,
  &echoCommand
};
static const uint8_t NUM_COMMANDS = sizeof(COMMANDS) / sizeof(CommandHandler*);

static const uint8_t BUF_SIZE = 64;
static const uint8_t ARGV_SIZE = 5;
static const char PROMPT[] = "$ ";

// Auto-inserts underlying StreamProcessorCoroutine into CoroutineScheduler
StreamProcessorManager<BUF_SIZE, ARGV_SIZE> commandManager(
    SERIAL_PORT_MONITOR, COMMANDS, NUM_COMMANDS, SERIAL_PORT_MONITOR, PROMPT);

//---------------------------------------------------------------------------

void setup() {
#if ! defined(EPOXY_DUINO)
  delay(1000);
#endif
  SERIAL_PORT_MONITOR.begin(115200);
  while (!SERIAL_PORT_MONITOR); // micro/leonardo
  pinMode(LED, OUTPUT);
#if defined(EPOXY_DUINO)
  SERIAL_PORT_MONITOR.setLineModeUnix();
  enableTerminalEcho();
#endif

  CoroutineScheduler::setup();
}

void loop() {
  CoroutineScheduler::loop();
}
