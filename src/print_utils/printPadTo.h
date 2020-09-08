/*
 * MIT License
 * Copyright (c) 2018 Brian T. Park
 */

#ifndef PRINT_UTILS_PRINT_PAD_TO_H
#define PRINT_UTILS_PRINT_PAD_TO_H

#include <stdint.h>
#include <Print.h>

namespace print_utils {

/** Print an unsigned 2-digit integer to 'printer'. */
inline void printPad2To(Print& printer, uint16_t value, char padChar = ' ') {
  if (value < 10) printer.print(padChar);
  printer.print(value);
}

/** Print an unsigned 3-digit integer to 'printer'. */
inline void printPad3To(Print& printer, uint16_t val, char padChar = ' ') {
  if (val < 100) printer.print(padChar);
  if (val < 10) printer.print(padChar);
  printer.print(val);
}

/** Print an unsigned 4-digit integer to 'printer'. */
inline void printPad4To(Print& printer, uint16_t val, char padChar = ' ') {
  if (val < 1000) printer.print(padChar);
  if (val < 100) printer.print(padChar);
  if (val < 10) printer.print(padChar);
  printer.print(val);
}

/** Print an unsigned 5-digit integer to 'printer'. */
inline void printPad5To(Print& printer, uint16_t val, char padChar = ' ') {
  if (val < 10000) printer.print(padChar);
  if (val < 1000) printer.print(padChar);
  if (val < 100) printer.print(padChar);
  if (val < 10) printer.print(padChar);
  printer.print(val);
}

}

#endif
