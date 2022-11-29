OUTPUTDIR := bin/

CFLAGS := -std=c++14 -Wall

ifeq (,$(CONFIGURATION))
	CONFIGURATION := release
endif

ifeq (debug,$(CONFIGURATION))
CFLAGS += -g
else
CFLAGS += -O2
endif

SOURCES := src/*.cpp
# HEADERS := src/*.h

TARGETBIN := simplex-$(CONFIGURATION)

.SUFFIXES:
.PHONY: all clean

all: $(TARGETBIN)

# $(TARGETBIN): $(SOURCES) $(HEADERS)
$(TARGETBIN): $(SOURCES)
	$(CXX) -o $@ $(CFLAGS) $(SOURCES) 

format:
	clang-format -i src/*.cpp src/*.h

clean:
	rm -rf ./simplex-$(CONFIGURATION)
	rm -rf ./inputs/*_parsed.txt

check: all
	./checker.py

FILES = src/*.cpp \
		src/*.h