subroutine setaux(mbc,mx,xlower,dx,maux,aux)

    use geoclaw_module, only: earth_radius, coordinate_system, DEG2RAD
    use grid_module, only: xp_edge,mx_edge
    use topo_module, only: zcell

    implicit none

    integer mbc, mx, maux
    double precision xlower, dx
    double precision :: aux(maux,1-mbc:mx+mbc)

    double precision pi, pi2
    common /compi/ pi, pi2

    integer i
    double precision :: xc
    double precision :: bmount

    do i=1-mbc,mx+mbc
        aux(1,i) = zcell(i)        
        aux(2,i) = (xp_edge(i+1) - xp_edge(i))/dx
        if (coordinate_system == 2) then
            aux(2,i) = aux(2,i)* DEG2RAD * earth_radius            
        endif
    enddo


    do i = 1-mbc, mx+mbc
        xc = xlower + (i-0.5)*dx
        aux(1,i) = bmount(DEG2RAD*xc)
    end do
    return
end subroutine setaux
