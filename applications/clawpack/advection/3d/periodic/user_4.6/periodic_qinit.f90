subroutine clawpack46_qinit(meqn,mbc, mx,my,mz, & 
            xlower,ylower,zlower, dx,dy,dz, q,maux,aux)

    implicit none
    integer meqn, mbc, mx, my, mz, maux
    double precision xlower, ylower, zlower, dx, dy, dz
    double precision   q(1-mbc:mx+mbc, 1-mbc:my+mbc, 1-mbc:mz+mbc, meqn)
    double precision aux(1-mbc:mx+mbc, 1-mbc:my+mbc, 1-mbc:mz+mbc, maux)

    integer i, j, k, mq
    double precision xlow, ylow, zlow, wl
    integer blockno, fc3d_clawpack46_get_block

    blockno = fc3d_clawpack46_get_block()

    do mq = 1,meqn
        do i = 1-mbc,mx+mbc
            xlow = xlower + (i-1)*dx
            do j = 1-mbc,my+mbc
                ylow = ylower + (j-1)*dy
                do k = 1-mbc,mz+mbc
                    zlow = zlower + (k-1)*dz
                    call cellave3(blockno,xlow,ylow,zlow, dx,dy,dz, wl)
                    q(i,j,k,1) = wl
                end do
            end do
        end do
    end do

    return
end subroutine clawpack46_qinit



