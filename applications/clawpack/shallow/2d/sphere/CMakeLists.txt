set(maps ${PROJECT_SOURCE_DIR}/clawpack/advection/2d/all)

add_library(sphere_swe_f OBJECT
	setprob.f90
	user_4.6/sphere_qinit.f90
	user_4.6/sphere_gaussian.f90
	user_4.6/sphere_setaux.f90
	user_4.6/sphere_b4step2.f90
	user_4.6/rpn2swsphere.f90
	user_4.6/rpt2swq.f90
)

add_executable(sphere_swe
	sphere.cpp
	sphere_user.cpp
	sphere_options.c
	${maps}/fclaw2d_map_latlong.c
	${maps}/fclaw2d_map_cubedsphere.c
	${maps}/fclaw2d_map_pillowsphere.c
	$<TARGET_OBJECTS:sphere_swe_f>
)

target_include_directories(sphere_swe PRIVATE 
  ${CMAKE_CURRENT_SOURCE_DIR} 
)

target_link_libraries(sphere_swe PRIVATE 
	FORESTCLAW::CLAWPACK4.6
	FORESTCLAW::CLAWPACK5
)