!! # check to see if value exceeds threshold

integer function user_exceeds_threshold(blockno,meqn,& 
                                     qval,qmin,qmax,quad, & 
                                     dx,dy,xc,yc,ivar_threshold, threshold, &
                                     init_flag, is_ghost)
    implicit none
    
    integer :: meqn,ivar_threshold
    double precision :: qval(meqn),qmin(meqn),qmax(meqn),threshold
    double precision :: quad(-1:1,-1:1,meqn)
    double precision :: dx,dy, xc, yc
    integer :: blockno, init_flag, refine
    logical(kind=4) :: is_ghost

    double precision ring_inner, ring_outer
    common /swe_initcond_parms2/ ring_inner, ring_outer

   double precision hin, hout
   common /swe_initcond_parms4/  hin,hout

    integer :: mq
    double precision :: fdisc, fval, pi
    double precision :: ri_save, ro_save

    refine = 0

    pi = 4.d0*atan(1.d0)

    if (init_flag  .ne. 0) then
        ri_save = ring_inner
        ro_save = ring_outer
        ring_inner = 0
        ring_outer = 1.05*ring_outer  !! Set to somthing larger than ring_outer
        fval = fdisc(blockno,xc,yc)
        ring_inner = ri_save
        ring_outer = ro_save
        if (fval .le. 0) then
            refine = 1
        endif
    else
        mq = ivar_threshold

        if (.not. is_ghost) then
            if (abs(qval(1)-hout) .gt. threshold) then
                refine = 1
            endif
        endif
    endif


    user_exceeds_threshold = refine

end function user_exceeds_threshold
