all:
	make -f Makefile.win
	make -f Makefile.unix
	make -f Makefile.wasm
win:
	make -f Makefile.win
unix:
	make -f Makefile.unix
wasm:
	make -f Makefile.wasm
