subroutine clawpack46_qinit(maxmx,maxmy,meqn,mbc,mx,my, & 
   xlower,ylower,dx,dy,q,maux,aux)

   implicit none

   integer meqn, mbc, mx, my, maux,maxmx,maxmy
   double precision xlower, ylower, dx, dy
   double precision q(1-mbc:mx+mbc, 1-mbc:my+mbc, meqn)
   double precision aux(1-mbc:mx+mbc, 1-mbc:my+mbc, maux)

   integer i,j
   double precision xc,yc,xp,yp,zp
   double precision gaussian_sphere

   integer init_cond
   common /swe_initcond/ init_cond

   integer blockno, fc2d_clawpack46_get_block
   integer*8 cont, fclaw_map_get_context

   cont = fclaw_map_get_context()

   blockno = fc2d_clawpack46_get_block()

   do j = 1-mbc,my+mbc
      do i = 1-mbc,mx+mbc
         xc = xlower + (i-0.5)*dx
         yc = ylower + (j-0.5)*dy
         call fclaw2d_map_c2m(cont,blockno,xc,yc,xp,yp,zp)
         q(i,j,1) = gaussian_sphere(xp,yp,zp) + 0.2d0
         q(i,j,2) = 0
         q(i,j,3) = 0
         q(i,j,4) = 0
      enddo
   enddo

   return
end
