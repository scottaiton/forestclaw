SUBROUTINE clawpack5_rpn2_fwave(ixy,maxm,meqn,mwaves, maux, &
    mbc, mx,ql,qr,auxl,auxr,fwave,s,amdq,apdq)

!! Roe-solver for the 2D shallow water equations
!!  on the sphere, using 3d Cartesian representation of velocities
!!
!! waves: 3
!! equations: 4
!! aux fields: 18
!!
!! Conserved quantities:
!!       1 depth
!!       2 x_momentum
!!       3 y_momentum
!!       4 z_momentum
!!
    implicit none

    integer :: ixy, maxm,meqn,mwaves,mbc,mx, maux
    double precision  :: fwave(meqn,mwaves,1-mbc:maxm+mbc)
    double precision  ::     s(mwaves,1-mbc:maxm+mbc)
    double precision  ::   ql(meqn,1-mbc:maxm+mbc)
    double precision  ::   qr(meqn,1-mbc:maxm+mbc)
    double precision  :: apdq(meqn,1-mbc:maxm+mbc)
    double precision  :: amdq(meqn,1-mbc:maxm+mbc)
    double precision  :: auxl(maux,1-mbc:maxm+mbc)
    double precision  :: auxr(maux,1-mbc:maxm+mbc)

    !! # local arrays
    !! # -----------
    !! # double precision :: delta(3)
    logical :: efix

!
    double precision :: dtcom, dxcom, dycom, tcom
    integer :: icom, jcom
    common /comxyt/ dtcom,dxcom,dycom,tcom,icom,jcom

    double precision grav
    common /swe_model_parms/  grav


    integer :: i, m, mw, mq, ioff
    double precision :: enx, eny, enz, etx,ety,etz
    double precision :: hunl, hunr, hutl, hutr
    double precision :: gamma, amn, apn, df, dy, qn
    double precision :: erx, ery, erz, h1, h3, hi, him1, hu1, hu3
    double precision :: s0, s03, s1, s3, sfract

    double precision :: hL, hR, huL, huR, bL, bR, hvL, hvR
    double precision :: uL, vL, uR, vR, phiR, phiL, sL, sR
    double precision :: uhat, chat
    double precision :: sw(mwaves), fw(meqn,mwaves)
    double precision szm(3), szp(3), z, sdk


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
    mbathy = 18
    do i = 2-mbc, mx+mbc

        enx =   auxl(ioff+1,i)
        eny =   auxl(ioff+2,i)
        enz =   auxl(ioff+3,i)
        etx =   auxl(ioff+4,i)
        ety =   auxl(ioff+5,i)
        etz =   auxl(ioff+6,i)

        !! Normalize edge
        gamma = dsqrt(etx**2 + ety**2 + etz**2)
        etx =   etx / gamma
        ety =   ety / gamma
        etz =   etz / gamma

        !! # projection to the sphere  (already done in src2)
        erx = auxl(14,i)
        ery = auxl(15,i)
        erz = auxl(16,i)
