AUTOMAKE_OPTIONS = foreign

INCLUDES = \
  -I$(includedir) \
  -I$(OFFLINE_MAIN)/include \
  -I`root-config --incdir`

libTreeMaker_la_LDFLAGS = \
  -nodefaultlibs \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib \
  `root-config --libs`

lib_LTLIBRARIES = \
  libTreeMaker.la

include_HEADERS = \
  ClusterIso.h \
  TreeMaker.h
  
libTreeMaker_la_SOURCES = \
  ClusterIso.C \
  TreeMaker.C

libTreeMaker_la_LIBADD = \
  -lg4dst \
  -lg4eval \
  -lcalotrigger \
  -ljetbackground \
  -lphool

BUILT_SOURCES = \
  testClusterIso.C
	testTreeMaker.C
noinst_PROGRAMS = \
  testClusterIso
	testTreeMaker
testexternals_LDADD = \
  libClusterIso.la
	libTreeMaker.la
testClusterIso.C:
testTreeMaker.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

##############################################
# please add new classes in alphabetical order
# Rule for generating CINT dictionaries from class headers.
%_Dict.C: %.h %LinkDef.h
	rootcint -f $@ -c $(CINTFLAGS) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $^

clean-local:
	rm -f *Dict* testexternals.C
testClusterIso_SOURCES = testClusterIso.C
testTreeMaker_SOURCES = testTreeMaker.C
