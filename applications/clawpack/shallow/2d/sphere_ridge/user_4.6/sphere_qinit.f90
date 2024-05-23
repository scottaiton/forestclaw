subroutine clawpack46_qinit(maxmx,maxmy,meqn,mbc,mx,my, & 
    xlower,ylower,dx,dy,q,maux,aux)

    implicit none

    integer meqn, mbc, mx, my, maux,maxmx,maxmy
    double precision xlower, ylower, dx, dy
    double precision, dimension(1-mbc:mx+mbc, 1-mbc:my+mbc, meqn) :: q
    double precision, dimension(1-mbc:mx+mbc, 1-mbc:my+mbc, maux) :: aux

    integer example
    common /swe_example/ example

    double precision pi, pi2
    common /compi/ pi, pi2

    integer initchoice
    common /swe_initcond/ initchoice

    double precision ring_inner, ring_outer
    common /swe_initcond_parms2/ ring_inner, ring_outer

    double precision hin, hout
    common /swe_initcond_parms4/  hin,hout

    double precision Px, Py, Pz, theta_ridge, theta_wave, & 
                        ampl, alpha, bathy(2), speed
    common /comm_ridge/ Px, Py, Pz, theta_ridge, theta_wave, & 
                        ampl, alpha, bathy, speed

    !! Local variables
    integer i,j
    double precision xc,yc,xlow,ylow,xp,yp,zp, w, qval
    double precision gaussian_sphere

    double precision deg2rad, phi, phi0, width
    double precision theta, q1, R, u0, Rsphere

    integer blockno, fc2d_clawpack46_get_block
    integer*8 cont, fclaw_map_get_context

    character(len=12) :: fname


    cont = fclaw_map_get_context()

    blockno = fc2d_clawpack46_get_block()    
    fname = 'qinit_X.out'
    fname(7:7) = char(ichar('0') + blockno)
    !!write(6,*) fname


    !!open(10,file=fname)
    Rsphere = 1
    deg2rad = pi/180
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
                    phi = acos(min(xp,1.0))
                    phi0 = deg2rad*20
                    width = deg2rad*5
                    qval = exp(-((phi-phi0)/width)**2) 
                endif
                q(i,j,1) = hout + (hin - hout)*qval
                q(i,j,2) = 0
                q(i,j,3) = 0
                q(i,j,4) = 0
            elseif (example .eq. 1) then
                if (initchoice .ne. 3) then
                    write(6,*) "sphere_qinit : Ridge problem requires initchoice 3"
                    stop
                endif

                !! For ridge problem
                !! theta
                theta = asin((xp*Px + yp*Py + zp*Pz) / Rsphere)
                q1 = dexp(-alpha*(theta-theta_wave)**2)
                R = max(sqrt(xp**2 + yp**2), 1.d-10)
                u0 = speed*ampl*q1 / (Rsphere*R)  !! Not sure about 2d2
                q(i,j,1) = -aux(i,j,18)
                !!write(6,*) blockno, q(i,j,1)
                if (q1 .gt. 1.d-20) then
                    q(i,j,1) = q(i,j,1) + ampl*q1
                    q(i,j,2) = u0*xp*zp
                    q(i,j,3) = u0*yp*zp
                    q(i,j,4) = -u0*(xp**2+yp**2)
                endif
                !!write(10,*) theta, aux(i,j,18), q(i,j,1)
            else
                write(6,*) 'sphere_qinit : Example must be 0 or 1'
                stop
            endif 
        enddo
    enddo
    !!close(10)

    return
end  subroutine clawpack46_qinit

