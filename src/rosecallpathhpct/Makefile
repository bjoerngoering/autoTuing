#make rose callpath hpct lib

INTERMEDIATE_PATH_CALLPATH = $(INTERMEDIATE_PATH)/rosecallpathhpct
BOOST_ROOT=/home/bjoern/opt/boost/1.61.0/gcc-4.9.3-default
SRCS = $(wildcard *.cc)
LIBOBJS = $(SRCS:.cc=.lo)
DIRNAME=$(lastword $(subst /, ,$(shell pwd)))
DIR_PATH=$(INTERMEDIATE_PATH_CALLPATH)
LIBNAME=lib$(DIRNAME).la

#-------------------------------SET-ENVIRONMENT-------------------------------#

export INTERMEDIATE_PATH_CALLPATH LIB_PATH

all : librosecallpathhpct.so 

librosecallpathhpct.so : create_lib
	cp $(INTERMEDIATE_PATH_CALLPATH)/.libs/* $(LIB_PATH)

compile_components :
	$(MAKE) -C profir
	$(MAKE) -C profir2sage
	$(MAKE) -C sage
	$(MAKE) -C util
	$(MAKE) -C xml
	$(MAKE) -C xml2profir


create_dir : 
	mkdir -p $(DIR_PATH)

rosecallpathhpct.lo : rosecallpathhpct.cc | create_dir
	libtool compile $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $(DIR_PATH)/$^

create_lib: compile_components rosecallpathhpct.lo
	cd $(DIR_PATH) && libtool --mode=link $(CXX) $(CXXFLAGS) $(CPPFLAGS) -L/usr/lib/x86_64-linux-gnu -module -o $(LIBNAME) -rpath $(LIB_PATH) \
	  profir2sage/libprofir2sage.la profir/libprofir.la  sage/libsage.la util/libutil.la  \
	 xml2profir/libxml2profir.la xml/libxml.la  rosecallpathhpct.lo -lxml2 -lm
