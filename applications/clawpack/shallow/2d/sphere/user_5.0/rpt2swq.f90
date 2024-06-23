subroutine clawpack5_rpt2(ixy,imp,maxm,meqn,mwaves,maux, &
    mbc,mx, ql,qr,aux1,aux2,aux3,asdq,bmasdq,bpasdq)

    implicit none

    !! # Riemann solver in the transverse direction for the shallow water
    !! # equations  on a quadrilateral grid.

    !! # Split asdq (= A^* \Delta q, where * = + or -)
    !! # into down-going flux difference bmasdq (= B^- A^* \Delta q)
    !! #    and up-going flux difference bpasdq (= B^+ A^* \Delta q)

    !! # Uses Roe averages and other quantities which were
    !! # computed in rpn2sh and stored in the common block comroe.

    integer ixy,maxm,meqn,mwaves,mbc,mx, imp, maux

    double precision, dimension(meqn,1-mbc:maxm+mbc) :: ql,qr
    double precision, dimension(meqn,1-mbc:maxm+mbc) :: asdq, bmasdq, bpasdq
    double precision   aux1(maux,1-mbc:maxm+mbc)
    double precision   aux2(maux,1-mbc:maxm+mbc)
    double precision   aux3(maux,1-mbc:maxm+mbc)

    !! Dummy arguments
    double precision wave(meqn, mwaves,1-mbc:maxm+mbc)
    double precision    s(mwaves, 1-mbc:maxm+mbc)
    double precision, dimension(1-mbc:maxm+mbc) :: u,v,a,h
    double precision, dimension(1-mbc:maxm+mbc) :: enx, eny, enz
    double precision, dimension(1-mbc:maxm+mbc) :: etx, ety, etz
    double precision, dimension(1-mbc:maxm+mbc) :: gamma

    double precision grav
    common /swe_model_parms/  grav

    double precision dtcom, dxcom, dycom, tcom
    integer icom,jcom
    common /comxyt/ dtcom,dxcom,dycom,tcom,icom,jcom

    double precision delta(4)
    double precision a1, a2, a3, dx, dy, bn
    double precision erx, ery, erz 
    integer i, j, m, i1, mw, ioff, ix1, ixm1


    if (ixy .eq. 1) then
        dx = dxcom
    else
        dx = dycom
    endif


    if (ixy .eq. 1) then
        ioff = 7
    else
        ioff = 1
    endif


    !! # imp is used to flag whether wave is going to left or right,
    !! # since states and grid are different on each side
    if (imp .eq. 1) then
        !! # asdq = amdq, moving to left
        ix1 = 2-mbc
        ixm1 = mx+mbc
    else
        !! # asdq = apdq, moving to right
        ix1 = 1-mbc
        ixm1 = mx+mbc
    endif

    !!  --------------
    !!  # up-going:
    !!  --------------

    !! # determine rotation matrix for interface above cell, using
    !! # aux3

    !! [ alf  beta ]
    !! [-beta  alf ]
    
    do i = ix1,ixm1

        if (imp.eq.1) then
            i1 = i-1
        else
            i1 = i
        endif

        enx(i) =   aux3(ioff+1,i1)
        eny(i) =   aux3(ioff+2,i1)
        enz(i) =   aux3(ioff+3,i1)

        etx(i) =   aux3(ioff+4,i1)
        ety(i) =   aux3(ioff+5,i1)
        etz(i) =   aux3(ioff+6,i1)

        gamma(i) = dsqrt(etx(i)**2 + ety(i)**2 + etz(i)**2)
        !!gamma(i) = aux3(i1,17+(3-ixy)-1)
        etx(i) =   etx(i) / gamma(i)
        ety(i) =   ety(i) / gamma(i)
        etz(i) =   etz(i) / gamma(i)

        h(i) = ql(1,i1)
        u(i) = (enx(i)*ql(2,i1)+eny(i)*ql(3,i1)+enz(i)*ql(4,i1)) & 
              / h(i)
        v(i) = (etx(i)*ql(2,i1)+ety(i)*ql(3,i1)+etz(i)*ql(4,i1)) &
              / h(i)
        a(i) = dsqrt(grav*h(i))
    end do



    !! # now split asdq into waves:

    !! # find a1 thru a3, the coefficients of the 3 eigenvectors:
    do  i = ix1,ixm1
        delta(1) = asdq(i,1)
        delta(2) = enx(i)*asdq(2,i)+eny(i)*asdq(3,i)+enz(i)*asdq(4,i)
        delta(3) = etx(i)*asdq(2,i)+ety(i)*asdq(3,i)+etz(i)*asdq(4,i)
        a1 = ((u(i)+a(i))*delta(1) - delta(2))*(0.50d0/a(i))
        a2 = -v(i)*delta(1) + delta(3)
        a3 = (-(u(i)-a(i))*delta(1) + delta(2))*(0.50d0/a(i))

        !! # Compute the waves.

        wave(1,1,i) = a1
        wave(2,1,i) = enx(i)*a1*(u(i)-a(i)) + etx(i)*a1*v(i)
        wave(3,1,i) = eny(i)*a1*(u(i)-a(i)) + ety(i)*a1*v(i)
        wave(4,1,i) = enz(i)*a1*(u(i)-a(i)) + etz(i)*a1*v(i)
        s(1,i) = (u(i)-a(i))*gamma(i)/dx

        wave(1,2,i) = 0.0d0
        wave(2,2,i) = etx(i)*a2
        wave(3,2,i) = ety(i)*a2
        wave(4,2,i) = etz(i)*a2
        s(2,i) = u(i) * gamma(i)/dx

        wave(1,3,i) = a3
        wave(2,3,i) = enx(i)*a3*(u(i)+a(i)) + etx(i)*a3*v(i)
        wave(3,3,i) = eny(i)*a3*(u(i)+a(i)) + ety(i)*a3*v(i)
        wave(4,3,i) = enz(i)*a3*(u(i)+a(i)) + etz(i)*a3*v(i)
        s(3,i) = (u(i)+a(i)) * gamma(i)/dx
    end do

    !! --------------------------------
    !! # compute flux difference bpasdq
    !! --------------------------------

    do m = 1,meqn
        do i = ix1,ixm1
            bpasdq(m,i) = 0.d0
            do mw = 1,mwaves
                bpasdq(m,i) = bpasdq(m,i) & 
                    + dmax1(s(mw,i),0.d0)*wave(m,mw,i)
            end do
        end do
    end do


    !! # project momentum component of bpasdq to tangent plane:
    do i = ix1, ixm1
        if (imp.eq.1) then
            i1 = i-1
        else
            i1 = i
        endif
        !! erx = aux3(14,i1)
        !! ery = aux3(15,i1)
        !! erz = aux3(16,i1)
        !! bn = erx*bpasdq(2,i)+ery*bpasdq(3,i)+erz*bpasdq(4,i)
        !! bpasdq(i,2) = bpasdq(i,2) - bn*erx
        !! bpasdq(i,3) = bpasdq(i,3) - bn*ery
        !! bpasdq(i,4) = bpasdq(i,4) - bn*erz
    end do

    !! --------------
    !! # down-going:
    !! --------------


    !! # determine rotation matrix for interface below cell, using
    !! aux2
    !! [ alf  beta ]
    !! [-beta  alf ]

    do i=ix1,ixm1

        if (imp.eq.1) then
            i1 = i-1
        else
            i1 = i
        endif

        enx(i) =   aux2(ioff+1,i1)
        eny(i) =   aux2(ioff+2,i1)
        enz(i) =   aux2(ioff+3,i1)
        etx(i) =   aux2(ioff+4,i1)
        ety(i) =   aux2(ioff+5,i1)
        etz(i) =   aux2(ioff+6,i1)
        gamma(i) = dsqrt(etx(i)**2 + ety(i)**2 + etz(i)**2)
        !!gamma(i) = aux3(i1,17+(3-ixy)-1)
        etx(i) =   etx(i) / gamma(i)
        ety(i) =   ety(i) / gamma(i)
        etz(i) =   etz(i) / gamma(i)
        u(i) = (enx(i)*ql(2,i1)+eny(i)*ql(3,i1)+enz(i)*ql(4,i1)) & 
                 / h(i)
        v(i) = (etx(i)*ql(2,i1)+ety(i)*ql(3,i1)+etz(i)*ql(4,i1)) &
                 / h(i)
    end do



    !!# now split asdq into waves:

    !!# find a1 thru a3, the coefficients of the 3 eigenvectors:
    do i = ix1,ixm1
        delta(1) = asdq(i,1)
        delta(2) = enx(i)*asdq(2,i)+eny(i)*asdq(3,i)+enz(i)*asdq(4,i)
        delta(3) = etx(i)*asdq(2,i)+ety(i)*asdq(3,i)+etz(i)*asdq(4,i)

        a1 = ((u(i)+a(i))*delta(1) - delta(2))*(0.50d0/a(i))
        a2 = -v(i)*delta(1) + delta(3)
        a3 = (-(u(i)-a(i))*delta(1) + delta(2))*(0.50d0/a(i))

        !! # Compute the waves.

        wave(1,1,i) = a1
        wave(2,1,i) = enx(i)*a1*(u(i)-a(i)) + etx(i)*a1*v(i)
        wave(3,1,i) = eny(i)*a1*(u(i)-a(i)) + ety(i)*a1*v(i)
        wave(4,1,i) = enz(i)*a1*(u(i)-a(i)) + etz(i)*a1*v(i)
        s(1,i) = (u(i)-a(i)) * gamma(i)/dx

        wave(1,2,i) = 0.0
        wave(2,2,i) = etx(i)*a2
        wave(3,2,i) = ety(i)*a2
        wave(4,2,i) = etz(i)*a2
        s(2,i) = u(i) * gamma(i)/dx

        wave(1,3,i) = a3
        wave(2,3,i) = enx(i)*a3*(u(i)+a(i)) + etx(i)*a3*v(i)
        wave(3,3,i) = eny(i)*a3*(u(i)+a(i)) + ety(i)*a3*v(i)
        wave(4,3,i) = enz(i)*a3*(u(i)+a(i)) + etz(i)*a3*v(i)
        s(3,i) = (u(i)+a(i)) * gamma(i)/dx
    end do


    !! # compute flux difference bmasdq
    !! --------------------------------

    do  m=1,meqn
        do  i = ix1,ixm1
            bmasdq(m,i) = 0.d0
            do  mw = 1,mwaves
                bmasdq(m,i) = bmasdq(m,i) &
                    + dmin1(s(mw,i), 0.d0)*wave(m,mw,i)
            end do
        end do
    end do

    !! # project momentum component of bmasdq to tangent plane:
    do i = ix1, ixm1
        if (imp.eq.1) then
            i1 = i-1
        else
            i1 = i
        endif
        !!erx = aux1(i1,14)
        !!ery = aux1(i1,15)
        !!erz = aux1(i1,16)
        !!bn = erx*bmasdq(i,2)+ery*bmasdq(i,3)+erz*bmasdq(i,4)
        !!bmasdq(i,2) = bmasdq(i,2) - bn*erx
        !!bmasdq(i,3) = bmasdq(i,3) - bn*ery
        !!bmasdq(i,4) = bmasdq(i,4) - bn*erz
    end do

    return
end subroutine clawpack5_rpt2
