TARGETTYPE := executable
TARGETNAME := unittests
TARGETPATH := ./

include ../../Makefile.common

# Override default configuration
LIBS := $(LIBS) -lboost_unit_test_framework

SERVER_SOURCES := $(shell find ../../src/server -name "*.$(ENDING_SOURCE)" | grep -v main.cpp )

SERVER_OBJECTS := $(patsubst %.$(ENDING_SOURCE),%.o,$(SERVER_SOURCES))

OBJECTS := $(OBJECTS) $(SERVER_OBJECTS)