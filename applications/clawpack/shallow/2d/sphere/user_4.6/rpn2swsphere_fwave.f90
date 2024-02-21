! =====================================================
SUBROUTINE clawpack46_rpn2_fwave(ixy,maxm,meqn,mwaves,maux, &
    mbc, mx,ql,qr,auxl,auxr,fwave,s,amdq,apdq)
! =====================================================

! Roe-solver for the 2D shallow water equations
!  on the sphere, using 3d Cartesian representation of velocities

! waves: 3
! equations: 4
! aux fields: 16

! Conserved quantities:
!       1 depth
!       2 x_momentum
!       3 y_momentum
!       4 z_momentum

! Auxiliary variables:
!         1  kappa
!         2  enx
!         3  eny
!         4  enz
!         5  etx
!         6  ety
!         7  etz
!         8  enx
!         9  eny
!        10  enz
!        11  etx
!        12  ety
!        13  etz
!        14  erx
!        15  ery
!        16  erz

! solve Riemann problems along one slice of data.

! On input, ql contains the state vector at the left edge of each cell
!           qr contains the state vector at the right edge of each cell

! This data is along a slice in the x-direction if ixy=1
!                            or the y-direction if ixy=2.
! On output, wave contains the waves, s the speeds,
! and amdq, apdq the decomposition of the flux difference
!   f(qr(i-1)) - f(ql(i))
! into leftgoing and rightgoing parts respectively.
! With the Roe solver we have
!    amdq  =  A^- \Delta q    and    apdq  =  A^+ \Delta q
! where A is the Roe matrix.  An entropy fix can also be incorporated
! into the flux differences.

! Note that the i'th Riemann problem has left state qr(i-1,:)
!                                    and right state ql(i,:)
! From the basic clawpack routines, this routine is called with ql = qr


    implicit none

    integer :: ixy, maxm,meqn,mwaves,mbc,mx, maux
    double precision  :: fwave(1-mbc:maxm+mbc,meqn,mwaves)
    double precision  ::    s(1-mbc:maxm+mbc,mwaves)
    double precision  ::   ql(1-mbc:maxm+mbc,meqn)
    double precision  ::   qr(1-mbc:maxm+mbc,meqn)
    double precision  :: apdq(1-mbc:maxm+mbc,meqn)
    double precision  :: amdq(1-mbc:maxm+mbc,meqn)
    double precision  :: auxl(1-mbc:maxm+mbc,maux)
    double precision  :: auxr(1-mbc:maxm+mbc,maux)

    !! # local arrays
    !! # -----------
    !! # double precision :: delta(3)
    logical :: efix

!
!    integer :: maxm2
!!    parameter (maxm2 = 1800)
!!    double precision :: u(1-mbc:maxm+mbc),v(1-mbc:maxm+mbc)
!!    double precision :: a(1-mbc:maxm+mbc)
!!    double precision :: h(1-mbc:maxm+mbc)

    double precision :: dtcom, dxcom, dycom, tcom
    integer :: icom, jcom
    common /comxyt/ dtcom,dxcom,dycom,tcom,icom,jcom

    double precision grav
    common /swe_model_parms/  grav


    integer :: i, m, mw, mq, ioff
    double precision :: enx, eny, enz, etx,ety,etz
    double precision :: hunl, hunr, hutl, hutr
