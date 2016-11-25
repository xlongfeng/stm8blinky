QT -= core gui

TARGET = blinky.ihx
TEMPLATE = app

OBJECTS_DIR = objs

sdcc {
    QMAKE_CFLAGS = -mstm8 --std-c99
    QMAKE_CXXFLAGS = -mstm8
    QMAKE_LFLAGS += -lstm8 -mstm8 --out-fmt-ihx
    QMAKE_CLEAN = *.lk *.map
    QMAKE_DISTCLEAN = $$OBJECTS_DIR/*.asm $$OBJECTS_DIR/*.lst $$OBJECTS_DIR/*.rst $$OBJECTS_DIR/*.sym
}


SOURCES += src/main.c
