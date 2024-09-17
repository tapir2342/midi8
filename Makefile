PROGRAM := midi8

ifeq ($(OS),Windows_NT)
# TODO: Windows
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
		SOURCES := midi8_linux.c $(shell pkg-config --cflags --libs x11 xtst)
    endif
    ifeq ($(UNAME_S),Darwin)
		SOURCES := midi8_macos.c
		CCFLAGS := -Wall -D__MACOSX_CORE__ $(shell pkg-config --cflags --libs coremidi) -framework CoreMIDI -framework CoreAudio -framework CoreFoundation -framework ApplicationServices
    endif
endif

all: $(PROGRAM)
	./$(PROGRAM)

$(PROGRAM): $(SOURCES)
	cc -o $(PROGRAM) $(SOURCES) $(CCFLAGS)