!!    double precision :: hsqr,hsql,hsq
!!    double precision :: a1,a2,a3, 
    double precision :: gamma, amn, apn, df, dy
    double precision :: erx, ery, erz, h1, h3, hi, him1, hu1, hu3
    double precision :: s0, s03, s1, s3, sfract

    double precision :: hL, hR, huL, huR, bL, bR, hvL, hvR
    double precision :: uL, vL, uR, vR, phiR, phiL, sL, sR
    double precision :: uhat, chat
    double precision :: sw(mwaves), fw(meqn,mwaves)
    double precision szm(3), szp(3), z


    double precision :: smax

    integer mbathy 

    data efix /.false./    !# use entropy fix for transonic rarefactions

    if(ixy == 1) then
        dy = dycom
    else
        dy = dxcom
    endif

    !! The aux array has the following elements:
    !!  1  kappa = ratio of cell area to dxc*dyc
    !!  2  enx = x-component of normal vector to left edge in tangent plane
    !!  3  eny = y-component of normal vector to left edge in tangent plane
    !!  4  enz = z-component of normal vector to left edge in tangent plane
    !!  5  etx = x-component of tangent vector to left edge in tangent plane
    !!  6  ety = y-component of tangent vector to left edge in tangent plane
    !!  7  etz = z-component of tangent vector to left edge in tangent plane
    !!  8  enx = x-component of normal vector to bottom edge in tangent plane
    !!  9  eny = y-component of normal vector to bottom edge in tangent plane
    !! 10  enz = z-component of normal vector to bottom edge in tangent plane
    !! 11  etx = x-component of tangent vector to bottom edge in tangent plane
    !! 12  ety = y-component of tangent vector to bottom edge in tangent plane
    !! 13  etz = z-component of tangent vector to bottom edge in tangent plane
    !! 14  erx = x-component of unit vector in radial direction at cell ctr
    !! 15  ery = y-component of unit vector in radial direction at cell ctr
    !! 16  erz = z-component of unit vector in radial direction at cell ctr

    !! # offset to index into aux array for enx, eny, etx, ety, gamma
    !! #    depends on whether ixy=1 (left edge) or ixy=2 (bottom edge).

    !! Index needed to get to aux array variables.
    ioff = 6*(ixy-1) + 1


    !! # find a1 thru a3, the coefficients of the 3 eigenvectors:

    smax = 0
    mbathy = 19
    do i = 2-mbc, mx+mbc

        enx =   auxl(i,ioff+1)
        eny =   auxl(i,ioff+2)
        enz =   auxl(i,ioff+3)
        etx =   auxl(i,ioff+4)
        ety =   auxl(i,ioff+5)
        etz =   auxl(i,ioff+6)

        !! Normalize edge
        gamma = dsqrt(etx**2 + ety**2 + etz**2)
        etx =   etx / gamma
        ety =   ety / gamma
        etz =   etz / gamma

        !!  # compute normal and tangential momentum at cell edge:
        hunl = enx*ql(i,2)   + eny*ql(i,3)   + enz*ql(i,4)
        hunr = enx*qr(i-1,2) + eny*qr(i-1,3) + enz*qr(i-1,4)

        hutl = etx*ql(i,2)   + ety*ql(i,3)   + etz*ql(i,4)
        hutr = etx*qr(i-1,2) + ety*qr(i-1,3) + etz*qr(i-1,4)

        !! Get Riemann variables Riemann problem variables
        hL  = qr(i-1,1)
        hR  = ql(i,1)

        huL = hunr
        huR = hunL

        bL  = auxr(i-1,mbathy)
        bR  = auxl(i,mbathy)

        hvL = hutR
        hvR = hutL

        uR = huR/hR
        vR = hvR/hR
        phiR = 0.5d0*grav*hR**2 + huR**2/hR

        uL = huL/hL
        vL = hvL/hL
        phiL = 0.5d0*grav*hL**2 + huL**2/hL


        !! Start new code
        sL = uL - SQRT(grav*hL) !! 1 wave speed of left state
        sR = uR + SQRT(grav*hR) !! 2 wave speed of right state

        uhat = (SQRT(grav*hL)*uL + SQRT(grav*hR)*uR)/(SQRT(grav*hR) + SQRT(grav*hL)) 
        chat = SQRT(grav*0.5d0*(hR + hL)) 

        CALL  simple_riemann(hR,uR,vR, hL,uL,vl, uhat,chat,bL, bR, &
                             phiR,phiL,sw,fw)

      DO mw = 1,mwaves
            s(i,mw) = gamma*sw(mw)/dy
            fwave(i,1,mw) = fw(1,mw)
            fwave(i,2,mw) = fw(2,mw)*enx + fw(3,mw)*etx
            fwave(i,3,mw) = fw(2,mw)*eny + fw(3,mw)*ety
            fwave(i,4,mw) = fw(2,mw)*enz + fw(3,mw)*etz
        ENDDO
    end do

    !! # no entropy fix
    !! ----------------

    if (efix) go to 110


    do i = 2-mbc,mx+mbc

        !! Convert +1 --> 1; -1 --> 0
        do mw = 1,mwaves
            z = sign(1.d0,s(i,mw))
            szm(mw) = (1-z)/2
            szp(mw) = (1+z)/2
        end do

        !! # amdq = SUM s*wave   over left-going waves
        !! # apdq = SUM s*wave   over right-going waves

        do mq = 1,meqn
            amdq(i,mq) =              szm(1)*fwave(i,mq,1)
            amdq(i,mq) = amdq(i,mq) + szm(2)*fwave(i,mq,2)
            amdq(i,mq) = amdq(i,mq) + szm(3)*fwave(i,mq,3)

            apdq(i,mq) =              szp(1)*fwave(i,mq,1)
            apdq(i,mq) = apdq(i,mq) + szp(2)*fwave(i,mq,2)
            apdq(i,mq) = apdq(i,mq) + szp(3)*fwave(i,mq,3)
        enddo 
    end do

    !! Continue to section where we project out any momentum in 
    !! the normal direction
    goto 900




