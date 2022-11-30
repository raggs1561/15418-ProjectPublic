OUTPUTDIR := bin/

CFLAGS := -std=c++14 -Wall -fvisibility=hidden -lpthread -fopenmp

ifeq (,$(CONFIGURATION))
	CONFIGURATION := release
endif

ifeq (debug,$(CONFIGURATION))
CFLAGS += -g
else
CFLAGS += -O2
endif

TARGETBIN := simplex-$(CONFIGURATION)

.SUFFIXES:
.PHONY: all clean

all: simplex-seq simplex-openmp

simplex-seq: src/simplex-sequential.cpp
	$(CXX) -o $@ $(CFLAGS) src/simplex-sequential.cpp

simplex-openmp: src/simplex-openmp.cpp
	$(CXX) -o $@ $(CFLAGS) src/simplex-openmp.cpp

format:
	clang-format -i src/*.cpp src/*.h

clean:
	rm -rf ./simplex-openmp
	rm -rf ./simplex-seq
	rm -rf ./inputs/*_parsed.txt

check: all
	./checker.py

FILES = src/*.cpp \
		src/*.h