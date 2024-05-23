!! This routine computes the flux at cell-centered value
!! 
!!      flux(1) = hu
!!      flux(2) = hu^2 + 0.5*g*h^2
!!      flux(3) = huv
!!      flux(4) = huw
!!

!! All scaling and potential sign changes are handled by the 
!! calling routine.


subroutine  rpn2cons_update(meqn,maux, idir, iface, q, & 
    auxvec_center, auxvec_edge,flux)
    implicit none

    integer meqn,maux,idir, iface
    double precision q(meqn), flux(meqn)

    !! # maux == 0 in this routine.      
    double precision auxvec_center(maux), auxvec_edge(maux)

    double precision grav
    common /swe_model_parms/  grav

    double precision hun, hut, h, un, ut

    integer ioff
    double precision enx, eny, enz
    double precision etx, ety, etz
    double precision erx,ery,erz
    double precision gamma, apn
    double precision flux_rot(meqn)

    integer i


    if (idir .eq. 0) then
        ioff = 1
    else
        ioff = 7
    endif

    enx = auxvec_center(ioff+1)
    eny = auxvec_center(ioff+2)
    enz = auxvec_center(ioff+3)
    etx = auxvec_center(ioff+4)
    ety = auxvec_center(ioff+5)
    etz = auxvec_center(ioff+6)
    gamma = sqrt(etx**2 + ety**2 + etz**2)
    etx =   etx / gamma
    ety =   ety / gamma
    etz =   etz / gamma

    h = q(1)
    hun = enx*q(2) + eny*q(3) + enz*q(4)
    hut = etx*q(2) + ety*q(3) + etz*q(4)


    erx = auxvec_center(14)
    ery = auxvec_center(15)
    erz = auxvec_center(16)
    apn = erx*q(2) + ery*q(3) + erz*q(4)
    write(6,*) 'here ... 1'
    if (abs(apn) .gt. 1e-13) then
        write(6,*) 'rpn2cons_update : apn > 0; ',apn
        stop
    endif

    return

    un = hun/h
    ut = hut/h

    flux(1) = hun
    flux_rot(2) = hun**2/h + 0.5*grav*h**2
    flux_rot(3) = un*hut
    flux_rot(4) = un*q(4)

    flux(2) = enx*flux_rot(2) + etx*flux_rot(3)
    flux(3) = eny*flux_rot(2) + ety*flux_rot(3)
    flux(4) = enz*flux_rot(2) + etz*flux_rot(3)

end