!!-----------------------------------------------------

    110 continue

    !! # With entropy fix
    !! # ------------------

    !! # compute flux differences amdq and apdq.
    !! # First compute amdq as sum of s*wave for left going waves.
    !! # Incorporate entropy fix by adding a modified fraction of wave
    !! # if s should change sign.

    do i = 2-mbc,mx+mbc
        do mw = 1,mwaves
            z = sign(1.d0,s(i,mw))
            szm(mw) = (1-z)/2
            szp(mw) = (1+z)/2
        end do

        do m = 1, meqn
            amdq(i,m) = 0.d0
            apdq(i,m) = 0.d0
        enddo
        enx =   auxl(i,ioff+1)
        eny =   auxl(i,ioff+2)
        enz =   auxl(i,ioff+3)
        etx =   auxl(i,ioff+4)
        ety =   auxl(i,ioff+5)
        etz =   auxl(i,ioff+6)
        gamma = dsqrt(etx**2 + ety**2 + etz**2)
        etx =   etx / gamma
        ety =   ety / gamma
        etz =   etz / gamma

        !! # compute normal and tangential momentum at cell edge:
        hunl = enx*ql(i,2)   + eny*ql(i,3) + enz*ql(i,4)
        hunr = enx*qr(i-1,2) + eny*qr(i-1,3) + enz*qr(i-1,4)

        !! check 1-wave
        him1 = qr(i-1,1)
        s0 =  (hunr/him1 - dsqrt(grav*him1)) * gamma / dy
        !! check for fully supersonic case :
        if (s0 > 0.0d0 .AND. s(i,1) > 0.0d0) then
            do m = 1,4
                amdq(i,m)=0.0d0
            end do
            goto 200
        endif

        h1 = qr(i-1,1)+fw(1,1)
        hu1 = hunr + enx*fw(2,1) + eny*fw(3,1) + enz*fw(4,1)
        s1 = (hu1/h1 - dsqrt(grav*h1))*gamma/dy  !speed just to right of 1-wave
        if (s0 < 0.0d0 .AND. s1 > 0.0d0) then
            !! transonic rarefaction in 1-wave
            sfract = s0*((s1-s(i,1))/(s1-s0))
        else if (s(i,1) < 0.0d0) then
            !! 1-wave is leftgoing
            sfract = s(i,1)
        else
            !! 1-wave is rightgoing
            sfract = 0.0d0
        endif
        do m = 1,meqn
            amdq(i,m) = sfract*fw(m,1)
        end do
        !! check 2-wave
        if (s(i,2) > 0.0d0) then
            !!	#2 and 3 waves are right-going
            go to 200
        endif

        do m=1,4
            amdq(i,m) = amdq(i,m) + s(i,2)*fw(m,2)
        end do

        !! check 3-wave

        hi = ql(i,1)
        s03 = (hunl/hi + dsqrt(grav*hi)) * gamma/dy
        h3 = ql(i,1)-fw(1,3)
        hu3 = hunl-(enx*fw(2,3)+eny*fw(3,3)+enz*fw(4,3))
        s3 = (hu3/h3 + dsqrt(grav*h3)) * gamma/dy
        if (s3 < 0.0d0 .AND. s03 > 0.0d0) then
            !! transonic rarefaction in 3-wave
            sfract = s3*((s03-s(i,3))/(s03-s3))
        else if (s(i,3) < 0.0d0) then
            !! 3-wave is leftgoing
            sfract = s(i,3)
        else
            !! 3-wave is rightgoing
            goto 200
        endif
        do m=1,4
            amdq(i,m) = amdq(i,m) + sfract*fw(m,3)
        end do
    200 END DO

