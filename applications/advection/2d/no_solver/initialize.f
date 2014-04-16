      subroutine initialize(mx,my,meqn,mbc,xlower,ylower,
     &      dx,dy,q,mpirank)
      implicit none

      integer maxmx, maxmy, meqn, mbc, mx, my, mpirank
      double precision xlower, ylower, dx, dy
      double precision q(1-mbc:mx+mbc, 1-mbc:my+mbc, meqn)

      integer i, j, mq, ichoice
      double precision xlow, ylow,wl

      common /com_init/ ichoice

      ichoice = 2

      do mq = 1,meqn
         do i = 1-mbc,mx+mbc
            xlow = xlower + (i-1)*dx
            do j = 1-mbc,my+mbc
               ylow = ylower + (j-1)*dy

               call cellave2(xlow,ylow,dx,dy,wl)

               if (mq .eq. 1) then
                  q(i,j,mq) = mpirank
               else
                  q(i,j,mq) = wl
               endif
            enddo
         enddo
      enddo

      return
      end

      double precision function fdisc(x,y)
      implicit none

      double precision x,y, r
      double precision xp, yp, rp, th, x0
      double precision y0, pi
      integer m, ichoice

      common /com_init/ ichoice

      pi = 4.d0*atan(1.d0)

      xp = x
      yp = y

      fdisc = -1
      rp = sqrt((xp)**2 + (yp)**2)

c     # Large inner circle
      if (rp .lt. 0.4d0) then
         fdisc = 1.d0
      endif

      if (ichoice .eq. 1) then
         fdisc = -fdisc
         return
      endif

c     # Smaller surrounding circles
      do m = 1,4
c        # Larger circles
         th = (m-1)*pi/2.d0
         x0 = 0.6*cos(th)
         y0 = 0.6*sin(th)
         rp = sqrt((xp - x0)**2 + (yp-y0)**2)
         if (rp .lt. 0.2) then
            fdisc = 1.d0
         endif

c        # Smaller circles
         th = pi/4.d0 + (m-1)*pi/2.d0
         x0 = 0.55*cos(th)
         y0 = 0.55*sin(th)
         rp = sqrt((xp - x0)**2 + (yp-y0)**2)
         if (rp .lt. 0.15) then
            fdisc = 1.d0
         endif
      enddo
      fdisc = -fdisc


      end