!!        qn = erx*ql(i,2) + ery*ql(i,3) + erz*ql(i,4)
!!        ql(i,2) = ql(i,2) - qn*erx
!!        ql(i,3) = ql(i,3) - qn*ery
!!        ql(i,4) = ql(i,4) - qn*erz
!!        qr(i,2) = ql(i,2)
!!        qr(i,3) = ql(i,3)
!!        qr(i,4) = ql(i,4)


        !!  # compute normal and tangential momentum at cell edge:
        huR = enx*ql(2,i)   + eny*ql(3,i)   + enz*ql(4,i)
        huL = enx*qr(2,i-1) + eny*qr(3,i-1) + enz*qr(4,i-1)

        hvR = etx*ql(2,i)   + ety*ql(3,i)   + etz*ql(4,i)
        hvL = etx*qr(2,i-1) + ety*qr(3,i-1) + etz*qr(4,i-1)

        !! Get Riemann variables Riemann problem variables
        hL  = qr(1,i-1)
        hR  = ql(1,i)

        bL  = auxr(mbathy,i-1)
        bR  = auxl(mbathy,i)

        CALL  simple_riemann(hR,huR,hvR, bR, hL,huL,hvL,bL, sw,fw)

        sdk = gamma/dy
        DO mw = 1,mwaves
            s(mw,i) = sdk*sw(mw)
            fwave(1,mw,i) = sdk*fw(1,mw)
            fwave(2,mw,i) = sdk*(fw(2,mw)*enx + fw(3,mw)*etx)
            fwave(3,mw,i) = sdk*(fw(2,mw)*eny + fw(3,mw)*ety)
            fwave(4,mw,i) = sdk*(fw(2,mw)*enz + fw(3,mw)*etz)
        ENDDO
    end do

    !! # no entropy fix
    !! ----------------

    if (efix) go to 110

    do i = 2-mbc,mx+mbc

        do mq=1,meqn
            amdq(mq,i) = 0.d0
            apdq(mq,i) = 0.d0
            do mw=1,mwaves
                if (s(mw,i) .lt. 0.d0) then
                    amdq(mq,i) = amdq(mq,i) + fwave(mq,mw,i)
                else
                    apdq(mq,i) = apdq(mq,i) + fwave(mq,mw,i)
                endif
            enddo
        end do

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
            z = sign(1.d0,s(mw,i))
            szm(mw) = (1-z)/2
            szp(mw) = (1+z)/2
        end do

        do m = 1, meqn
            amdq(m,i) = 0.d0
            apdq(m,i) = 0.d0
        enddo
        enx =   auxl(ioff+1,i)
        eny =   auxl(ioff+2,i)
        enz =   auxl(ioff+3,i)
        etx =   auxl(ioff+4,i)
        ety =   auxl(ioff+5,i)
        etz =   auxl(ioff+6,i)
        gamma = dsqrt(etx**2 + ety**2 + etz**2)
        etx =   etx / gamma
        ety =   ety / gamma
        etz =   etz / gamma

        !! # compute normal and tangential momentum at cell edge:
        hunl = enx*ql(2,i)   + eny*ql(3,i)   + enz*ql(4,i)
        hunr = enx*qr(2,i-1) + eny*qr(3,i-1) + enz*qr(4,i-1)

        !! check 1-wave
        him1 = qr(1,i-1)
        s0 =  (hunr/him1 - dsqrt(grav*him1)) * gamma / dy
        !! check for fully supersonic case :
        if (s0 > 0.0d0 .AND. s(1,i) > 0.0d0) then
            do m = 1,4
                amdq(m,i)=0.0d0
            end do
            goto 200
        endif

        h1 = qr(1,i-1)+fw(1,1)
        hu1 = hunr + enx*fw(1,2) + eny*fw(1,3) + enz*fw(1,4)
        s1 = (hu1/h1 - dsqrt(grav*h1))*gamma/dy  !speed just to right of 1-wave
        if (s0 < 0.0d0 .AND. s1 > 0.0d0) then
            !! transonic rarefaction in 1-wave
            sfract = s0*((s1-s(1,i))/(s1-s0))
        else if (s(1,i) < 0.0d0) then
            !! 1-wave is leftgoing
            sfract = s(1,i)
        else
            !! 1-wave is rightgoing
            sfract = 0.0d02
        endif
        do m = 1,meqn
            amdq(m,i) = sfract*fw(1,m)
        end do
        !! check 2-wave
        if (s(2,i) > 0.0d0) then
            !!	#2 and 3 waves are right-going
            go to 200
        endif

        do m=1,4
            amdq(m,i) = amdq(m,i) + s(2,i)*fw(2,m)
        end do

        !! check 3-wave

        hi = ql(1,i)
        s03 = (hunl/hi + dsqrt(grav*hi)) * gamma/dy
        h3 = ql(1,i)-fw(3,1)
        hu3 = hunl-(enx*fw(3,2)+eny*fw(3,3)+enz*fw(3,4))
        s3 = (hu3/h3 + dsqrt(grav*h3)) * gamma/dy
        if (s3 < 0.0d0 .AND. s03 > 0.0d0) then
            !! transonic rarefaction in 3-wave
            sfract = s3*((s03-s(3,i))/(s03-s3))
        else if (s(3,i) < 0.0d0) then
            !! 3-wave is leftgoing
            sfract = s(3,i)
        else
            !! 3-wave is rightgoing
            goto 200
        endif
        do m=1,4
            amdq(m,i) = amdq(m,i) + sfract*fw(3,m)
        end do
    200 END DO

!           compute rightgoing flux differences :

    do i = 2-mbc,mx+mbc
        do m=1,4
            df = 0.0d0
            do mw=1,mwaves
                df = df + sign(1.d0,s(mw,i))*fw(m,mw)
            end do
            apdq(m,i)=df - amdq(m,i)
        END DO
    end do

