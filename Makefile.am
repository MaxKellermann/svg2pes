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
	src/Main.cxx
