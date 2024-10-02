subroutine setprob()
    implicit none

    double precision pi, pi2
    common /compi/ pi,pi2

    double precision velocity_vec(3)
    common /comvelocity/ velocity_vec

    open(10,file='setprob.data')
    read(10,*) velocity_vec(1)
    read(10,*) velocity_vec(2)
    read(10,*) velocity_vec(3)
    close(10)

    pi = 4.d0*datan(1.d0)
    pi2 = 2.d0*pi


end subroutine setprob
