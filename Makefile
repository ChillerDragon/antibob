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

libantibot.so: md5 git_hash $(POLYBOB_OBJS) $(ANTIBOB_OBJS)
	$(CXX) \
		$(ANTIBOB_OBJS) \
		$(POLYBOB_OBJS) \
		-Isrc/ddnet \
		build/md5.o \
		$(CXXFLAGS) \
		-rdynamic \
		-shared \
		-o libantibot.so

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

build/%_bob_test: src/test/bob/%.cpp md5 git_hash libantibot.so $(BOBTEST_OBJS)
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

git_hash: build/src/antibob/bob/generated/git_revision.cpp

build/src/antibob/bob/generated/git_revision.cpp:
	mkdir -p build/src/antibob/bob/generated
	echo "const char *GIT_SHORTREV_HASH = \"$(GIT_HASH)\";" > \
		build/src/antibob/bob/generated/git_revision.cpp

md5: build/md5.o

build/md5.o: src/ddnet/polybob/engine/external/md5/md5.c src/ddnet/polybob/engine/external/md5/md5.h
	mkdir -p build
	$(CC) \
		src/ddnet/polybob/engine/external/md5/md5.c \
		-c \
		-o build/md5.o

tests: $(TESTS_BINARIES)

run_tests: tests
	$(foreach var,$(TESTS_BINARIES),LD_LIBRARY_PATH=. $(var);)

clean:
	rm -rf build
	rm -f libantibot.so
	rm -f antibot_test

.PHONY: run_tests clean

