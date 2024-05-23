double precision function bmount(blockno,xc,yc)
    implicit none

    double precision xc,yc
    integer blockno

    integer example
    common /swe_example/ example

    double precision pi,pi2
    common /compi/ pi, pi2

    double precision Px, Py, Pz, theta_ridge, theta_wave, & 
                        ampl, alpha, bathy(2), speed
    common /comm_ridge/ Px, Py, Pz, theta_ridge, theta_wave, & 
                        ampl, alpha, bathy, speed

    double precision xp, yp, zp, theta, d, tr
    integer*8 cont, fclaw_map_get_context

    double precision :: qb

    if (example .eq. 0) then
        bmount = -1
    else

        cont = fclaw_map_get_context()
        call fclaw_map_2d_c2m(cont,blockno,xc,yc,xp,yp,zp)    

        !! # Get lat/long coordinates

        d = xp*Px + yp*Py + zp*Pz
        if (abs(d) .gt. 1) then
            write(6,*) "bmount : Point is not on the sphere"
            write(6,*) 'blockno = ',blockno
            write(6,100) xc,yc
            write(6,*) xp,yp,zp
            write(6,*) Px, Py, Pz
            write(6,*) "d = ", d
            stop
        endif
        theta = asin(d) !! in [-pi/2, pi/2]
        qb = exp(-alpha*(theta-theta_ridge)**2)
        bmount = -bathy(1) + bathy(2)*qb
100 format(3F24.16)                
    endif

    return
end function bmount
