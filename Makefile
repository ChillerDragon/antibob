SHELL  := env DDNET_DIR=$(DDNET_DIR) $(SHELL)
DDNET_DIR ?= ../ddnet

CXX := clang++
CXX_FLAGS = \
	    -I$(DDNET_DIR)/src \
	    -I$(DDNET_DIR)/build/src \
	    -std=c++17 \
	    -rdynamic \
	    -fPIC \
	    -shared \
	    -Og -g

antibot:
	$(CXX) src/*.cpp $(CXX_FLAGS) -o libantibot.so

clean:
	rm libantibot.so
