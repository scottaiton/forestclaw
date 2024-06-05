double precision function bmount(blockno,xc,yc)
    implicit none

    double precision xc,yc
    integer blockno

    integer example
    common /swe_example/ example

    double precision pi,pi2
    common /compi/ pi, pi2

    double precision ring_inner, ring_outer, center(3)
    common /swe_initcond_parms2/ ring_inner, ring_outer, center
    
    double precision theta_ridge, theta_wave, ampl,  & 
         alpha, bathy(2), speed, gravity_ridge
    common /comm_ridge/ theta_ridge, theta_wave, ampl,  & 
         alpha, bathy, speed, gravity_ridge

    double precision xp, yp, zp, theta, d
    integer*8 cont, fclaw_map_get_context

    double precision :: qb

    if (example .eq. 0) then
        bmount = -1
    else

        cont = fclaw_map_get_context()
        call fclaw_map_2d_c2m(cont,blockno,xc,yc,xp,yp,zp)    

        !! # Get lat/long coordinates

        d = xp*center(1) + yp*center(2) + zp*center(3)
        if (abs(d) .gt. 1) then
            write(6,*) "bmount : Point is not on the sphere"
            write(6,*) 'blockno = ',blockno
            write(6,100) xc,yc
            write(6,*) xp,yp,zp
            write(6,*) center(1), center(2), center(3)
            write(6,*) "d = ", d
            stop
        endif
        theta = asin(d) !! in [-pi/2, pi/2]
        qb = exp(-alpha*(theta-theta_ridge)**2)
        bmount = -bathy(1) + bathy(2)*qb
    endif

    return
    
100 format(3F24.16)                
end function bmount
