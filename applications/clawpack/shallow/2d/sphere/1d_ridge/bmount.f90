double precision function bmount(xc)
    implicit none

    double precision xc

    double precision pi,pi2
    common /compi/ pi, pi2

    double precision Px, Py, Pz, theta_ridge, theta_wave, & 
                        ampl, alpha, bathy(2), speed
    common /comm_ridge/ Px, Py, Pz, theta_ridge, theta_wave, & 
                        ampl, alpha, bathy, speed

    double precision :: qb

    qb = dexp(-alpha*(xc-theta_ridge)**2)
    bmount = -bathy(1) + bathy(2)*qb;
    if (bmount .gt. 0) then
        write(6,*) 'bmount : bmount > 0;  ', bmount
    endif

    return
end
