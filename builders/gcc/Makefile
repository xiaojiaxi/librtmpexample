SYS=posix
#SYS=mingw

all: librtmp app myapp

install: librtmp app myapp
	@cd ../../librtmp; $(MAKE) install
	@cd ../../app; $(MAKE) install
	@cd ../../myapp; $(MAKE) install
clean:
	@cd ../../librtmp; $(MAKE) clean
	@cd ../../app; $(MAKE) clean
	@cd ../../myapp; $(MAKE) clean

librtmp: 
	@cd ../../librtmp; $(MAKE) all
app:
	@cd ../../app; $(MAKE) all
myapp:
	@cd ../../myapp; $(MAKE) all
