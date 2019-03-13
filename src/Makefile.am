AM_CPPFLAGS = -iquote $(top_srcdir) -iquote $(top_srcdir)/cwds

bin_PROGRAMS = quantum rational_test

quantum_SOURCES = quantum.cxx QuBit.cxx XState.cxx YState.cxx ZState.cxx QState.cxx QuBitField.cxx Gates.cxx Circuit.cxx State.cxx InputCollector.cxx
quantum_CXXFLAGS = @LIBCWD_FLAGS@ @EIGEN_CFLAGS@
quantum_LDADD = ../utils/libutils.la ../cwds/libcwds.la -lgmp @EIGEN_LIBS@

rational_test_SOURCES = rational_test.cxx QuBitField.cxx
rational_test_CXXFLAGS = @LIBCWD_FLAGS@ @EIGEN_CFLAGS@
rational_test_LDADD = ../utils/libutils.la ../cwds/libcwds.la -lgmp @EIGEN_LIBS@

# --------------- Maintainer's Section

if MAINTAINER_MODE
distclean-local:
	rm -f *.s *.ii

clean-local:
endif

MAINTAINERCLEANFILES = $(srcdir)/Makefile.in
