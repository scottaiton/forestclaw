subroutine clawpack5_qinit(meqn,mbc,mx,my, & 
    xlower,ylower,dx,dy,q,maux,aux)

    implicit none

    integer meqn, mbc, mx, my, maux
    double precision xlower, ylower, dx, dy
    double precision, dimension(meqn,1-mbc:mx+mbc, 1-mbc:my+mbc) :: q
    double precision, dimension(maux,1-mbc:mx+mbc, 1-mbc:my+mbc) :: aux

    integer example
    common /swe_example/ example

    double precision pi, pi2, deg2rad
    common /compi/ pi, pi2, deg2rad

    integer initchoice
    common /swe_initcond/ initchoice

    double precision ring_inner, ring_outer, center(3)
    common /swe_initcond_parms2/ ring_inner, ring_outer, center

    double precision hin, hout
    common /swe_initcond_parms4/  hin,hout

    double precision theta_ridge, theta_wave, ampl,  & 
         alpha, bathy(2), speed, gravity_ridge
    common /comm_ridge/ theta_ridge, theta_wave, ampl,  & 
         alpha, bathy, speed, gravity_ridge

    !! Local variables
    integer i,j
    double precision xc,yc,xlow,ylow,xp,yp,zp, w, qval

    double precision phi, phi0, width
    double precision theta, q1, R, u0, Rsphere, d

    integer blockno, fc2d_clawpack5_get_block
    integer*8 cont, fclaw_map_get_context

    character(len=12) :: fname


    cont = fclaw_map_get_context()

    blockno = fc2d_clawpack5_get_block()    

    do j = 1-mbc,my+mbc
        yc = ylower + (j-0.5)*dy
        ylow = ylower + (j-1)*dy
        do i = 1-mbc,mx+mbc
            xc = xlower + (i-0.5)*dx
            xlow = xlower + (i-1)*dx

            call fclaw_map_2d_c2m(cont,blockno,xc,yc,xp,yp,zp)

            qval = 0
            if (example .eq. 0) then
                if (initchoice .eq. 1) then
                    !! Disk or ring
                    call cellave2(blockno,xlow,ylow,dx,dy,w)
                    qval = w
                elseif (initchoice .eq. 2) then
                    !! RJL initial conditions - matches 1d solution
                    d = (xp*center(1) + yp*center(2) + zp*center(3)) 
                    phi = acos(d)
                    phi0 = deg2rad*20
                    width = deg2rad*5
                    qval = exp(-((phi-phi0)/width)**2) 
                endif
                q(1,i,j) = hout + (hin - hout)*qval
                q(2,i,j) = 0
                q(3,i,j) = 0
                q(4,i,j) = 0
            elseif (example .eq. 1) then
                if (initchoice .ne. 3) then
                    write(6,*) "sphere_qinit : Ridge problem requires initchoice 3"
                    stop
                endif

                !! For ridge problem
                !! theta
                d = xp*center(1) + yp*center(2) + zp*center(3)
                theta = acos(d/Rsphere)
                q1 = exp(-alpha*(theta-theta_wave)**2)
                R = max(sqrt(xp**2 + yp**2), 1.d-10)
                u0 = speed*ampl*q1 / (Rsphere*R)  !! Not sure about 2d2
                q(1,i,j) = -aux(18,i,j)
                !!write(6,*) blockno, q(i,j,1)
                if (q1 .gt. 1.d-20) then
                    q(1,i,j) = q(1,i,j) + ampl*q1
                    q(2,i,j) = u0*xp*zp
                    q(3,i,j) = u0*yp*zp
                    q(4,i,j) = -u0*(xp**2+yp**2)
                endif
                !!write(10,*) theta, aux(i,j,18), q(i,j,1)
            else
                write(6,*) 'sphere_qinit : Example must be 0 or 1'
                stop
            endif 
        enddo
    enddo

    return
end  subroutine clawpack5_qinit

