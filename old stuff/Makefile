# Sample makefile for programs that use the ROSE library.
#
# ROSE has a number of configuration details that must be present when
# compiling and linking a user program with ROSE, and some of these 
# details are difficult to get right.  The most foolproof way to get
# these details into your own makefile is to use the "rose-config"
# tool. 
#
#
# This makefile assumes:
#   1. The ROSE library has been properly installed (refer to the
#      documentation for configuring, building, and installing ROSE).
#
#   2. The top of the installation directory is $(ROSE_INSTALLED). This
#      is the same directory you specified for the "--prefix" argument
#      of the "configure" script, or the "CMAKE_INSTALL_PREFIX" if using 
#      cmake. E.g., "/usr/local".
#
# The "rose-config" tool currently only works for ROSE configured with
# GNU auto tools (e.g., you ran "configure" when you built and
# installed ROSE). The "cmake" configuration is not currently
# supported by "rose-config" [September 2015].
##############################################################################

# Standard C++ compiler stuff (see rose-config --help)

CXX      = $(shell $(ROSE_INSTALLED)/bin/rose-config cxx)
CPPFLAGS = $(shell $(ROSE_INSTALLED)/bin/rose-config cppflags) -I.
CXXFLAGS = $(shell $(ROSE_INSTALLED)/bin/rose-config cxxflags)
LDFLAGS  = $(shell $(ROSE_INSTALLED)/bin/rose-config ldflags)
LIBDIRS  = $(shell $(ROSE_INSTALLED)/bin/rose-config libdirs)

PROGRAM_PATH = /home/bjoern/Documents/programs

BOOST_ROOT=/home/bjoern/opt/boost/1.61.0/gcc-4.9.3-default
ROSE_LIB_DIR = $(ROSE_INSTALLED)/lib
ROSE_LIBS = $(ROSE_LIB_DIR)/librose.so.0


MOSTLYCLEANFILES =
TEST_APPLICATION = mm

TRANSLATOR=Translator

AT_BUILD=./tests
AT_FLAGS=-rose:outline:output_path "$(AT_BUILD)"

ROSE_FLAGS=-rose:verbose 0 -g3 

ROSE_PROF1 =-rose:hpct:prof $(PROGRAM_PATH)/hpctoolkit-$(TEST_APPLICATION)-database/experiment.xml
ROSE_PROF2 =-rose:hpct:prof $(srcdir)/jacobi-raw-omp.xml 
ROSE_PROF3 =-rose:gprof:linebyline ./$(TEST_APPLICATION).result

EQUAL_PATH1=-rose:hpct:eqpath .=`cd $(PROGRAM_PATH) && pwd`

#ROSE_FLAGS+= -I$(PROGRAM_PATH)
# test command line options for outliner, They are set automatically by the translator now
#ROSE_FLAGS+= -rose:outline:parameter_wrapper -rose:outline:new_file -rose:outline:temp_variable -rose:outline:exclude_headers -rose:autotuning:enable_debug 
ROSE_FLAGS+= -rose:outline:parameter_wrapper -rose:outline:new_file -rose:outline:temp_variable -rose:outline:exclude_headers -rose:autotuning:enable_debug -rose:outline:enable_liveness


##############################################################################
# Assuming your source code is "Translator.cpp" to build an executable named "Translator".

all: Translator

Translator.o: Translator.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -I/usr/include/libxml2 -o $@ -c $^

Translator: Translator.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) -lrosehpct -L/usr/lib/x86_64-linux-gnu -lxml2 

MOSTLYCLEANFILES += $(TRANSLATOR) *.o *.out rose_$(TEST_APPLICATION).cpp

with_hpct : hpcprof Translator
	./$(TRANSLATOR) -c  $(ROSE_FLAGS) $(AT_FLAGS) $(EQUAL_PATH1) $(ROSE_PROF1) ./$(TEST_APPLICATION)

hpcprof : struct measurements
	hpcprof -S $(TEST_APPLICATION).hpcstruct -I $(PROGRAM_PATH)/$(TEST_APPLICATION).cpp hpctoolkit-$(TEST_APPLICATION)-measurements

struct : $(TEST_APPLICATION)_hpct
	hpcstruct $(TEST_APPLICATION)

measurements : $(TEST_APPLICATION)_hpct
	hpcrun -e WALLCLOCK -- ./$(TEST_APPLICATION) -n 120 120 120 -d 3

$(TEST_APPLICATION)_hpct: $(TEST_APPLICATION).cpp
	$(CXX) -g -O3 $(TEST_APPLICATION).cpp -o $(TEST_APPLICATION)

MOSTLYCLEANFILES += hpctoolkit-*-database* hpctoolkit-*-measurements *.hpcstruct

#with gprof

with_gprof: $(TEST_APPLICATION).result Translator
	./$(TRANSLATOR) -c -rose:output rose_$(TEST_APPLICATION).cpp $(ROSE_FLAGS) $(AT_FLAGS) $(ROSE_PROF3) ./$(TEST_APPLICATION)

$(TEST_APPLICATION).result : gmon.out
	gprof -l -L $(TEST_APPLICATION) gmon.out &>$(TEST_APPLICATION).result

gmon.out : $(TEST_APPLICATION)_gprof
	./$(TEST_APPLICATION)

$(TEST_APPLICATION)_gprof: $(TEST_APPLICATION).cpp
	$(CXX) -g $(TEST_APPLICATION).cpp -o $(TEST_APPLICATION) -pg



MOSTLYCLEANFILES += $(TEST_APPLICATION) $(TEST_APPLICATION).dump $(TEST_APPLICATION).new $(TEST_APPLICATION).result

##############################################################################
# Standard boilerplate

.PHONY: clean 
clean:
	rm -r -f $(MOSTLYCLEANFILES)
