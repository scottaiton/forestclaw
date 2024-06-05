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
	applications/clawpack/shallow/2d/sphere/bmount.f90 \
	applications/clawpack/shallow/2d/sphere/fdisc.f90 \
	applications/clawpack/shallow/2d/sphere/sphere_exceeds_threshold.f90 \
	applications/clawpack/shallow/2d/sphere/user_4.6/sphere_output_ascii_fort.f \
	applications/clawpack/shallow/2d/sphere/user_4.6/rpn2cons_update_manifold.f  \
	applications/clawpack/shallow/2d/sphere/user_4.6/sphere_qinit.f90 \
	applications/clawpack/shallow/2d/sphere/user_4.6/sphere_gaussian.f90 \
	applications/clawpack/shallow/2d/sphere/user_4.6/sphere_setaux.f90 \
	applications/clawpack/shallow/2d/sphere/user_4.6/sphere_b4step2.f90 \
	applications/clawpack/shallow/2d/sphere/user_4.6/sphere_src2.f90 \
	applications/clawpack/shallow/2d/sphere/user_4.6/rpn2swsphere.f90 \
	applications/clawpack/shallow/2d/sphere/user_4.6/rpt2swq.f90 \
	applications/clawpack/shallow/2d/sphere/user_4.6/rpn2swsphere_fwave.f90 \
	applications/clawpack/shallow/2d/sphere/user_4.6/rpt2swq_fwave.f90

applications_clawpack_shallow_2d_sphere_sphere_CPPFLAGS = \
        $(AM_CPPFLAGS) $(FCLAW_CLAWPACK46_CPPFLAGS)

applications_clawpack_shallow_2d_sphere_sphere_LDADD = \
        $(FCLAW_CLAWPACK46_LDADD) $(LDADD)
