subroutine qinit(meqn,mbc,mx,xlower,dx,q,maux,aux)
    implicit none


    ! Set initial conditions for the q array.
    ! This default version simply sets eta = max(h + b,0)

    ! For more specific initial conditions
    !  copy this to an application directory and
    !  loop over all grid cells to set values of q(1:meqn, 1:mx).

    !use geoclaw_module, only: dry_tolerance !uncomment if needed
    !use geoclaw_module, only: grav  !uncomment if needed
    !!use grid_module, only: xcell

    double precision pi, pi2
    common /compi/ pi, pi2

    integer example
    common /swe_example/ example

    integer init_cond
    common /swe_initcond/ init_cond

    double precision ring_inner, ring_outer, center(3)
    common /swe_initcond_parms2/ ring_inner, ring_outer, center

    integer ring_units
    common /swe_initcond_parms3/ ring_units

    double precision hin, hout
    common /swe_initcond_parms4/  hin,hout


    integer, intent(in) :: meqn,mbc,mx,maux
    real(kind=8), intent(in) :: xlower,dx
    real(kind=8), intent(in) :: aux(maux,1-mbc:mx+mbc)
    real(kind=8), intent(inout) :: q(meqn,1-mbc:mx+mbc)

    !locals
    integer :: i
    real(kind=8) :: xe, xc,x0, bathy, qval, deg2rad
    real(kind=8) :: dxrad

    real(kind=8) :: eta, width, ri, ro

    deg2rad = pi/180
    width = 5.   ! controls width of Gaussian
    x0 = 70.     ! initial location of Gaussian
    bathy = -1

    !! Flip sign so inner is closer to pi/2
    ri = ring_inner - pi/2
    ro = ring_outer - pi/2

    do i = 1,mx
        xe = xlower + (i-1)*dx  ! latitude in degrees
        xc = xlower +  (i-0.5)*dx

        if (init_cond .eq. 1) then            
            xe = xe*deg2rad
            dxrad = deg2rad*dx
            if (xe .le. ri .and. ri .lt. xe+dxrad) then
                qval = (xe+dxrad-ri)/dxrad
            elseif (xe .le. ro .and. ro .lt. xe+dxrad) then
                qval = (ro-xe)/dxrad
            elseif (ri .lt. xe .and. xe .lt. ro) then
                qval = 1
            else
                qval = 0
            endif
        elseif (init_cond .eq. 2) then

            eta = exp(-((xc-x0)/width)**2)

            if (eta < 1d-20) eta = 0.d0
            qval = eta
        endif
        q(1,i) = hout + (hin-hout)*qval
        q(2,i) = 0.d0 
        !!write(6,*) 'qinit : ',q(1,i)
   enddo
   !!write(6,*) 'Stopping in qinit'
   !!stop


end subroutine qinit
