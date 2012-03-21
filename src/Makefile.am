bin_PROGRAMS = gramd
gramd_SOURCES = gramd.cpp data.cpp encoding.cpp

gramd_CPPFLAGS = $(BOOST_CPPFLAGS)
gramd_LDFLAGS = $(BOOST_THREAD_LDFLAGS) $(BOOST_SYSTEM_LDFLAGS) $(BOOST_WSERIALIZATION_LDFLAGS) $(BOOST_PROGRAM_OPTIONS_LDFLAGS) \
	$(BOOST_THREAD_LIBS) $(BOOST_SYSTEM_LIBS) $(BOOST_WSERIALIZATION_LIBS) $(BOOST_PROGRAM_OPTIONS_LIBS)

#gramd_LDADD = -lpthread -lboost_system -lboost_thread-mt -lboost_wserialization -lboost_program_options
