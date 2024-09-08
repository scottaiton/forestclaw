## Makefile.am in applications/clawpack/advection/3d/periodic

bin_PROGRAMS += applications/clawpack/advection/3d/periodic/periodic3d

TESTS += applications/clawpack/advection/3d/periodic/regressions.sh

applications_clawpack_advection_3d_periodic_periodic3d_SOURCES = \
	applications/clawpack/advection/3d/periodic/periodic_user.cpp \
	applications/clawpack/advection/3d/periodic/periodic_user.h \
	applications/clawpack/advection/3d/periodic/periodic_options.c \
	applications/clawpack/advection/3d/periodic/periodic.cpp \
	applications/clawpack/advection/3d/periodic/setprob.f90 \
	applications/clawpack/advection/3d/periodic/fdisc.f90 \
	applications/clawpack/advection/3d/periodic/user_4.6/periodic_qinit.f90 \
	applications/clawpack/advection/3d/all/rpn3_advection.f90 \
	applications/clawpack/advection/3d/all/rpt3_advection.f90 \
	applications/clawpack/advection/3d/all/rptt3_advection.f90

EXTRA_DIST += \
	applications/clawpack/advection/3d/periodic/fclaw_options.ini \
	applications/clawpack/advection/3d/periodic/regression.ini \
	applications/clawpack/advection/3d/periodic/afterframe.m \
	applications/clawpack/advection/3d/periodic/setplot3.m \
	applications/clawpack/advection/3d/periodic/regions_utility.py \
	applications/clawpack/advection/3d/periodic/write_regions.py

## Include headers and libraries needed to build this application
##  -- Only really need AM_CPPFLAGS and LDADD (since these include all the other 
##     variables).  But others are included here for completeness
applications_clawpack_advection_3d_periodic_periodic3d_CPPFLAGS = \
        $(AM_CPPFLAGS) \
        $(FCLAW_CLAWPACK3.46_CPPFLAGS) \
        $(FCLAW_CLAWPATCH_CPPFLAGS)


applications_clawpack_advection_3d_periodic_periodic3d_LDADD = \
        $(LDADD) \
        $(FCLAW_CLAWPACK3.46_LDADD) \
        $(FCLAW_CLAWPATCH_LDADD) \
        $(FCLAW_LDADD)
