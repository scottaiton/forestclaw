c
cc     =====================================================
      subroutine burgers_flux2(ixy,maxm,meqn,maux,mbc,mx,
     &      q1d,dtdx1d,aux1,aux2,aux3, faddm,faddp,gaddm,gaddp,
     &      cfl1d,wave,s,amdq,apdq,cqxx,bmasdq,bpasdq,
     &      rpn2,rpt2,mwaves,mcapa,method,mthlim)
c     =====================================================

c
c     =====================================================
c      subroutine burgers_flux2(ixy,maxm,meqn,maux,mbc,mx,
c     &                 q1d,dtdx1d,aux1,aux2,aux3,
c     &                 faddm,faddp,gaddm,gaddp,
c     &                 method,mthlim,qadd,fadd,gadd,cfl1d,wave,s,
c     &                 amdq,apdq,cqxx,bmasdq,bpasdq,rpn2,rpt2)
cc     =====================================================
c
c     # Compute the modification to fluxes f and g that are generated by
c     # all interfaces along a 1D slice of the 2D grid. 
c     #    ixy = 1  if it is a slice in x
c     #          2  if it is a slice in y
c     # This value is passed into the Riemann solvers. The flux modifications
c     # go into the arrays fadd and gadd.  The notation is written assuming
c     # we are solving along a 1D slice in the x-direction.
c
c     # fadd(i,.) modifies F to the left of cell i
c     # gadd(i,.,1) modifies G below cell i
c     # gadd(i,.,2) modifies G above cell i
c
c     # The method used is specified by method(2:3):
c
c         method(2) = 1 if only first order increment waves are to be used.
c                   = 2 if second order correction terms are to be added, with
c                       a flux limiter as specified by mthlim.  
c
c         method(3) = 0 if no transverse propagation is to be applied.
c                       Increment and perhaps correction waves are propagated
c                       normal to the interface.
c                   = 1 if transverse propagation of increment waves 
c                       (but not correction waves, if any) is to be applied.
c                   = 2 if transverse propagation of correction waves is also
c                       to be included.  
c
c     Note that if method(6)=1 then the capa array comes into the second 
c     order correction terms, and is already included in dtdx1d:
c     If ixy = 1 then
c        dtdx1d(i) = dt/dx                 if method(6) = 0
c                  = dt/(dx*capa(i,jcom))  if method(6) = 1
c     If ixy = 2 then
c        dtdx1d(j) = dt/dy                 if method(6) = 0
c                  = dt/(dy*capa(icom,j))  if method(6) = 1
c
c     Notation:
c        The jump in q (q1d(i,:)-q1d(i-1,:))  is split by rpn2 into
c            amdq =  the left-going flux difference  A^- Delta q  
c            apdq = the right-going flux difference  A^+ Delta q  
c        Each of these is split by rpt2 into 
c            bmasdq = the down-going transverse flux difference B^- A^* Delta q
c            bpasdq =   the up-going transverse flux difference B^+ A^* Delta q
c        where A^* represents either A^- or A^+.
c
c
      implicit double precision (a-h,o-z)
      external rpn2,rpt2
      dimension    q1d(1-mbc:maxm+mbc, meqn)
      dimension   amdq(1-mbc:maxm+mbc, meqn)
      dimension   apdq(1-mbc:maxm+mbc, meqn)
      dimension bmasdq(1-mbc:maxm+mbc, meqn)
      dimension bpasdq(1-mbc:maxm+mbc, meqn)
      dimension   cqxx(1-mbc:maxm+mbc, meqn)
      double precision   faddm(1-mbc:maxm+mbc, meqn)
      double precision   faddp(1-mbc:maxm+mbc, meqn)
      double precision   gaddm(1-mbc:maxm+mbc, meqn, 2)
      double precision   gaddp(1-mbc:maxm+mbc, meqn, 2)

c      dimension   qadd(1-mbc:maxm+mbc, meqn)
c      dimension   fadd(1-mbc:maxm+mbc, meqn)
c      dimension   gadd(1-mbc:maxm+mbc, meqn, 2)
c
      dimension dtdx1d(1-mbc:maxm+mbc)
      dimension aux1(1-mbc:maxm+mbc, *)
      dimension aux2(1-mbc:maxm+mbc, *)
      dimension aux3(1-mbc:maxm+mbc, *)