900 continue

    !! if you don't want to project out momentum in direction
    !! of surface normals, you can 
    !!return

    !! Project out momentum in direction normal to the surface
    do i=2-mbc,mx+mbc
        !! project momentum components onto tangent plane
        erx = auxr(14,i-1)
        ery = auxr(15,i-1)
        erz = auxr(16,i-1)
        amn = erx*amdq(2,i)+ery*amdq(3,i)+erz*amdq(4,i)
        amdq(2,i) = amdq(2,i) - amn*erx
        amdq(3,i) = amdq(3,i) - amn*ery
        amdq(4,i) = amdq(4,i) - amn*erz

        erx = auxl(14,i)
        ery = auxl(15,i)
        erz = auxl(16,i)
        apn = erx*apdq(2,i)+ery*apdq(3,i)+erz*apdq(4,i)
        apdq(2,i) = apdq(2,i) - apn*erx
        apdq(3,i) = apdq(3,i) - apn*ery
        apdq(4,i) = apdq(4,i) - apn*erz

    END DO

    return
END SUBROUTINE clawpack5_rpn2_fwave


!! SUBROUTINE simple_riemann(hR,huR,hvR, br, hL,huL,hvl,bL, s,fwave)
!!     IMPLICIT NONE
!! 
!!     DOUBLE PRECISION :: hr,hur,hvr, br, hl,hul,hvl, bl
!!     double precision :: s(3), fwave(4,3)
!! 
!!     double precision grav
!!     common /swe_model_parms/  grav
!! 
!!     DOUBLE PRECISION :: fluxdiff(3),beta(3), hbar, fl(3), fr(3)
!!     double precision :: psiL, psiR, sL, sR, vhat, ul, vl, ur, vr
!!     double precision :: hsq, uhat, chat, phir, phil
!! 
!!     fwave = 0
!!     s = 0
!! 
!!     uR = huR/hR
!!     uL = huL/hL
!! 
!!     vR = hvR/hR
!!     vL = hvL/hL
!! 
!!     uhat = (SQRT(grav*hL)*uL + SQRT(grav*hR)*uR)/(SQRT(grav*hR) + SQRT(grav*hL)) 
!!     vhat = (SQRT(grav*hL)*vL + SQRT(grav*hR)*vR)/(SQRT(grav*hR) + SQRT(grav*hL)) 
!!     chat = SQRT(grav*0.5d0*(hR + hL)) 
!! 
!!     hbar = 0.5 * (hr + hl)
!! 
!! 
!!     !!hsq = SQRT(hR) + SQRT(hL)
!!     !!uhat = (hul/sqrt(hl) + huR/sqrt(hr))/hsq
!!     !!vhat = (hvl/sqrt(hl) + hvl/sqrt(hl))/hsq 
!!     !!chat = SQRT(grav*hbar) 
!! 
!!     fl(1) = huL
!!     fr(1) = huR
!! 
!!     fl(2) = 0.5*grav*hL**2 + huL**2/hL
!!     fr(2) = 0.5*grav*hR**2 + huR**2/hR
!! 
!!     fl(3) = huL*vL
!!     fr(3) = huR*vR
!! 
!!     psiL = grav*hbar*bL
!!     psiR = grav*hbar*bR
!! 
!!     !! # Flux differences
!! !!    fluxdiff(1) = (hr * ur) - (hl * ul)
!! !!    fluxdiff(2) = phir - phil + grav * hbar * (br - bl)
!! !!    fluxdiff(3) = hr * ur * vr - hl * ul * vl
!! 
!!     fluxdiff(1) = fr(1) - fl(1)
!!     fluxdiff(2) = fr(2) - fl(2)  + (psir - psil)
!!     fluxdiff(3) = fr(3) - fl(3)
!! 
!!     !! # Wave speeds
!!     s(1) = MIN(ul - SQRT(grav * hl), uhat - chat)
!!     s(3) = MAX(ur + SQRT(grav * hr), uhat + chat)!!    
!!     s(2) = 0.5d0 * (s(1) + s(3))
!! 
!! 
!!     beta(1) =  (s(3)*fluxdiff(1) - fluxdiff(2)) / (s(3) - s(1))
!!     beta(3) = (-s(1)*fluxdiff(1) + fluxdiff(2)) / (s(3) - s(1))
!!     beta(2) =  - vhat*fluxdiff(1) + fluxdiff(3) 
!! 
!!     !! # Flux waves = beta*R
!!     fwave(1,1) = beta(1)
!!     fwave(2,1) = beta(1)*s(1)
!!     fwave(3,1) = beta(1)*vhat
!! 
!!     fwave(1,2) = 0
!!     fwave(2,2) = 0
!!     fwave(3,2) = beta(2)
!! 
!!     fwave(1,3) = beta(3)
!!     fwave(2,3) = beta(3)*s(3)
!!     fwave(3,3) = beta(3)*vhat
!! 
!! END subroutine simple_riemann  
