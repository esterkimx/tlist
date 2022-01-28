.PHONY: test
test: 
	mkdir -p build
	clang++ -ftemplate-depth=4096 -std=c++17 test/main.cpp -o build/test
	zsh -c 'time ./build/test'

clean:
	rm -rf build