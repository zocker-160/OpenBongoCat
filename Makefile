.PHONY: run compile


compile: build
	mkdir -p build
	cmake -S src -B build

run: CMakeLists.txt build
	cmake --build build
	./build/OpenBongoCat
