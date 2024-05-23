subroutine clawpack46_rpn2(ixy,maxm,meqn,mwaves,mbc,mx,& 
    ql,qr,auxl,auxr, wave,s,amdq,apdq)

    implicit none
    integer ixy,maxm,meqn,mwaves,mbc,mx
!!
    double precision wave(1-mbc:maxm+mbc, meqn, mwaves)
    double precision    s(1-mbc:maxm+mbc, mwaves)
    double precision   ql(1-mbc:maxm+mbc, meqn)
    double precision   qr(1-mbc:maxm+mbc, meqn)
    double precision  apdq(1-mbc:maxm+mbc, meqn)
    double precision  amdq(1-mbc:maxm+mbc, meqn)
    double precision auxl(1-mbc:maxm+mbc, *)
    double precision auxr(1-mbc:maxm+mbc, *)
    
    !!     local arrays -- common block comroe is passed to rpt2
    !! ------------
    double precision delta(3)
    logical efix

    !! dummy arrays
    double precision u(1-mbc:maxm+mbc),v(1-mbc:maxm+mbc)
    double precision a(1-mbc:maxm+mbc),h(1-mbc:maxm+mbc)
    double precision enx, eny, enz, etx, ety, etz
    double precision gamma, hunl, hunr, hutl, hutr, hl, hr
    double precision hsqr, hsql, sk, hsq, a1, a2, a3
    double precision amn, erx, ery, erz, apn
    integer ioff, m, mw, i

    double precision dx, dy, qn


    double precision grav
    common /swe_model_parms/  grav

    !!common /comroe/ u, v, a, h

    double precision dtcom, dxcom, dycom, tcom
    integer icom,jcom
    common /comxyt/ dtcom,dxcom,dycom,tcom,icom,jcom

    data efix /.false./    !# use entropy fix for transonic rarefactions


    if(ixy.eq.1) then
        dy = dycom
        dx = dxcom
    else
        dx = dycom
        dy = dxcom
    endif

    !! The aux array has the following elements:
    !! 1  kappa = ratio of cell area to dxc*dyc
    !! 2  enx = x-component of normal vector to left edge in tangent plane
    !! 3  eny = y-component of normal vector to left edge in tangent plane
    !! 4  enz = z-component of normal vector to left edge in tangent plane
    !! 5  etx = x-component of tangent vector to left edge in tangent plane
    !! 6  ety = y-component of tangent vector to left edge in tangent plane
    !! 7  etz = z-component of tangent vector to left edge in tangent plane
    !! 8  enx = x-component of normal vector to bottom edge in tangent plane
    !! 9  eny = y-component of normal vector to bottom edge in tangent plane
    !! 10  enz = z-component of normal vector to bottom edge in tangent plane
    !! 11  etx = x-component of tangent vector to bottom edge in tangent plane
    !! 12  ety = y-component of tangent vector to bottom edge in tangent plane
    !! 13  etz = z-component of tangent vector to bottom edge in tangent plane
    !! 14  erx = x-component of unit vector in radial direction at cell ctr
    !! 15  ery = y-component of unit vector in radial direction at cell ctr
    !! 16  erz = z-component of unit vector in radial direction at cell ctr

    !! # offset to index into aux array for enx, eny, etx, ety, gamma
    !! #    depends on whether ixy=1 (left edge) or ixy=2 (bottom edge).
    ioff = 6*(ixy-1) + 1


    !! # find a1 thru a3, the coefficients of the 3 eigenvectors:

    do i = 2-mbc, mx+mbc

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

        !! # projection to the sphere  (already done in src2)
        erx = auxl(i,14)
        ery = auxl(i,15)
        erz = auxl(i,16)
        qn = erx*ql(i,2) + ery*ql(i,3) + erz*ql(i,4)
