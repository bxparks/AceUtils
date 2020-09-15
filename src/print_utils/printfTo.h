/*
 * MIT License
 * Copyright (c) 2018 Brian T. Park
 */

/*
 * Implement a printfTo() that accept formatting strings like printf() and
 * prints the result to the given `Print` object. This is intended to be used
 * on platforms whose `Print` does not provide a `Print.printf()` function, for
 * example, the original Arduino UNO or Nano. I finally got tired of writing
 * multiple lines of SERIAL_PORT_MONITOR.print() for debugging.
 *
 * NOTE: These *must* be implemented as inline function to allow the compiler
 * to remove unused functions from the binary. For some reason, on AVR, ESP8266
 * and ESP32 compilers, link-time-optimization does not seem to work well. If
 * these functions are defined in a .cpp file, they are included in the binary,
 * even if they are not reference at all by anything. This causes the binary to
 * be about 700 (AVR) to 1000 (ESP32) bytes larger in flash memory.
 *
 * Being inlined here means that <Arduino.h> must be included here, which can
 * cause some problems in files that try to clobber macros defined in
 * <Arduino.h>.
 */

#ifndef PRINT_UTILS_PRINTF_TO_H
#define PRINT_UTILS_PRINTF_TO_H

#include <stdarg.h>
#include <Arduino.h>

namespace print_utils {

/** Maximum size of the internal stack buffer used by printfTo(). */
static const int BUF_SIZE = 192;

/** Helper function for implementing the printfTo() function. */
inline void vprintfTo(Print& printer, const char *fmt, va_list args) {
  char buf[BUF_SIZE];
  vsnprintf(buf, BUF_SIZE, fmt, args);
  printer.print(buf);
}

/**
 * A printf() that works on an Arduino `Print` object using the built-in
 * vsnprintf(). Append a '\\n' at the end of the string to print a newline.
 * This is intended to be used on platforms without a built-in `Print.printf()`
 * function, for example, the original Arduino UNO or Nano.
 */
inline void printfTo(Print& printer, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vprintfTo(printer, fmt, args);
  va_end(args);
}

}

#endif
