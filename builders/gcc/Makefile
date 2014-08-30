SYS=posix
#SYS=mingw

all: librtmp app

install: librtmp app
	@cd ../../librtmp; $(MAKE) install
	@cd ../../app; $(MAKE) install
clean:
	@cd ../../librtmp; $(MAKE) clean
	@cd ../../app; $(MAKE) clean

librtmp: 
	@cd ../../librtmp; $(MAKE) all
app:
	@cd ../../librtmp; $(MAKE) all
