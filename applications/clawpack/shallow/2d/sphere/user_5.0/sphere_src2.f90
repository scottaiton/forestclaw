subroutine clawpack5_src2(meqn,mbc,mx,my, & 
    xlower,ylower,dx,dy,q,maux,aux,t,dt)
    implicit none

    integer meqn, mbc, mx, my, maux
    double precision xlower, ylower, dx, dy, t, dt
    double precision   q(meqn,1-mbc:mx+mbc,1-mbc:my+mbc)
    double precision aux(maux,1-mbc:mx+mbc,1-mbc:my+mbc)

    double precision grav
    common /swe_model_parms/  grav

    double precision ns(3), qv(meqn), qn, gt, c
    integer i,j,m

    !! This one of at least two places where we can project out 
    !! velocities normal to the sphere. 

    do i=1-mbc,mx+mbc        
        do j=1-mbc,my+mbc

            ns(1) = aux(14,i,j)
            ns(2) = aux(15,i,j)
            ns(3) = aux(16,i,j)

            do m = 1,meqn
                qv(m) = q(i,j,m)
            end do
            qn = qv(2)*ns(1) + qv(3)*ns(2) + qv(4)*ns(3)
            qv(2) = qv(2) - qn*ns(1)
            qv(3) = qv(3) - qn*ns(2)
            qv(4) = qv(4) - qn*ns(3)


            c = aux(17,i,j)
            gt = -2*c*0.5*grav*q(i,j,1)**2
            q(2,i,j) = qv(2) + dt*gt*ns(1)
            q(3,i,j) = qv(3) + dt*gt*ns(2)
            q(4,i,j) = qv(4) + dt*gt*ns(3)
        end do
    end do
    return
end subroutine clawpack5_src2
