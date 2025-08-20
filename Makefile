.PHONY: run compile


compile: build
	mkdir -p build
	cmake -S . -B build

run: CMakeLists.txt build
	cmake --build build
	./build/OpenBongoCat
