CXX = clang++

antibot:
	$(CXX) src/antibot.cpp -I../ddnet/src -I../ddnet/build/src/ -fvisibility=hidden -fPIC -shared -o libantibot.so -Og -g
