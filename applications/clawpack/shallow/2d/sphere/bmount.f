      double precision function bmount(xc,yc)
      implicit none

      double precision xc,yc

      double precision hin, hout
      common /swe_initcond_parms4/  hin,hout

      double precision Rsphere, omega
      common /comsphere/ Rsphere, Omega

      double precision Px, Py, Pz
      common /comic/ Px,Py,Pz

      double precision pi,pi2
      common /compi/ pi, pi2

      double precision theta,thetam
      double precision xp, yp, zp


      bmount = -hout

      return

      bmount = -4.d4

c     # compute latitude:
      call mapc2m(xc,yc,xp,yp,zp)
      theta = asin((xp*Px + yp*Py + zp*Pz) / Rsphere)

      thetam = -pi/6.d0 !position of ridge
      bmount = bmount + 3.d4*exp(-1000.d0*(theta-thetam)**2)
      return
      end
