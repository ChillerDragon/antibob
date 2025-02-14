CXX = clang++

antibot:
	$(CXX) src/antibot.cpp -I../ddnet/src -I../ddnet/build/src/ -rdynamic -fPIC -shared -o libantibot.so -Og -g
