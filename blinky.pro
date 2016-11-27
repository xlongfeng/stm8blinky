QT -= core gui

TARGET = blinky.ihx
TEMPLATE = app

OBJECTS_DIR = objs

QMAKE_CFLAGS = --verbose --stack-auto -mstm8
QMAKE_CXXFLAGS = $$QMAKE_CFLAGS
QMAKE_LFLAGS += --verbose -lstm8 -mstm8 --out-fmt-ihx
QMAKE_CLEAN = *.cdb *.lk *.map
QMAKE_DISTCLEAN = $$OBJECTS_DIR/*.asm $$OBJECTS_DIR/*.lst $$OBJECTS_DIR/*.rst $$OBJECTS_DIR/*.sym

DEFINES += STM8S103
INCLUDEPATH += src libs/STM8S_StdPeriph_Driver/inc

SOURCES += \
    src/main.c \
    src/stm8s_it.c \
    libs/STM8S_StdPeriph_Driver/src/stm8s_gpio.c

HEADERS += \
    src/stm8s_conf.h \
    src/stm8s_it.h \
    libs/STM8S_StdPeriph_Driver/inc/stm8s.h \
    libs/STM8S_StdPeriph_Driver/inc/stm8s_gpio.h
