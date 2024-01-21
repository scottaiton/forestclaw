subroutine clawpack46_qinit(maxmx,maxmy,meqn,mbc,mx,my, & 
   xlower,ylower,dx,dy,q,maux,aux)

   implicit none

   integer meqn, mbc, mx, my, maux,maxmx,maxmy
   double precision xlower, ylower, dx, dy
   double precision, dimension(1-mbc:mx+mbc, 1-mbc:my+mbc, meqn) :: q
   double precision, dimension(1-mbc:mx+mbc, 1-mbc:my+mbc, maux) :: aux

   double precision pi, pi2
   common /compi/ pi, pi2

   integer initchoice
   common /swe_initcond/ initchoice

   double precision ring_inner, ring_outer
   common /swe_initcond_parms2/ ring_inner, ring_outer

   double precision hin, hout
   common /swe_initcond_parms4/  hin,hout

   integer i,j
   double precision xc,yc,xlow,ylow,xp,yp,zp, w, qval
   double precision gaussian_sphere

   double precision deg2rad, phi, phi0, width

   integer blockno, fc2d_clawpack46_get_block
   integer*8 cont, fclaw_map_get_context

   cont = fclaw_map_get_context()

   blockno = fc2d_clawpack46_get_block()

   deg2rad = pi/180
   do j = 1-mbc,my+mbc
      yc = ylower + (j-0.5)*dy
      ylow = ylower + (j-1)*dy
      do i = 1-mbc,mx+mbc
         xc = xlower + (i-0.5)*dx
         xlow = xlower + (i-1)*dx

         !! Get points on the sphere
         call fclaw2d_map_c2m(cont,blockno,xc,yc,xp,yp,zp)

         qval = 0
         if (initchoice .eq. 0) then
            !! Gaussian hump
            qval = gaussian_sphere(xp,yp,zp)
         elseif (initchoice .eq. 1) then
            !! Disk or ring
            call cellave2(blockno,xlow,ylow,dx,dy,w)
            qval = w
         elseif (initchoice .eq. 2) then
            !! RJL initial conditions
            phi = acos(min(xp,1.0))
            phi0 = deg2rad*(ring_inner+ring_outer)/2.0
            width = deg2rad*5
            qval = exp(-((phi-phi0)/width)**2) 
         endif 
         q(i,j,1) = hout + (hin - hout)*qval
         q(i,j,2) = 0
         q(i,j,3) = 0
         q(i,j,4) = 0
      enddo
   enddo

   !! write(6,*) 'Stopping in qinit'
   !! stop

   return
end
