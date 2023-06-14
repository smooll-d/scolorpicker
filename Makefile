CFLAGS := -Wall -Wextra $(shell pkg-config --cflags --libs x11) -I./src
INSTALLDIR := /usr/bin

all: build

build: src/main.c
	mkdir -p build
	gcc -o build/scolorpicker ${CFLAGS} src/main.c

install: all
	mkdir -p ${INSTALLDIR}
	cp ./build/scolorpicker ${INSTALLDIR}

clean:
	rm -rf build/

uninstall:
	rm -f ${INSTALLDIR}/scolorpicker

.PHONY: all uninstall clean build install
