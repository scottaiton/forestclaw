subroutine clawpack5_rpn2(ixy,maxm,meqn,mwaves,maux, mbc,mx,& 
    ql,qr,auxl,auxr, wave,s,amdq,apdq)

    implicit none
    integer ixy,maxm,meqn,mwaves,mbc,mx, maux
!!
    double precision wave(meqn,mwaves,1-mbc:maxm+mbc)
    double precision    s(mwaves,1-mbc:maxm+mbc)
    double precision    ql(meqn,1-mbc:maxm+mbc)
    double precision    qr(meqn,1-mbc:maxm+mbc)
    double precision  apdq(meqn,1-mbc:maxm+mbc)
    double precision  amdq(meqn,1-mbc:maxm+mbc)
    double precision auxl(maux,1-mbc:maxm+mbc)
    double precision auxr(maux,1-mbc:maxm+mbc)
    
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

    double precision dx, dy


    double precision grav
    common /swe_model_parms/  grav

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

        !! # projection to the sphere  (already done in src2)
        erx = auxl(14,i)
        ery = auxl(15,i)
        erz = auxl(16,i)
!!        qn = erx*ql(i,2) + ery*ql(i,3) + erz*ql(i,4)

    !! # compute normal and tangential momentum at cell edge:
        hunl = enx*ql(2,i)   + eny*ql(3,i)   + enz*ql(4,i)
        hunr = enx*qr(2,i-1) + eny*qr(3,i-1) + enz*qr(4,i-1)
  
        hutl = etx*ql(2,i)   + ety*ql(3,i)   + etz*ql(4,i)
        hutr = etx*qr(2,i-1) + ety*qr(3,i-1) + etz*qr(4,i-1)

        !! # compute the Roe-averaged variables needed in the Roe solver.
        !! # These are stored in the common block comroe since they are
        !! # later used in routine rpt2 to do the transverse wave splitting.
        hl = ql(1,i)
        hr = qr(1,i-1)
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

        a1 =  ((u(i)+a(i))*delta(1) - delta(2))*(0.5d0/a(i))
        a3 = (-(u(i)-a(i))*delta(1) + delta(2))*(0.5d0/a(i))
        a2 = -v(i)*delta(1) + delta(3)

        !! # Compute the waves.  Rotate waves back into 
        !! # computational space.

        sk = gamma/dx
        wave(1,1,i) = a1
        wave(2,1,i) = (a1*(u(i)-a(i))*enx + a1*v(i)*etx)
        wave(3,1,i) = (a1*(u(i)-a(i))*eny + a1*v(i)*ety)
        wave(4,1,i) = (a1*(u(i)-a(i))*enz + a1*v(i)*etz)
        s(1,i) = (u(i)-a(i))*sk

        wave(1,2,i) = 0.0d0
        wave(2,2,i) = a2*etx
        wave(3,2,i) = a2*ety
        wave(4,2,i) = a2*etz
        s(2,i) = u(i) * sk

        wave(1,3,i) = a3
        wave(2,3,i) = (a3*(u(i)+a(i))*enx + a3*v(i)*etx)
        wave(3,3,i) = (a3*(u(i)+a(i))*eny + a3*v(i)*ety)
        wave(4,3,i) = (a3*(u(i)+a(i))*enz + a3*v(i)*etz)
        s(3,i) = (u(i)+a(i)) * sk

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
            amdq(m,i) = 0.d0
            apdq(m,i) = 0.d0
            do mw=1,mwaves
                if (s(mw,i) .lt. 0.d0) then
                    amdq(m,i) = amdq(m,i) + s(mw,i)*wave(m,mw,i)
                else
                    apdq(m,i) = apdq(m,i) + s(mw,i)*wave(m,mw,i)
                endif
            enddo
        end do
    end do

    !! # Set apdq/amdq at i = -1 to something reasonable to avoid
    !! # floating point errors in rpt2.  Set here so there are no
    !! # incoming waves from the left edge.
    do m = 1,meqn
        amdq(m,-1) = 0
        apdq(m,-1) = 0
    end do

    !! # project momentum components of amdq and apdq onto tangent plane:

    do i=2-mbc,mx+mbc
        erx = auxr(14,i-1)
        ery = auxr(15,i-1)
        erz = auxr(16,i-1)
        amn = erx*amdq(2,i) + ery*amdq(3,i) + erz*amdq(4,i)
        amdq(2,i) = amdq(2,i) - amn*erx
        amdq(3,i) = amdq(3,i) - amn*ery
        amdq(4,i) = amdq(4,i) - amn*erz

        erx = auxl(14,i)
        ery = auxl(15,i)
        erz = auxl(16,i)
        apn = erx*apdq(2,i) + ery*apdq(3,i) + erz*apdq(4,i)
        apdq(2,i) = apdq(2,i) - apn*erx
        apdq(3,i) = apdq(3,i) - apn*ery
        apdq(4,i) = apdq(4,i) - apn*erz
    end do

    go to 900

    !! 
    !! -----------------------------------------------------
    !! 

  110    continue
      !! Efix goes here ...

  900    continue
         return
         end
