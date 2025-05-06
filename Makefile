CXX := clang++
CC := clang
CXXFLAGS := \
	    -std=c++17 \
	    -Isrc/antibob \
	    -Isrc/ddnet \
	    -fPIC \
	    -Og -g

POLYBOB_SRCS := $(wildcard \
	      src/ddnet/polybob/*/*.cpp \
	      src/ddnet/polybob/base/unicode/*.cpp \
	      src/ddnet/polybob/game/generated/*.cpp \
	      src/ddnet/polybob/engine/shared/*.cpp)
POLYBOB_OBJS := $(patsubst %.cpp,build/objs/polybob/%.o,$(POLYBOB_SRCS))

ANTIBOB_SRCS := $(wildcard \
	      src/antibob/interface.cpp \
	      src/antibob/*/*.cpp)
ANTIBOB_OBJS := $(patsubst %.cpp,build/objs/antibob/%.o,$(ANTIBOB_SRCS))

BOBTEST_SRCS := $(wildcard src/test/*.cpp)
BOBTEST_OBJS := $(patsubst %.cpp,build/objs/bobtest/%.o,$(BOBTEST_SRCS))

TESTS_SRCS := $(wildcard src/test/bob/*.cpp)
TESTS_BINARIES := $(patsubst src/test/bob/%.cpp,build/%_bob_test,$(TESTS_SRCS))

DEPENDENCIES := build/objs/external/md5.o \
		build/objs/generated/git_revision.o

libantibot.so: $(DEPENDENCIES) $(POLYBOB_OBJS) $(ANTIBOB_OBJS)
	$(CXX) \
		$(ANTIBOB_OBJS) \
		$(POLYBOB_OBJS) \
		$(DEPENDENCIES) \
		-Isrc/ddnet \
		$(CXXFLAGS) \
		-rdynamic \
		-shared \
		-o libantibot.so

asan: CXXFLAGS += -fsanitize=address,undefined -fsanitize-recover=address,undefined -fno-omit-frame-pointer
asan: libantibot.so

build/objs/antibob/%.o: %.cpp %.h
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/objs/antibob/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/objs/polybob/%.o: %.cpp %.h
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/objs/polybob/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/objs/bobtest/%.o: %.cpp %.h
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/objs/bobtest/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/%_bob_test: src/test/bob/%.cpp libantibot.so $(DEPENDENCIES) $(BOBTEST_OBJS)
	$(CXX) \
		$(BOBTEST_OBJS) \
		$< \
		-Isrc/test \
		$(CXXFLAGS) \
		-I . \
		-L. \
		libantibot.so \
		-o $@

GIT_HASH := $(shell git rev-parse --short=16 HEAD)

build/objs/generated/%.o: build/src/antibob/bob/generated/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/src/antibob/bob/generated/git_revision.cpp:
	mkdir -p build/src/antibob/bob/generated
	echo "const char *BOB_GIT_SHORTREV_HASH = \"$(GIT_HASH)\";" > \
		build/src/antibob/bob/generated/git_revision.cpp

# this should be a C file
# compiled with C compiler
# but we need the C++ namespace feature for polybob
# to avoid odr violations because the server defines the same symbols
build/objs/external/md5.o: src/ddnet/polybob/engine/external/md5/md5.cpp src/ddnet/polybob/engine/external/md5/md5.h
	mkdir -p build/objs/external
	$(CXX) $< -c -o $@

tests: $(TESTS_BINARIES)

run_tests: tests
	$(foreach var,$(TESTS_BINARIES),LD_LIBRARY_PATH=. $(var);)

clean:
	rm -rf build
	rm -f libantibot.so
	rm -f antibot_test

.PHONY: run_tests clean

