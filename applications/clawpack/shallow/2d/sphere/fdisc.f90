double precision function fdisc(blockno,xc,yc)
    implicit none

    double precision xc,yc
    integer blockno

    double precision pi, pi2
    common /compi/ pi, pi2

    integer init_cond
    common /swe_initcond/ init_cond

    double precision ring_inner, ring_outer
    common /swe_initcond_parms2/ ring_inner, ring_outer

    integer*8 cont, fclaw_map_get_context

    double precision xp, yp, zp, rp, m, a,ri, ro
    double precision phi, deg2rad


    cont = fclaw_map_get_context()

    call fclaw2d_map_c2m(cont,blockno,xc,yc,xp,yp,zp)

    rp = sqrt(xp**2 + yp**2 + zp**2)
    !!call map2spherical(xp,yp,zp,theta,phi)


    !! Dot and Normalize with unit vector on x axis : (1,0,0)

    deg2rad = pi/180
    if (init_cond .eq. 1) then
        ri = ring_inner*deg2rad            
        ro = ring_outer*deg2rad
    else
        write(6,*) "fdisc.f90 : Should never be never be here"
        stop
    endif
    phi = acos(xp/rp)  
    a = (ro - ri)/2.0   !! Half-width of the ring
    m = (ro + ri)/2.0   !! Midpoint of the ring
    fdisc = abs(phi-m) - a
end



