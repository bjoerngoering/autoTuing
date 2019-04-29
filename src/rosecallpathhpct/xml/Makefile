SRCS = $(wildcard *.cc)
LIBOBJS = $(SRCS:.cc=.lo)
DIRNAME=$(lastword $(subst /, ,$(shell pwd)))
DIR_PATH=$(INTERMEDIATE_PATH_CALLPATH)/$(DIRNAME)
LIBNAME=lib$(DIRNAME).la

all: create_lib

create_dir : 
	mkdir -p $(DIR_PATH)

%.lo : %.cc | create_dir
	libtool compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $(DIR_PATH)/$^

create_lib: $(LIBOBJS)
	cd $(DIR_PATH) && libtool --mode=link $(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $(DIR_PATH)/$(LIBNAME) $(LIBOBJS)