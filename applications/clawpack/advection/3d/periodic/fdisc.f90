double precision function fdisc(blockno, x, y, z)
    implicit none

    double precision x,y,z
    integer blockno

    fdisc = sqrt(x*x + y*y + z*z) - 0.25

end function fdisc

