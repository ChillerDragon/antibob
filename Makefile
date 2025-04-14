SHELL  := env DDNET_DIR=$(DDNET_DIR) $(SHELL)
DDNET_DIR ?= ../ddnet

CXX := clang++
CXX_FLAGS_BASE = \
	    -I$(DDNET_DIR)/src \
	    -I$(DDNET_DIR)/build/src \
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

antibot:
	$(CXX) src/antibob/interface.cpp src/antibob/*/*.cpp $(CXX_FLAGS_MAIN) -o libantibot.so

test:
	$(CXX) \
		src/antibob/interface.cpp \
		src/test/*.cpp \
		src/test/*/*.cpp \
		src/ddnet/polybob/*/*.cpp \
		src/ddnet/polybob/base/unicode/*.cpp \
		$(CXX_FLAGS_TEST) -I . -L. libantibot.so -o antibob_test

clean:
	rm libantibot.so

.PHONY: test clean

