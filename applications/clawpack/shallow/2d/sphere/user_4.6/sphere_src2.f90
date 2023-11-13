subroutine clawpack46_src2(maxmx,maxmy,meqn,mbc,mx,my, & 
    xlower,ylower,dx,dy,q,maux,aux,t,dt)
    implicit none

    integer maxmx, maxmy, meqn, mbc, mx, my, maux
    double precision xlower, ylower, dx, dy, t, dt
    double precision   q(1-mbc:maxmx+mbc,1-mbc:maxmy+mbc, meqn)
    double precision aux(1-mbc:maxmx+mbc,1-mbc:maxmy+mbc, maux)

    double precision grav
    common /swe_model_parms/  grav

    double precision ns(3), qv(meqn), qn, gt, c
    integer i,j,m

    !! # dummy source routine... does nothing

    do i=1-mbc,mx+mbc        
        do j=1-mbc,my+mbc

            ns(1) = aux(i,j,14)
            ns(2) = aux(i,j,15)
            ns(3) = aux(i,j,16)

            do m = 1,meqn
                qv(m) = q(i,j,m)
            end do
            !!qn = qv(2)*ns(1) + qv(3)*ns(2) + qv(4)*ns(3)
            !!qv(2) = qv(2) - qn*ns(1)
            !!qv(3) = qv(3) - qn*ns(2)
            !!qv(4) = qv(4) - qn*ns(3)


            c = aux(i,j,17)
            gt = -2*c*0.5*grav*q(i,j,1)**2
            q(i,j,2) = qv(2) + dt*gt*ns(1)
            q(i,j,3) = qv(3) + dt*gt*ns(2)
            q(i,j,4) = qv(4) + dt*gt*ns(3)
        end do
    end do
    return
end
