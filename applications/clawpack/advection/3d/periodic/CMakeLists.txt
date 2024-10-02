set(all ${PROJECT_SOURCE_DIR}/clawpack/advection/3d/all)

add_library(periodic3d_f OBJECT 
  setprob.f90
  fdisc.f90
  user_4.6/periodic_qinit.f90
  ${all}/rpn3_advection.f90 
  ${all}/rpt3_advection.f90 
  ${all}/rptt3_advection.f90 
)

add_executable(periodic3d 
  periodic_user.cpp 
  periodic.cpp 
  periodic_options.c
  $<TARGET_OBJECTS:periodic3d_f>
)

target_include_directories(periodic3d PRIVATE 
  ${CMAKE_CURRENT_SOURCE_DIR} 
  ${all}
)

target_link_libraries(periodic3d PRIVATE 
 FORESTCLAW::CLAWPACK3_46
)

add_test(NAME clawpack_advection_3d_periodic COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/regressions.sh WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set_tests_properties(clawpack_advection_3d_periodic PROPERTIES ENVIRONMENT "${FCLAW_TEST_ENVIRONMENT}")