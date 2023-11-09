subroutine setprob()
    implicit none

    double precision pi, pi2
    common /compi/ pi, pi2

    integer example
    common /swe_example/ example

    integer mapping
    common /swe_mapping_int/ mapping

    double precision omega(3)
    common /swe_mapping_float/ omega

    integer init_cond
    common /swe_init_cond/ init_cond

    double precision grav
    common /swe_parms/  grav

    double precision r0, hin, hout
    common /swe_init_parms/  r0, hin,hout

    pi = 4.d0*atan(1.d0)
    pi2 = 2*pi

    open(10,file='setprob.data')

    read(10,*) example
    read(10,*) grav
    read(10,*) mapping
    read(10,*) init_cond
    read(10,*) omega(1)
    read(10,*) omega(2)
    read(10,*) omega(3)
    read(10,*) r0
    read(10,*) hin
    read(10,*) hout
    close(10)

end
