# ----------------------------------
# Clawpack3 library and examples
# ----------------------------------


# -----------------------
# Scalar advection (3dx)
# -----------------------

# swirl : Uses aux array for variable velocity field
add_subdirectory(clawpack/advection/3dx/swirl)

# filament : Uses aux array for variable velocity field
add_subdirectory(clawpack/advection/3dx/filament)

# sphere : Uses aux array for variable velocity field
add_subdirectory(clawpack/advection/3dx/sphere)

# disk : Uses aux array for variable velocity field
add_subdirectory(clawpack/advection/3dx/disk)

# latlong : Uses aux array for variable velocity field
add_subdirectory(clawpack/advection/3dx/latlong)

# -----------------------
# Scalar advection (3d)
# -----------------------

# swirl : Uses aux array for variable velocity field
add_subdirectory(clawpack/advection/3d/swirl)

# periodic : Uses constant velocity field
add_subdirectory(clawpack/advection/3d/periodic)

# -----------------------
# Euler equations (3dx)
# -----------------------
add_subdirectory(clawpack/euler/3dx/overpressure)




