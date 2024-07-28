SUBROUTINE fc2d_geoclaw_fort_tag4coarsening(blockno,mx,my,mbc,meqn,maux, &
       xlower,ylower,dx,dy,t,q0,q1,q2,q3, &
       aux0,aux1,aux2,aux3,level,maxlevel, init_flag, tag_patch)

    IMPLICIT NONE

    INTEGER :: mx, my, mbc, meqn, maux, tag_patch, blockno
    INTEGER :: level, maxlevel, init_flag
    DOUBLE PRECISION :: xlower(0:3), ylower(0:3), dx, dy, t

    DOUBLE PRECISION :: q0(meqn,1-mbc:mx+mbc,1-mbc:my+mbc)
    DOUBLE PRECISION :: q1(meqn,1-mbc:mx+mbc,1-mbc:my+mbc)
    DOUBLE PRECISION :: q2(meqn,1-mbc:mx+mbc,1-mbc:my+mbc)
    DOUBLE PRECISION :: q3(meqn,1-mbc:mx+mbc,1-mbc:my+mbc)

    DOUBLE PRECISION :: aux0(maux,1-mbc:mx+mbc,1-mbc:my+mbc)
    DOUBLE PRECISION :: aux1(maux,1-mbc:mx+mbc,1-mbc:my+mbc)
    DOUBLE PRECISION :: aux2(maux,1-mbc:mx+mbc,1-mbc:my+mbc)
    DOUBLE PRECISION :: aux3(maux,1-mbc:mx+mbc,1-mbc:my+mbc)

    !! Assume we will coarsen, unless one of the four patches fails
    !! the coarsening test.
    tag_patch = 1

    call fc2d_geoclaw_test_coarsen(blockno, mx,my,mbc,meqn,maux, & 
                xlower(0), ylower(0), & 
                dx,dy,t,q0,aux0,level,maxlevel, init_flag, tag_patch)
    if (tag_patch == 0) return  !!  Can't coarsen

    call fc2d_geoclaw_test_coarsen(blockno, mx,my,mbc,meqn,maux, & 
                xlower(1), ylower(1), & 
                dx,dy,t,q1,aux1,level,maxlevel, init_flag, tag_patch)
    if (tag_patch == 0) return  !!  Can't coarsen  

    call fc2d_geoclaw_test_coarsen(blockno, mx,my,mbc,meqn,maux, &
                xlower(2), ylower(2), & 
                 dx,dy,t,q2,aux2,level,maxlevel, init_flag, tag_patch)
    if (tag_patch == 0) return  !!  Can't coarsen

    call fc2d_geoclaw_test_coarsen(blockno, mx,my,mbc,meqn,maux,&
                xlower(3), ylower(3), & 
                 dx,dy,t,q3,aux3,level,maxlevel, init_flag, tag_patch)
    if (tag_patch == 0) return  !!  Can't coarsen

    return
end subroutine fc2d_geoclaw_fort_tag4coarsening


SUBROUTINE fc2d_geoclaw_test_coarsen(blockno, mx,my,mbc,meqn,maux,xlower,ylower, &
                       dx,dy,t,q,aux,level,maxlevel, init_flag, tag_patch)

    IMPLICIT NONE

    INTEGER :: mx, my, mbc, meqn, maux, tag_patch, blockno
    INTEGER :: level, maxlevel,init_flag
    DOUBLE PRECISION :: xlower, ylower, dx, dy, t

    DOUBLE PRECISION :: q(meqn,1-mbc:mx+mbc,1-mbc:my+mbc)
    DOUBLE PRECISION :: aux(maux,1-mbc:mx+mbc,1-mbc:my+mbc)

    INTEGER :: i,j,m
    DOUBLE PRECISION :: xc,yc, xupper, yupper, qvec(meqn), auxvec(maux)

    LOGICAL :: is_coarsening

    INTEGER :: fc2d_geoclaw_flag2refine, flag_patch


    tag_patch = 1    !! Allow coarsening if nothing below prevents it

    is_coarsening = .true.
    do j = 1,my
        yc   = ylower + (j - 0.5d0) * dy
        do i = 1,mx
            xc   = xlower + (i - 0.5d0) * dx

            do m = 1,meqn
                qvec(m) = q(m,i,j)
            end do
            do m = 1,maux
                auxvec(m) = aux(m,i,j)
            enddo

            flag_patch = fc2d_geoclaw_flag2refine( & 
                    blockno, meqn, maux, qvec, auxvec, dx,dy,xc,yc,t,level, & 
                    maxlevel, init_flag, is_coarsening)

!!          # flag_patch : 
!!          # -1 : Not conclusive (possibly ghost cell) (do not tag for coarsening)
!!          # 0  : Does not exceed threshold (tag for coarsening)      
!!          # 1  : Exceeds coarsening threshold (do not tag for coarsening)

            if (flag_patch .gt. 0) then
                tag_patch = 0
                return
            endif

        end do
    end do

END SUBROUTINE fc2d_geoclaw_test_coarsen