c
      dimension     s(1-mbc:maxm+mbc, mwaves)
      dimension  wave(1-mbc:maxm+mbc, meqn, mwaves)
      dimension   phi(1-mbc:maxm+mbc, meqn, mwaves)

c      dimension    st(1-mbc:maxm+mbc, mwaves)
      dimension   cqxx2(1-mbc:maxm+mbc, meqn)
c
      dimension method(7),mthlim(mwaves)

      double precision gupdate, fupdate

      logical limit
      common /comxyt/ dtcom,dxcom,dycom,tcom,icom,jcom


c
      limit = .false.
      do 5 mw=1,mwaves
         if (mthlim(mw) .gt. 0) limit = .true.
   5  continue
c
c     # initialize flux increments:
c     -----------------------------
c
         do 20 m=1,meqn
            do 10 i = 1-mbc, mx+mbc
               faddm(i,m) = 0.d0
               faddp(i,m) = 0.d0
               gaddm(i,m,1) = 0.d0
               gaddm(i,m,2) = 0.d0
               gaddp(i,m,1) = 0.d0
               gaddp(i,m,2) = 0.d0
   10       continue
   20    continue
c
c
c     # solve Riemann problem at each interface and compute Godunov updates
c     ---------------------------------------------------------------------
c

c      call rpn2(ixy,maxm,meqn,mwaves,mbc,mx,q1d,aux2,aux1,aux3,
c     &        wave,s,amdq,apdq)      

      call rpn2(ixy,maxm,meqn,mwaves,mbc,mx,q1d,q1d,aux2,aux2,
     &        wave,s,amdq,apdq)      
c
c     # Set qadd for the donor-cell upwind method (Godunov)
      do 41 m=1,meqn
         do 40 i=3-mbc,mx+mbc-2
            faddp(i,m) = faddp(i,m) - apdq(i,m)
            faddm(i,m) = faddm(i,m) + amdq(i,m)
   40    continue
   41 continue
c
c     # compute maximum wave speed for checking Courant number:
      cfl1d = 0.d0
      do 51 mw=1,mwaves
         do 50 i=1,mx+1
c          # if s>0 use dtdx1d(i) to compute CFL,
c          # if s<0 use dtdx1d(i-1) to compute CFL:
            cfl1d = dmax1(cfl1d, dtdx1d(i)*s(i,mw), 
     &                          -dtdx1d(i-1)*s(i,mw))
   50    continue
   51 continue
c
      if (method(2).eq.1) go to 130
c
c     # modify F fluxes for second and third order correction terms:
c     -----------------------------------------------------------
c
      do 101 i = 1-mbc, mx+mbc
         do 101 m = 1, meqn
            do 101 mw = 1, mwaves
               phi(i,m,mw) = 1d0
  101          continue
c
c
c     # compute third-order correction terms
c     --------------------------------------
      if(method(2) .ge. 3) then

      do 530 i=4-mbc,mx+mbc-3
        dtdxave = 0.5d0 * (dtdx1d(i-1) + dtdx1d(i))
         do 520 m=1,meqn
            do 510 mw=1,mwaves
               a=sign(1d0,s(i,mw))*wave(i,m,mw)
               if(a .eq. 0d0) go to 510
               if(s(i,mw) .gt. 0d0) then
                  b=wave(i-1,m,mw)
                  r=b/(a+1.d-15)
                  phi(i,m,mw) = 1d0 - ((1d0-(dtdxave*s(i,mw))**2)
     &               - (1d0-(dtdxave*s(i-1,mw))**2)* r)
     &                /(3d0*(1d0-dtdxave*dabs(s(i,mw))))
               else
                  b=wave(i+1,m,mw)
                  r=b/(a+1.d-15)
                  phi(i,m,mw) = 1d0-(((1d0-(dtdxave*s(i+1,mw))**2)*r)
     &               + (1d0-(dtdxave*s(i,mw))**2))
     &               /(3d0*(1d0-dtdxave*dabs(s(i,mw))))
               endif
  510          continue
  520       continue
  530    continue
c
      endif
c
c     # apply limiter to waves:
      if (limit) call limiter(maxm,meqn,mwaves,mbc,mx,wave,s,
     &                        dtdx1d,phi,mthlim)
c
      do 120 i = 4-mbc, mx+mbc-3
