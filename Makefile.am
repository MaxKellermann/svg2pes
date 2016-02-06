ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = foreign 1.11 subdir-objects dist-xz

AM_CPPFLAGS += -I$(srcdir)/src

bin_PROGRAMS = svg2pes

svg2pes_SOURCES = \
	src/Compiler.h \
	src/util/AllocatedArray.hxx \
	src/util/ByteOrder.hxx \
	src/util/GrowingBuffer.hxx \
	src/util/SystemError.hxx \
	src/util/ScopeExit.hxx \
	src/util/ConstBuffer.hxx \
	src/util/WritableBuffer.hxx \
	src/ExpatParser.cxx src/ExpatParser.hxx \
	src/SvgParser.cxx src/SvgParser.hxx \
	src/Main.cxx
svg2pes_CPPFLAGS = $(AM_CPPFLAGS) $(EXPAT_CFLAGS)
svg2pes_LDADD = $(AM_LDADD) $(EXPAT_LIBS)
