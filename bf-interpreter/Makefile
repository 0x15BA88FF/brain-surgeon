PREFIX ?= /usr/local
SRC = src/*.c
CFLAGS = -std=c99 -Wall -pedantic
BIN = bf

.PHONY: clean uninstall

$(BIN):
	mkdir -p build
	$(CC) $(SRC) $(CFLAGS) -o build/$@

install: $(BIN)
	install build/$(BIN) $(PREFIX)/bin

clean:
	rm -rf build

uninstall:
	rm -f $(PREFIX)/bin/$(BIN)
