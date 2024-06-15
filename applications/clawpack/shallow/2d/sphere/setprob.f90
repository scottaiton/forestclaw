subroutine setprob()
    implicit none

    double precision pi, pi2, deg2rad
    common /compi/ pi, pi2, deg2rad

    integer example
    common /swe_example/ example

    integer mapping
    common /swe_mapping_int/ mapping

    integer init_cond
    common /swe_initcond/ init_cond

    double precision ring_inner, ring_outer, center(3)
    common /swe_initcond_parms2/ ring_inner, ring_outer, center

    integer ring_units
    common /swe_initcond_parms3/ ring_units

    double precision hin, hout
    common /swe_initcond_parms4/  hin,hout

    double precision gravity
    common /swe_model_parms/  gravity

    double precision theta_ridge, theta_wave, ampl,  & 
         alpha, bathy(2), speed, gravity_ridge
    common /comm_ridge/ theta_ridge, theta_wave, ampl,  & 
         alpha, bathy, speed, gravity_ridge

    double precision latitude(2), longitude(2)

    pi = 4.d0*atan(1.d0)
    pi2 = 2*pi

    deg2rad = pi/180.0

    open(10,file='setprob_2d.data')
    read(10,*) example
    read(10,*) gravity
    read(10,*) mapping
    read(10,*) init_cond
    read(10,*) hin
    read(10,*) hout
    read(10,*) ring_inner
    read(10,*) ring_outer
    read(10,*) ring_units
    read(10,*) latitude(1)
    read(10,*) latitude(2)
    read(10,*) longitude(1)
    read(10,*) longitude(2)
    read(10,*) center(1)
    read(10,*) center(2)
    read(10,*) center(3)
    read(10,*) theta_wave
    read(10,*) theta_ridge
    read(10,*) gravity_ridge
    read(10,*) ampl
    read(10,*) alpha
    read(10,*) speed
    read(10,*) bathy(1)
    read(10,*) bathy(2)
    close(10)

    if (example .eq. 1) then
        gravity = gravity_ridge
    endif

end
