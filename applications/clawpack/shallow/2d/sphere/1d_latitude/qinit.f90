subroutine qinit(meqn,mbc,mx,xlower,dx,q,maux,aux)

    ! Set initial conditions for the q array.
    ! This default version simply sets eta = max(h + b,0)

    ! For more specific initial conditions
    !  copy this to an application directory and
    !  loop over all grid cells to set values of q(1:meqn, 1:mx).

    !use geoclaw_module, only: dry_tolerance !uncomment if needed
    !use geoclaw_module, only: grav  !uncomment if needed
    use grid_module, only: xcell

    implicit none

    integer, intent(in) :: meqn,mbc,mx,maux
    real(kind=8), intent(in) :: xlower,dx
    real(kind=8), intent(in) :: aux(maux,1-mbc:mx+mbc)
    real(kind=8), intent(inout) :: q(meqn,1-mbc:mx+mbc)

    !locals
    integer :: i
    real(kind=8) :: x,x0, bathy

    real(kind=8) :: eta, width

    width = 5.   ! controls width of Gaussian
    x0 = 70.     ! initial location of Gaussian
    bathy = -1

    do i = 1,mx
        x = xcell(i)  ! latitude in degrees
        !!if (abs(x-20) .lt. 5) then
        !!    eta = 1
        !!else
        !!    eta = 0
        !!endif

        eta = exp(-((x-x0)/width)**2)

        if (eta < 1d-20) eta = 0.d0
        !!q(1,i) = max(0.0, eta - aux(1,i))
        q(1,i) = eta - bathy
        q(2,i) = 0.d0 
   enddo


end subroutine qinit
