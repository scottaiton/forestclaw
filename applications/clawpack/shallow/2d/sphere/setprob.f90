subroutine setprob()
    implicit none

    double precision pi, pi2
    common /compi/ pi, pi2

    integer example
    common /swe_example/ example

    integer mapping
    common /swe_mapping_int/ mapping

    integer init_cond
    common /swe_initcond/ init_cond

    double precision ring_inner, ring_outer
    common /swe_initcond_parms2/ ring_inner, ring_outer

    integer ring_units
    common /swe_initcond_parms3/ ring_units

    double precision hin, hout, speed
    common /swe_initcond_parms4/  hin,hout, speed

    double precision gravity
    common /swe_model_parms/  gravity

    double precision Px, Py, Pz, theta_ridge, ampl, alpha, bathy(2)
    common /comm_ridge/ Px, Py, Pz, theta_ridge, ampl, alpha, bathy

    double precision latitude(2), longitude(2)

    pi = 4.d0*atan(1.d0)
    pi2 = 2*pi

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
    read(10,*) Px
    read(10,*) Py
    read(10,*) Pz
    read(10,*) theta_ridge
    read(10,*) ampl
    read(10,*) alpha
    read(10,*) speed
    read(10,*) bathy(1)
    read(10,*) bathy(2)
    close(10)

end