c
c     # For correction terms below, need average of dtdx in cell
c     # i-1 and i.  Compute these and overwrite dtdx1d:
c
c        # modified in Version 4.3 to use average only in cqxx, not transverse
         dtdxave = 0.5d0 * (dtdx1d(i-1) + dtdx1d(i))
c
         do 120 m=1,meqn
            cqxx(i,m) = 0.d0
            cqxx2(i,m) = 0.d0
            do 119 mw=1,mwaves
               dtdxave = 0.5d0 * (dtdx1d(i-1) + dtdx1d(i))

c              # correction terms: 
               cqxx(i,m) = cqxx(i,m) + sign(1d0,s(i,mw))
     &            * (1.d0 - dabs(s(i,mw))*dtdxave) 
     &            * phi(i,m,mw) * wave(i,m,mw)

               cqxx2(i,m) = cqxx2(i,m) + sign(1d0,s(i,mw))
     &            * (1.d0 - dabs(s(i,mw))*dtdxave) 
     &            * wave(i,m,mw)
c
  119       continue
            if(method(2).eq.3) then
              faddm(i,m) = faddm(i,m) + 0.5d0 * cqxx(i,m)
              faddp(i,m) = faddp(i,m) + 0.5d0 * cqxx(i,m)
            endif
            if(method(2).eq.2) then
              faddm(i,m) = faddm(i,m) + 0.5d0 * cqxx2(i,m)
              faddp(i,m) = faddp(i,m) + 0.5d0 * cqxx2(i,m)
            endif
  120    continue
  121 continue
c
c
  130  continue
c
      if (method(3).le.0) go to 999   !# no transverse propagation
c
      if (method(2).gt.1 .and. method(3).le.3) then
c        # incorporate cqxx into amdq and apdq so that it is split also.
         do 151 m=1,meqn
            do 150 i = 4-mbc, mx+mbc-3
               if(method(3).eq.3) then  
                  amdq(i,m) = amdq(i,m) + cqxx(i,m)
                  apdq(i,m) = apdq(i,m) - cqxx(i,m)
               endif
               if(method(3).eq.2) then  
                  amdq(i,m) = amdq(i,m) + cqxx2(i,m)
                  apdq(i,m) = apdq(i,m) - cqxx2(i,m)
               endif
  150       continue
  151    continue
      endif
c
c
c      # modify G fluxes for transverse propagation
c      --------------------------------------------
c
c
c     # split the left-going flux difference into down-going and up-going:
      call rpt2(ixy,maxm,meqn,mwaves,mbc,mx,q1d,q1d,aux1,
     &        aux2,aux3,
     &        1,amdq,bmasdq,bpasdq)
c
c     # modify flux below and above by B^- A^- Delta q and  B^+ A^- Delta q:
      do 161 m=1,meqn
         do 160 i= 4-mbc, mx+mbc-3
               gupdate = 0.5d0*dtdx1d(i-1) * bmasdq(i,m)
               gaddm(i-1,m,1) = gaddm(i-1,m,1) - gupdate
               gaddp(i-1,m,1) = gaddp(i-1,m,1) - gupdate
c
               gupdate = 0.5d0*dtdx1d(i-1) * bpasdq(i,m)
               gaddm(i-1,m,2) = gaddm(i-1,m,2) - gupdate
               gaddp(i-1,m,2) = gaddp(i-1,m,2) - gupdate
  160    continue
  161 continue
c
c     # split the right-going flux difference into down-going and up-going:
      call rpt2(ixy,maxm,meqn,mwaves,mbc,mx,q1d,q1d,aux1,
     &        aux2,aux3,
     &        2,apdq,bmasdq,bpasdq)
c
c     # modify flux below and above by B^- A^+ Delta q and  B^+ A^+ Delta q:
      do 181 m=1,meqn
         do 180 i = 4-mbc, mx+mbc-3
               gupdate = 0.5d0*dtdx1d(i) * bmasdq(i,m)
               gaddm(i,m,1) = gaddm(i,m,1) - gupdate
               gaddp(i,m,1) = gaddp(i,m,1) - gupdate
c
               gupdate = 0.5d0*dtdx1d(i) * bpasdq(i,m)
               gaddm(i,m,2) = gaddm(i,m,2) - gupdate
               gaddp(i,m,2) = gaddp(i,m,2) - gupdate
  180    continue
  181 continue
c
  999 continue
      return
      end
