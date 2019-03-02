AM_CPPFLAGS = -iquote $(top_srcdir) -iquote $(top_srcdir)/cwds

bin_PROGRAMS = quantum

quantum_SOURCES = quantum.cxx
quantum_CXXFLAGS = @LIBCWD_FLAGS@
quantum_LDADD = ../cwds/libcwds.la

# --------------- Maintainer's Section

if MAINTAINER_MODE
distclean-local:
	rm -f *.s *.ii

clean-local:
endif

MAINTAINERCLEANFILES = $(srcdir)/Makefile.in
