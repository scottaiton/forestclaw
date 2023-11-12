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

    double precision amp
    common /swe_initcond_parms0/ amp

    double precision ring_inner, ring_outer
    common /swe_initcond_parms2/ ring_inner, ring_outer

    double precision hin, hout
    common /swe_initcond_parms4/  hin,hout

    double precision gravity
    common /swe_model_parms/  gravity

    pi = 4.d0*atan(1.d0)
    pi2 = 2*pi

    open(10,file='setprob.data')
    read(10,*) example
    read(10,*) gravity
    read(10,*) mapping
    read(10,*) init_cond
    read(10,*) amp
    read(10,*) hin
    read(10,*) hout
    read(10,*) ring_inner
    read(10,*) ring_outer
    close(10)

end
