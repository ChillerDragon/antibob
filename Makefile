SHELL  := env DDNET_DIR=$(DDNET_DIR) $(SHELL)
DDNET_DIR ?= ../ddnet

CXX := clang++
CC := clang
CXX_FLAGS_BASE = \
	    -std=c++17 \
	    -Isrc/antibob \
	    -rdynamic \
	    -fPIC \
	    -Og -g
CXX_FLAGS_MAIN = \
	    $(CXX_FLAGS_BASE) \
	    -shared
CXX_FLAGS_TEST = \
	    $(CXX_FLAGS_BASE) \
	    -Isrc/ddnet \
	    -Isrc/test

POLYBOB_SRC = \
	      src/ddnet/polybob/*/*.cpp \
	      src/ddnet/polybob/base/unicode/*.cpp \
	      src/ddnet/polybob/game/generated/*.cpp \
	      src/ddnet/polybob/engine/shared/*.cpp

antibot: md5
	$(CXX) \
		src/antibob/interface.cpp \
		src/antibob/*/*.cpp \
		$(POLYBOB_SRC) \
		-Isrc/ddnet \
		build/*.o \
		$(CXX_FLAGS_MAIN) -o libantibot.so

test: md5
	$(CXX) \
		src/antibob/interface.cpp \
		src/test/*.cpp \
		src/test/*/*.cpp \
		$(CXX_FLAGS_TEST) -I . -L. libantibot.so -o antibob_test

md5: src/ddnet/polybob/engine/external/md5/md5.c src/ddnet/polybob/engine/external/md5/md5.h
	mkdir -p build
	$(CC) \
		src/ddnet/polybob/engine/external/md5/md5.c \
		-c \
		-o build/md5.o

clean:
	rm -rf build
	rm libantibot.so

.PHONY: test clean

