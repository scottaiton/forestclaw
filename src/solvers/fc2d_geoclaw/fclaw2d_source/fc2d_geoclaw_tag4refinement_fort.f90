SUBROUTINE fc2d_geoclaw_fort_tag4refinement(mx,my,mbc,meqn,maux,xlower,ylower, &
    dx,dy,t,blockno,q,aux,level,maxlevel,init_flag,tag_patch)

    IMPLICIT NONE

    INTEGER :: mx,my, mbc, meqn, maux, tag_patch, init_flag
    INTEGER :: blockno, level, maxlevel
    DOUBLE PRECISION :: xlower, ylower, dx, dy, t
    DOUBLE PRECISION, intent(in) :: q(meqn,1-mbc:mx+mbc,1-mbc:my+mbc)
    DOUBLE PRECISION, INTENT(in) :: aux(maux,1-mbc:mx+mbc,1-mbc:my+mbc)

    !! Local variables
    INTEGER :: i,j, m
    DOUBLE PRECISION :: xc,yc,qvec(meqn), auxvec(maux)
    logical :: is_coarsening

    INTEGER :: fc2d_geoclaw_flag2refine, flag_patch

    tag_patch = 0 

    is_coarsening = .false.   !! Don't loop over ghost cells.
    DO j = 1,my
        DO i = 1,mx
            xc = xlower + (i-0.5)*dx
            yc = ylower + (j-0.5)*dy
            do m = 1,meqn
                qvec(m) = q(m,i,j)
            end do
            do m = 1,maux
                auxvec(m) = aux(m,i,j)
            enddo
            flag_patch = fc2d_geoclaw_flag2refine( & 
                    blockno, meqn,maux, qvec, auxvec, dx,dy,xc,yc,t,level, & 
                    maxlevel, init_flag, is_coarsening)

!!          # -1 : Not conclusive (possibly ghost cell); don't tag for refinement
!!          # 0  : Does not pass threshold (don't tag for refinement)      
!!          # 1  : Passes threshold (tag for refinement)
            if (flag_patch .gt. 0) then
                tag_patch = 1
                return
            endif
        end do 
    enddo 

END SUBROUTINE fc2d_geoclaw_fort_tag4refinement
