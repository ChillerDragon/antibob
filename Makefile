CXX := clang++
CC := clang
CXXFLAGS := \
	    -std=c++17 \
	    -Isrc/antibob \
	    -rdynamic \
	    -fPIC \
	    -Og -g
CXX_FLAGS_TEST = \
	    $(CXXFLAGS) \
	    -Isrc/ddnet \
	    -Isrc/test

POLYBOB_SRCS := $(wildcard \
	      src/ddnet/polybob/*/*.cpp \
	      src/ddnet/polybob/base/unicode/*.cpp \
	      src/ddnet/polybob/game/generated/*.cpp \
	      src/ddnet/polybob/engine/shared/*.cpp)
POLYBOB_OBJS := $(patsubst %.cpp,build/objs/polybob/%.o,$(POLYBOB_SRCS))

antibot: md5 $(POLYBOB_OBJS)
	$(CXX) \
		src/antibob/interface.cpp \
		src/antibob/*/*.cpp \
		$(POLYBOB_OBJS) \
		-Isrc/ddnet \
		build/*.o \
		$(CXXFLAGS) \
		-rdynamic \
		-shared \
		-o libantibot.so

build/objs/polybob/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) -Isrc/antibob -Isrc/ddnet -fPIC -Og -g -std=c++17 -c $< -o $@

test: md5
	$(CXX) \
		src/antibob/interface.cpp \
		src/test/*.cpp \
		src/test/*/*.cpp \
		$(CXX_FLAGS_TEST) -I . -L. libantibot.so -o antibob_test

build/md5.o: src/ddnet/polybob/engine/external/md5/md5.c src/ddnet/polybob/engine/external/md5/md5.h
	mkdir -p build
	$(CC) \
		src/ddnet/polybob/engine/external/md5/md5.c \
		-c \
		-o build/md5.o

md5: build/md5.o

run_test: test
	LD_LIBRARY_PATH=. ./antibob_test

clean:
	rm -rf build
	rm -f libantibot.so
	rm -f antibot_test

.PHONY: test clean

