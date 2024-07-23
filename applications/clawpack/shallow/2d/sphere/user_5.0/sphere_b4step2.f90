subroutine clawpack5_b4step2(mbc,mx,my,meqn,q, & 
   xlower,ylower,dx,dy,time,dt,maux,aux)
   implicit none

   integer mbc, mx, my, meqn, maux
   double precision xlower, ylower, dx,dy,time,dt

   double precision   q(meqn,1-mbc:mx+mbc,1-mbc:my+mbc)
   double precision aux(maux,1-mbc:mx+mbc,1-mbc:my+mbc)

   double precision erx, ery, erz, qn

   integer i,j

   do i=1,mx
      do j=1,my
         !! Surface normal
         erx = aux(14,i,j)
         ery = aux(15,i,j)
         erz = aux(16,i,j)
         qn = erx*q(i,j,2) + ery*q(i,j,3) + erz*q(i,j,4)

         !! Project out surface normal (this isn't really the right)
         !! thing to do, though. 
         q(2,i,j) = q(2,i,j) - qn*erx
         q(3,i,j) = q(3,i,j) - qn*ery
         q(4,i,j) = q(4,i,j) - qn*erz
      enddo
   enddo

   return
end subroutine clawpack5_b4step2
