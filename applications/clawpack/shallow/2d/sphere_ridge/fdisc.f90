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

    integer ring_units
    common /swe_initcond_parms3/ ring_units

    integer*8 cont, fclaw_map_get_context

    double precision xp, yp, zp, rp, m, a
    double precision phi, deg2rad, ri, ro


    cont = fclaw_map_get_context()

    call fclaw_map_2d_c2m(cont,blockno,xc,yc,xp,yp,zp)

    rp = sqrt(xp**2 + yp**2 + zp**2)
    !!write(6,*) xp,yp,zp,rp
    !!call map2spherical(xp,yp,zp,theta,phi)

    deg2rad = pi/180

    ri = ring_inner
    ro = ring_outer
    if (ring_units .gt. 0) then
        if (ring_units .eq. 1) then
            !! Convert degrees to radians
            ri = ri*deg2rad
            ro = ro*deg2rad
        elseif (ring_units .eq. 2) then
            !! Compute Gaussian distance to radians
            ri = ri/rp
            ro = ro/rp
        endif
    endif

    !! The ring is centered at (1,0,0).  
    if (abs(xp/rp) > 1) then
        write(6,*) 'fdisc : problem taking acos()'
        stop
    endif
    phi = asin(xp/rp)  
    a = (ro - ri)/2.0   !! Half-width of the ring
    m = (ro + ri)/2.0   !! Midpoint of the ring
    fdisc = abs(phi-m) - a
end



