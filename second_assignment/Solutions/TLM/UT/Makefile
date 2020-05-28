DESIGN_NAME=double_multiplier
ABSTRACTION_LEVEL=UT

EXECUTABLE=bin/$(DESIGN_NAME)_$(ABSTRACTION_LEVEL).x
FILE_MAIN=main_${ABSTRACTION_LEVEL}
FILE_TEST=testbench_${ABSTRACTION_LEVEL}
FILE_SRC=double_multiplier_${ABSTRACTION_LEVEL}
FILE_OTHER=define_${ABSTRACTION_LEVEL}

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

$(EXECUTABLE):  obj/$(FILE_MAIN).o obj/$(FILE_TEST).o obj/$(FILE_SRC).o
	@echo Building $@
	$(CXX) -g $(INCDIR) -o $@ $^ $(LIBDIR)/libsystemc.a -lpthread

obj/$(FILE_MAIN).o: src/$(FILE_MAIN).cc include/$(FILE_TEST).hh include/$(FILE_SRC).hh include/$(FILE_OTHER).hh
	@echo Building $@
	$(CXX) -g $(CFLAGS) $(INCDIR) -o $@ $< 

obj/$(FILE_TEST).o: src/$(FILE_TEST).cc include/$(FILE_TEST).hh include/$(FILE_OTHER).hh
	@echo Building $@
	$(CXX) -g $(CFLAGS) $(INCDIR) -o $@ $< 

obj/$(FILE_SRC).o: src/$(FILE_SRC).cc include/$(FILE_SRC).hh include/$(FILE_OTHER).hh
	@echo Building $@
	$(CXX) -g $(CFLAGS) $(INCDIR) -o $@ $<


clean:
	@echo Cleaning
	@rm -f obj/*.o *~
	@rm -f bin/*

.PHONY: all clean