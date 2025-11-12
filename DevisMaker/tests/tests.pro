QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

# Test sources
SOURCES += \
    AreaTest.cpp \
    MovingObjectTest.cpp

HEADERS += \
    AreaTest.h \
    MovingObjectTest.h

# Source files to test
SOURCES += \
    ../area.cpp \
    ../src/inventory/movingObject.cpp

HEADERS += \
    ../area.h \
    ../src/inventory/movingObject.h

# Include paths
INCLUDEPATH += ..
INCLUDEPATH += ../src
INCLUDEPATH += ../src/inventory

# Compiler flags
QMAKE_CXXFLAGS += -std=c++17

# Output directory
DESTDIR = $$PWD/bin
OBJECTS_DIR = $$PWD/obj
MOC_DIR = $$PWD/moc
RCC_DIR = $$PWD/rcc
