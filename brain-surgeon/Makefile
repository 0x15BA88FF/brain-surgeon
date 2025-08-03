PREFIX ?= /usr/local
SRC = src/*.cpp
CXXFLAGS = -std=c++17 -Iinclude -Wall -O2
BIN = brain-surgeon

.PHONY: clean uninstall

$(BIN):
	mkdir -p build
	$(CXX) $(SRC) $(CXXFLAGS) -o build/$@

install: $(BIN)
	install build/$(BIN) $(PREFIX)/bin

clean:
	rm -rf build

uninstall:
	rm -f $(PREFIX)/bin/$(BIN)