!           compute rightgoing flux differences :

    do i = 2-mbc,mx+mbc
        do m=1,4
            df = 0.0d0
            do mw=1,mwaves
                df = df + sign(1.d0,s(i,mw))*fw(m,mw)
            end do
            apdq(i,m)=df - amdq(i,m)
        END DO
    end do

900 continue

    !! if you don't want to project out momentum in direction
    !! of surface normals, you can 
    !! return

    !! Project out momentum in direction normal to the surface
    do i=2-mbc,mx+mbc
        !! project momentum components onto tangent plane
        erx = auxr(i-1,14)
        ery = auxr(i-1,15)
        erz = auxr(i-1,16)
        amn = erx*amdq(i,2)+ery*amdq(i,3)+erz*amdq(i,4)
        amdq(i,2) = amdq(i,2) - amn*erx
        amdq(i,3) = amdq(i,3) - amn*ery
        amdq(i,4) = amdq(i,4) - amn*erz

        erx = auxl(i,14)
        ery = auxl(i,15)
        erz = auxl(i,16)
        apn = erx*apdq(i,2)+ery*apdq(i,3)+erz*apdq(i,4)
        apdq(i,2) = apdq(i,2) - apn*erx
        apdq(i,3) = apdq(i,3) - apn*ery
        apdq(i,4) = apdq(i,4) - apn*erz

    END DO

    return
END SUBROUTINE clawpack46_rpn2_fwave


SUBROUTINE simple_riemann(hr,ur,vr, hl,ul,vl, uhat,chat,bl, br, &
                 phir,phil,s,fwave)
    IMPLICIT NONE

    DOUBLE PRECISION :: hr,ur,vr, hl,ul,vl, uhat, chat, phir, &
               phil,s(3), fwave(3,3), bl, bR

    double precision grav
    common /swe_model_parms/  grav

    DOUBLE PRECISION :: fluxdiff(3),beta(3), hbar

    fwave = 0
    s = 0

    hbar = 0.5 * (hr + hl)

    !! # Flux differences
    fluxdiff(1) = (hr * ur) - (hl * ul)
    fluxdiff(2) = phir - phil + grav * hbar * (br - bl)
    fluxdiff(3) = hr * ur * vr - hl * ul * vl

    !! # Wave speeds
    s(1) = MIN(ul - SQRT(grav * hl), uhat - chat)
    s(3) = MAX(ur + SQRT(grav * hr), uhat + chat)
    s(2) = 0.5d0 * (s(1) + s(3))
        
    !! Wave strengths
    beta(1) = -(fluxdiff(2) - s(3) * fluxdiff(1)) / (s(3) - s(1))
    beta(3) =  (fluxdiff(2) - s(1) * fluxdiff(1)) / (s(3) - s(1))
    beta(2) =   fluxdiff(3) - beta(1)*vl - beta(3)*vr

    !! # Flux waves = beta*R
    fwave(1,1) = beta(1)
    fwave(2,1) = beta(1)*s(1)
    fwave(3,1) = beta(1)*vl

    fwave(1,2) = 0
    fwave(2,2) = 0
    fwave(3,2) = beta(2)

    fwave(1,3) = beta(3)
    fwave(2,3) = beta(3)*s(3)
    fwave(3,3) = beta(3)*vr

END subroutine simple_riemann  
