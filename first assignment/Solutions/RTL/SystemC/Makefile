DESIGN_NAME=double_multiplier
ABSTRACTION_LEVEL=RTL

EXECUTABLE=bin/$(DESIGN_NAME)_$(ABSTRACTION_LEVEL).x
FILE_MAIN=main_${ABSTRACTION_LEVEL}
FILE_TEST=testbench_${ABSTRACTION_LEVEL}
FILE1=double_multiplier_${ABSTRACTION_LEVEL}
FILE2=multiplier_${ABSTRACTION_LEVEL}

SYSTEMC=$$SYSTEMC_HOME

PLATFORM= $(shell uname -s)
ARCH=$(shell getconf LONG_BIT)

ifeq ($(PLATFORM),Darwin)
SYSC_LIBDIR := $(SYSTEMC)/lib-macosx
endif
ifeq ($(PLATFORM),Linux)
SYSC_LIBDIR := $(SYSTEMC)/lib-linux
endif

ifeq ($(ARCH),64)
SYSC_LIBDIR := $(SYSC_LIBDIR)64
endif

INCDIR = -I./include/ -I. -I$(SYSTEMC)/include/

LIBDIR = $(SYSC_LIBDIR)

CXX=g++
CFLAGS= -c
LD=g++

all: $(EXECUTABLE)

$(EXECUTABLE):  obj/$(FILE_MAIN).o obj/$(FILE_TEST).o obj/$(FILE1).o obj/$(FILE2).o
	@echo Building $@
	$(CXX) -g $(INCDIR) -o $@ $^ $(LIBDIR)/libsystemc.a -lpthread

obj/$(FILE_MAIN).o: src/$(FILE_MAIN).cc include/$(FILE_TEST).hh include/$(FILE1).hh include/$(FILE2).hh
	@echo Building $@
	$(CXX) -g $(CFLAGS) $(INCDIR) -o $@ $< 

obj/$(FILE_TEST).o: src/$(FILE_TEST).cc include/$(FILE_TEST).hh
	@echo Building $@
	$(CXX) -g $(CFLAGS) $(INCDIR) -o $@ $< 

obj/$(FILE1).o: src/$(FILE1).cc include/$(FILE1).hh include/$(FILE2).hh
	@echo Building $@
	$(CXX) -g $(CFLAGS) $(INCDIR) -o $@ $<

obj/$(FILE2).o: src/$(FILE2).cc include/$(FILE2).hh
	@echo Building $@
	$(CXX) -g $(CFLAGS) $(INCDIR) -o $@ $<


clean:
	@echo Cleaning
	@rm -f obj/*.o *~
	@rm -f bin/*

.PHONY: all clean
