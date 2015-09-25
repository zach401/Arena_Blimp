# invoke SourceDir generated makefile for main.pe430
main.pe430: .libraries,main.pe430
.libraries,main.pe430: package/cfg/main_pe430.xdl
	$(MAKE) -f M:\EMBEDD~1\BLIMPP~1\air_modPWM/src/makefile.libs

clean::
	$(MAKE) -f M:\EMBEDD~1\BLIMPP~1\air_modPWM/src/makefile.libs clean

