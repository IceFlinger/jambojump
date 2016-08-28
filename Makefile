all:
	make -f Makefile.win
	make -f Makefile.unix
win:
	make -f Makefile.win
unix:
	make -f Makefile.unix
