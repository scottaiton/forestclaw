set(all ${PROJECT_SOURCE_DIR}/demo/2d/all)


add_executable(partitioning
  partitioning.cpp
)

set_target_properties(partitioning PROPERTIES OUTPUT_NAME partitioning)

target_include_directories(partitioning PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${all}
)

target_link_libraries(partitioning PRIVATE
 FORESTCLAW::CLAWPACK4.6
 FORESTCLAW::CLAWPACK5
)
