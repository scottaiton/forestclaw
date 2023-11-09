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

!! (not used here)
!! 17  xc at cell center
!! 18  yc at cell center
!! 19  bathymetry - averaged over all possible finer cells


subroutine sphere_setaux(mx,my,mbc,xlower,ylower, & 
   dx,dy,area,xnormals,ynormals, & 
   xtangents,ytangents,surfnormals,aux,maux)
      
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
   double precision surfnormals(-mbc:mx+mbc+1,-mbc:my+mbc+1,3)

   integer i,j,m
   double precision dxdy

   dxdy = dx*dy
   do i = 1-mbc,mx+mbc
      do j = 1-mbc,my+mbc

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
      enddo
   enddo

end
