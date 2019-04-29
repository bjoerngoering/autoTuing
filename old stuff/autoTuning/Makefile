
CXX      = $(shell $(ROSE_INSTALLED)/bin/rose-config cxx)
CPPFLAGS = $(shell $(ROSE_INSTALLED)/bin/rose-config cppflags)
#CXXFLAGS = $(shell $(ROSE_INSTALLED)/bin/rose-config cxxflags)
LDFLAGS  = $(shell $(ROSE_INSTALLED)/bin/rose-config ldflags)
LIBDIRS  = $(shell $(ROSE_INSTALLED)/bin/rose-config libdirs)

ROSE_LIB_DIR = $(ROSE_INSTALLED)/lib

ROSE_LIBS = $(ROSE_LIB_DIR)/librose.so.0

ROSE_INS = /home/bjoern/opt/rose_inst
BOOST_ROOT=/home/opt/boost/1.61.0/gcc-4.9.3-default

#-------------translator---------------------------
SRC_FILE=autoTuningSupport.C autoTuning.C
#---------------input code ----------------------
INPUT_CODE=jacobi.c


CXXFLAGS=-g3 -Wall
#CPP_FLAG=-D_GLIBCXX_CONCEPT_CHECKS -D_GLIBCXX_DEBUG -g3

.PHONY :clean test default
all:clean test
default: a.out test

# create the translator
a.out:$(SRC_FILE)
	g++ $(CXXFLAGS) -I$(BOOST_ROOT)/include -I$(ROSE_INS)/include/rose -I/usr/include/libxml2 $(SRC_FILE) -L$(ROSE_INS)/lib -lxml2 -lrose -lrosehpct 

# test the translator
# -rose:skipfinalCompileStep
# --edg:no_warnings

ROSE_FLAGS=-rose:verbose 2 -g3  
ROSE_FLAGS+=-rose:hpct:prof jacobi-raw.xml 
# -rose:hpct:eqpath .=`cd /home/bjoern/rose-develop/tutorial/roseHPCT && pwd`
ROSE_FLAGS+=-rose:hpct:eqpath #.=/home/liao6/svnrepos/projects/empirical-tuning/rose-support

test: a.out
	./a.out -c  $(ROSE_FLAGS) $(INPUT_CODE) 
#	./a.out -c  -rose:astMerge -rose:verbose 0 -g3 $(INPUT_CODE) 
clean:
	rm -rf a.out *.o rose_* *.dot