!!        ql(i,2) = ql(i,2) - qn*erx
!!        ql(i,3) = ql(i,3) - qn*ery
!!        ql(i,4) = ql(i,4) - qn*erz
!!        qr(i,2) = ql(i,2)
!!        qr(i,3) = ql(i,3)
!!        qr(i,4) = ql(i,4)

    !! # compute normal and tangential momentum at cell edge:
        hunl = enx*ql(i,2) + eny*ql(i,3) + enz*ql(i,4)
        hunr = enx*qr(i-1,2) + eny*qr(i-1,3) + enz*qr(i-1,4)

        hutl = etx*ql(i,2) + ety*ql(i,3) + etz*ql(i,4)
        hutr = etx*qr(i-1,2) + ety*qr(i-1,3) + etz*qr(i-1,4)

        !! # compute the Roe-averaged variables needed in the Roe solver.
        !! # These are stored in the common block comroe since they are
        !! # later used in routine rpt2 to do the transverse wave splitting.
        hl = ql(i,1)
        hr = qr(i-1,1)
        h(i) = (hl+hr)*0.50d0
        hsqr = sqrt(hr)
        hsql = sqrt(hl)
        hsq = hsqr + hsql
        u(i) = (hunr/hsqr + hunl/hsql) / hsq
        v(i) = (hutr/hsqr + hutl/hsql) / hsq
        a(i) = sqrt(grav*h(i))

        !! # Split the jump in q at each interface into waves
        delta(1) = hl - hr
        delta(2) = hunl - hunr
        delta(3) = hutl - hutr

        a1 =  ((u(i)+a(i))*delta(1) - delta(2))*(0.50d0/a(i))
        a3 = (-(u(i)-a(i))*delta(1) + delta(2))*(0.50d0/a(i))
        a2 = -v(i)*delta(1) + delta(3)

        !! # Compute the waves.  Rotate waves back into 
        !! # computational space.

        sk = gamma/dx
        wave(i,1,1) = a1
        wave(i,2,1) = (a1*(u(i)-a(i))*enx + a1*v(i)*etx)
        wave(i,3,1) = (a1*(u(i)-a(i))*eny + a1*v(i)*ety)
        wave(i,4,1) = (a1*(u(i)-a(i))*enz + a1*v(i)*etz)
        s(i,1) = (u(i)-a(i))*sk

        wave(i,1,2) = 0.0d0
        wave(i,2,2) = a2*etx
        wave(i,3,2) = a2*ety
        wave(i,4,2) = a2*etz
        s(i,2) = u(i) * sk

        wave(i,1,3) = a3
        wave(i,2,3) = (a3*(u(i)+a(i))*enx + a3*v(i)*etx)
        wave(i,3,3) = (a3*(u(i)+a(i))*eny + a3*v(i)*ety)
        wave(i,4,3) = (a3*(u(i)+a(i))*enz + a3*v(i)*etz)
        s(i,3) = (u(i)+a(i)) * sk

    end do



    !! # compute flux differences amdq and apdq.
    !! ---------------------------------------

    if (efix) go to 110

    !! # no entropy fix
    !! ----------------

    !! # amdq = SUM s*wave   over left-going waves
    !! # apdq = SUM s*wave   over right-going waves

    do m=1,meqn
        do i=2-mbc, mx+mbc
            amdq(i,m) = 0.d0
            apdq(i,m) = 0.d0
            do mw=1,mwaves
                if (s(i,mw) .lt. 0.d0) then
                    amdq(i,m) = amdq(i,m) + s(i,mw)*wave(i,m,mw)
                else
                    apdq(i,m) = apdq(i,m) + s(i,mw)*wave(i,m,mw)
                endif
            enddo
        end do
    end do

    !! # Set apdq/amdq at i = -1 to something reasonable to avoid
    !! # floating point errors in rpt2.  Set here so there are no
    !! # incoming waves from the left edge.
    do m = 1,meqn
        amdq(-1,m) = 0
        apdq(-1,m) = 0
    end do

    !! # project momentum components of amdq and apdq onto tangent plane:

    !!goto 900

    !! # Do projection in b4step2
    do i=2-mbc,mx+mbc
        erx = auxr(i-1,14)
        ery = auxr(i-1,15)
        erz = auxr(i-1,16)
        amn = erx*amdq(i,2) + ery*amdq(i,3) + erz*amdq(i,4)
        amdq(i,2) = amdq(i,2) - amn*erx
        amdq(i,3) = amdq(i,3) - amn*ery
        amdq(i,4) = amdq(i,4) - amn*erz

        erx = auxl(i,14)
        ery = auxl(i,15)
        erz = auxl(i,16)
        apn = erx*apdq(i,2) + ery*apdq(i,3) + erz*apdq(i,4)
        apdq(i,2) = apdq(i,2) - apn*erx
        apdq(i,3) = apdq(i,3) - apn*ery
        apdq(i,4) = apdq(i,4) - apn*erz
    end do

    go to 900

    !! 
    !! -----------------------------------------------------
    !! 

  110    continue

  900    continue
         return
         end
