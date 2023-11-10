subroutine clawpack46_qinit(maxmx,maxmy,meqn,mbc,mx,my, & 
   xlower,ylower,dx,dy,q,maux,aux)

   implicit none

   integer meqn, mbc, mx, my, maux,maxmx,maxmy
   double precision xlower, ylower, dx, dy
   double precision q(1-mbc:mx+mbc, 1-mbc:my+mbc, meqn)
   double precision aux(1-mbc:mx+mbc, 1-mbc:my+mbc, maux)

   integer initchoice
   common /swe_initcond/ initchoice

   double precision hin, hout
   common /swe_initcond_parms4/  hin,hout

   integer i,j
   double precision xc,yc,xlow,ylow,xp,yp,zp, w, qval
   double precision gaussian_sphere

   integer blockno, fc2d_clawpack46_get_block
   integer*8 cont, fclaw_map_get_context

   cont = fclaw_map_get_context()

   blockno = fc2d_clawpack46_get_block()

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
         elseif (initchoice .eq. 1 .or. initchoice .eq. 2) then
            !! Disk or ring
            call cellave2(blockno,xlow,ylow,dx,dy,w)
            if (qval > 0) then
                write(6,*) "qval > 0"
                stop
            endif
            qval = w
         endif 
         q(i,j,1) = hout + (hin - hout)*qval
         q(i,j,2) = 0
         q(i,j,3) = 0
         q(i,j,4) = 0
      enddo
   enddo

   return
end
