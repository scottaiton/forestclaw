subroutine sphere_setaux(mx,my,mbc,xlower,ylower, & 
   dx,dy,area,xnormals,ynormals, & 
   xtangents,ytangents,surfnormals,curvature, & 
   edgelengths,aux,maux)
      
   implicit none

   integer mbc, mx,my, maux
   double precision xlower, ylower, dx,dy
   double precision aux(1-mbc:mx+mbc,1-mbc:my+mbc,maux)

   !! # Edge based quantities
   double precision     xnormals(-mbc:mx+mbc+2,-mbc:my+mbc+2,3)
   double precision     ynormals(-mbc:mx+mbc+2,-mbc:my+mbc+2,3)
   double precision    xtangents(-mbc:mx+mbc+2,-mbc:my+mbc+2,3)
   double precision    ytangents(-mbc:mx+mbc+2,-mbc:my+mbc+2,3)
   double precision  edgelengths(-mbc:mx+mbc+2,-mbc:my+mbc+2,2)

   !! Cell centered quantities
   double precision        area(-mbc:mx+mbc+1,-mbc:my+mbc+1)
   double precision   curvature(-mbc:mx+mbc+2,-mbc:my+mbc+1)
   double precision surfnormals(-mbc:mx+mbc+1,-mbc:my+mbc+1,3)

    integer example
    common /swe_example/ example

   integer i,j,m
   double precision dxdy, xc, yc
   double precision bmount

   integer blockno, fc2d_clawpack46_get_block
   integer*8 cont, fclaw_map_get_context

   cont = fclaw_map_get_context()

   blockno = fc2d_clawpack46_get_block()

   dxdy = dx*dy
   do i = 1-mbc,mx+mbc
      do j = 1-mbc,my+mbc
         xc = xlower + (i-0.5)*dx
         yc = ylower + (j-0.5)*dy

         !! (1) Capacity
         aux(i,j,1) = area(i,j)/dxdy
         do m = 1,3

            !! (2-7) normals and tangents at left x-face
            aux(i,j,1+m) = xnormals(i,j,m)
            aux(i,j,4+m) = xtangents(i,j,m)

            !! (8-13) normals and tangents at bottom y-face
            aux(i,j,7+m) = ynormals(i,j,m)
            aux(i,j,10+m) = ytangents(i,j,m)

            !! (14-16) surface normal at cell centers
            aux(i,j,13+m) = surfnormals(i,j,m)

         enddo
         aux(i,j,17) = curvature(i,j)

         !! Set bathymetry
         !! Ridge bathymetry
         aux(i,j,18) = bmount(blockno, xc,yc)  
      enddo
   enddo

end
