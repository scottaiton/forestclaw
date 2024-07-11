double precision function bmount(xc)
    implicit none

    double precision xc

    double precision pi,pi2, deg2rad
    common /compi/ pi, pi2, deg2rad

    double precision theta_ridge, theta_wave, ampl,  & 
         alpha, bathy(2), speed, gravity_ridge
    common /comm_ridge/ theta_ridge, theta_wave, ampl,  & 
         alpha, bathy, speed, gravity_ridge

    double precision :: qb, theta

    theta = pi/2 - deg2rad*xc
    qb = dexp(-alpha*(theta-theta_ridge)**2)
    bmount = -bathy(1) + bathy(2)*qb;
    if (bmount .gt. 0) then
        write(6,*) 'bmount : bmount > 0;  ', bmount
    endif

    return
end
