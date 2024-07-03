## Makefile.am in applications/demo/2d/partitioning

bin_PROGRAMS += applications/demo/2d/partitioning/partitioning

applications_demo_2d_partitioning_partitioning_SOURCES = \
	applications/demo/2d/partitioning/partitioning.cpp

## Include headers and libraries needed to build this application
##  -- Only really need AM_CPPFLAGS and LDADD (since these include all the other 
##     variables).  But others are included here for completeness
applications_demo_2d_partitioning_partitioning_CPPFLAGS = \
        $(AM_CPPFLAGS) \
        $(FCLAW_CLAWPACK46_CPPFLAGS) \
        $(FCLAW_CLAWPACK5_CPPFLAGS) \
        $(FCLAW_CLAWPATCH_CPPFLAGS)


applications_demo_2d_partitioning_partitioning_LDADD = \
        $(LDADD) \
        $(FCLAW_CLAWPACK46_LDADD) \
        $(FCLAW_CLAWPACK5_LDADD) \
        $(FCLAW_CLAWPATCH_LDADD) \
        $(FCLAW_LDADD)
