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

    double precision theta_ridge, theta_wave, ampl,  & 
         alpha, bathy(2), speed, gravity_ridge
    common /comm_ridge/ theta_ridge, theta_wave, ampl,  & 
         alpha, bathy, speed, gravity_ridge

    integer, intent(in) :: meqn,mbc,mx,maux
    real(kind=8), intent(in) :: xlower,dx
    real(kind=8), intent(in) :: aux(maux,1-mbc:mx+mbc)
    real(kind=8), intent(inout) :: q(meqn,1-mbc:mx+mbc)

    !locals
    integer :: i
    real(kind=8) :: xe, xc,x0, bathy, qval, deg2rad
    real(kind=8) :: dxrad

    real(kind=8) :: eta, width, ri, ro, a, m, phil,phir

    deg2rad = pi/180

    do i = 1,mx
        xe = xlower + (i-1)*dx  ! latitude in degrees
        xc = xlower +  (i-0.5)*dx

        if (init_cond .eq. 1) then            
            !! Flip sign so inner is closer to pi/2
            ri = pi/2 - ring_inner
            ro = pi/2 - ring_outer

            xe = deg2rad*xe
            dxrad = deg2rad*dx
            a = abs(ro-ri)/2  !! half width
            m = (ro+ri)/2
            if (xe+dxrad .lt. m-a .or. xe .gt. m + a) then
                qval = 0
            elseif (m-a .le. xe .and. xe+dxrad .le. m+a) then
                qval = 1
            else
                phil = m - a
                phir = m + a
                if (xe .lt. m-a) then
                    qval = (xe+dxrad - phil)/dxrad
                else
                    qval = (phir - xe)/dxrad
                endif
            endif
        elseif (init_cond .eq. 2) then

            width = pi/2 - deg2rad*5.   ! controls width of Gaussian
            x0 = pi/2 - deg2rad*70.     ! initial location of Gaussian
            bathy = -1
            eta = exp(-((xc-theta_wave)/width)**2)

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
