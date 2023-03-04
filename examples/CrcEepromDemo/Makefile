# See https://github.com/bxparks/EpoxyDuino for documentation about this
# Makefile to compile and run Arduino programs natively on Linux or MacOS.

APP_NAME := CrcEepromDemo
ARDUINO_LIBS := EpoxyEepromEsp AceCommon AceCRC AceRoutine AceUtils
MORE_CLEAN := more_clean
include ../../../EpoxyDuino/EpoxyDuino.mk

more_clean:
	rm -f epoxyeepromdata
