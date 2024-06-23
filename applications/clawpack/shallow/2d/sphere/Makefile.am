## Makefile.am in applications/clawpack/shallow/2d/sphere

bin_PROGRAMS += applications/clawpack/shallow/2d/sphere/sphere


applications_clawpack_shallow_2d_sphere_sphere_SOURCES = \
	applications/clawpack/shallow/2d/sphere/sphere.cpp \
	applications/clawpack/shallow/2d/sphere/sphere_user.cpp \
	applications/clawpack/shallow/2d/sphere/sphere_user.h \
	applications/clawpack/shallow/2d/sphere/sphere_options.c \
	applications/clawpack/advection/2d/all/fclaw2d_map_latlong.c \
	applications/clawpack/advection/2d/all/fclaw2d_map_cubedsphere.c \
	applications/clawpack/advection/2d/all/fclaw2d_map_pillowsphere.c \
	applications/clawpack/shallow/2d/sphere/setprob.f90 \
	applications/clawpack/shallow/2d/sphere/rpn2cons_update_manifold.f \
	applications/clawpack/shallow/2d/sphere/bmount.f90 \
	applications/clawpack/shallow/2d/sphere/fdisc.f90 \
	applications/clawpack/shallow/2d/sphere/sphere_exceeds_threshold.f90 \
	applications/clawpack/shallow/2d/sphere/user_4.6/sphere_output_ascii_fort.f \
	applications/clawpack/shallow/2d/sphere/user_4.6/sphere_qinit.f90 \
	applications/clawpack/shallow/2d/sphere/user_4.6/sphere_setaux.f90 \
	applications/clawpack/shallow/2d/sphere/user_4.6/sphere_b4step2.f90 \
	applications/clawpack/shallow/2d/sphere/user_4.6/sphere_src2.f90 \
	applications/clawpack/shallow/2d/sphere/user_4.6/rpn2swsphere.f90 \
	applications/clawpack/shallow/2d/sphere/user_4.6/rpt2swq.f90 \
	applications/clawpack/shallow/2d/sphere/user_4.6/rpn2swsphere_fwave.f90 \
	applications/clawpack/shallow/2d/sphere/user_4.6/rpt2swq_fwave.f90 \
	applications/clawpack/shallow/2d/sphere/user_5.0/sphere_output_ascii_fort.f \
	applications/clawpack/shallow/2d/sphere/user_5.0/sphere_qinit.f90 \
	applications/clawpack/shallow/2d/sphere/user_5.0/sphere_setaux.f90 \
	applications/clawpack/shallow/2d/sphere/user_5.0/sphere_b4step2.f90 \
	applications/clawpack/shallow/2d/sphere/user_5.0/sphere_src2.f90 \
	applications/clawpack/shallow/2d/sphere/user_5.0/rpn2swsphere.f90 \
	applications/clawpack/shallow/2d/sphere/user_5.0/rpt2swq.f90 \
	applications/clawpack/shallow/2d/sphere/user_5.0/rpn2swsphere_fwave.f90 \
	applications/clawpack/shallow/2d/sphere/user_5.0/rpt2swq_fwave.f90

EXTRA_DIST += \
	applications/clawpack/shallow/2d/sphere/fclaw_options.ini \
	applications/clawpack/shallow/2d/sphere/regression.ini \
	applications/clawpack/shallow/2d/sphere/afterframe.m \
	applications/clawpack/shallow/2d/sphere/setplot2.m \
	applications/clawpack/shallow/2d/sphere/mapc2m.m \
	applications/clawpack/shallow/2d/sphere/map1d.m \
	applications/clawpack/shallow/2d/sphere/read_vars.m


applications_clawpack_shallow_2d_sphere_sphere_CPPFLAGS = \
        $(AM_CPPFLAGS) \
        $(FCLAW_CLAWPACK46_CPPFLAGS) \
        $(FCLAW_CLAWPACK5_CPPFLAGS) \
        $(FCLAW_CLAWPATCH_CPPFLAGS)

applications_clawpack_shallow_2d_sphere_sphere_LDADD = \
        $(LDADD) \
        $(FCLAW_CLAWPACK46_LDADD) \
        $(FCLAW_CLAWPACK5_LDADD) \
        $(FCLAW_CLAWPATCH_LDADD) \
        $(FCLAW_LDADD)
