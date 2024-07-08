option(FCLAW_ENABLE_MPI "use MPI library" ${mpi})
option(FCLAW_ENABLE_OPENMP "use OpenMP" ${openmp})
option(FCLAW_ENABLE_HDF5 "use HDF5 library" ${hdf5})

if(NOT DEFINED applications AND ${PROJECT_IS_TOP_LEVEL})
  set(applications ON)
endif()
option(FCLAW_ENABLE_APPLICATIONS "build applications" ${applications})

option(FCLAW_ENABLE_CLAWPATCH "build Clawpatch" ${clawpatch})
option(FCLAW_ENABLE_CLAWPACK "build Clawpack" ${clawpack})
option(FCLAW_ENABLE_GEOCLAW "build Geoclaw" ${geoclaw})
option(FCLAW_ENABLE_CUDACLAW "build CudaClaw" ${cudaclaw})
option(FCLAW_ENABLE_THUNDEREGG "build ThunderEgg" ${thunderegg})

# needed by ThunderEgg, P4EST, Libsc

option(CMAKE_TLS_VERIFY "verify TLS cert" on)

# --- default install directory under build/local
# users can specify like "cmake -B build -DCMAKE_INSTALL_PREFIX=~/mydir"
if(CMAKE_VERSION VERSION_LESS 3.21)
  get_property(_not_top DIRECTORY PROPERTY PARENT_DIRECTORY)
  if(NOT _not_top)
    set(FORESTCLAW_IS_TOP_LEVEL true)
  endif()
endif()

if(FORESTCLAW_IS_TOP_LEVEL AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  # will not take effect without FORCE
  set(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/local" CACHE PATH "Install top-level directory" FORCE)
endif()

unset(${PROJECT_NAME}_stdfs_link_flags)
if( (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.1.0") OR
  (LINUX AND CMAKE_CXX_COMPILER_ID STREQUAL "IntelLLVM" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "23") OR
  (CMAKE_CXX_COMPILER_ID STREQUAL "NVHPC" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "23.11") )
set(${PROJECT_NAME}_stdfs_link_flags stdc++fs stdc++)
endif()
# GCC < 9.1 needs -lstdc++ to avoid C main program link error
# NVHPC at least 23.11 and newer doesn't need the flags, but at least 23.5 and older do.
# INtel oneAPI 2021.1 and older needs, but 2023 and newer doesn't. (not sure about 2022)


# enable needed dependencies
if(FCLAW_ENABLE_MPI AND FCLAW_ENABLE_HDF5)
  set(hdf5_parallel ON)
endif()

# Necessary for shared library with Visual Studio / Windows oneAPI
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS true)
