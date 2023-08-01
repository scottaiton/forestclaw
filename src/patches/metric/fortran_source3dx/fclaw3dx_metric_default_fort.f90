!!> @file
!!> Fortran subroutines for metric terms
!!
!! ---------------------------------------------------------
!!> @brief @copybrief ::fclaw3dx_fort_compute_mesh_t
!!>
!!> Default implimentation
!!>
!!> @details @copydetails ::fclaw3dx_fort_compute_mesh_t
!! ---------------------------------------------------------
subroutine fclaw3dx_metric_fort_compute_mesh(mx,my,mz,mbc, &
        xlower,ylower, zlower, dx,dy,dz, blockno, & 
        xp,yp,zp,xd,yd,zd)
    implicit none

    integer mx,my, mz, mbc, blockno
    double precision dx,dy,dz, xlower,ylower, zlower

    double precision xp(-mbc:mx+mbc+1,-mbc:my+mbc+1,-mbc:mz+mbc+1)
    double precision yp(-mbc:mx+mbc+1,-mbc:my+mbc+1,-mbc:mz+mbc+1)
    double precision zp(-mbc:mx+mbc+1,-mbc:my+mbc+1,-mbc:mz+mbc+1)

    double precision xd(-mbc:mx+mbc+2,-mbc:my+mbc+2,-mbc:mz+mbc+2)
    double precision yd(-mbc:mx+mbc+2,-mbc:my+mbc+2,-mbc:mz+mbc+2)
    double precision zd(-mbc:mx+mbc+2,-mbc:my+mbc+2,-mbc:mz+mbc+2)

    integer i,j,k
    double precision dxf,dyf, dzf, xc,yc,zc, xd1, yd1,zd1

    integer*8 map_context_ptr, fclaw_map_get_context

    map_context_ptr = fclaw_map_get_context()

    !! # We need both cell centered and node locations to
    !! # compute the basis at each face
    dxf = dx/2.d0
    dyf = dy/2.d0
    dzf = dz/2.d0

    do i = -2*mbc-1,2*(mx+mbc+1)+1
        do j = -2*mbc-1,2*(my+mbc+1)+1
            do k = -2*mbc-1,2*(mz+mbc+1) + 1
                !! Skip over any values at face centers
                if ((abs(mod(i,2)) .ne. abs(mod(j,2))) .or.  & 
                    (abs(mod(i,2)) .ne. abs(mod(k,2))) .or.  & 
                    (abs(mod(j,2)) .ne. abs(mod(k,2)))) then 
                    cycle
                endif
                !! Compute mesh value on 2x finer mesh; Choose some values for
                !! cell center on coarser mesh; some values for nodes.
                xc = xlower + (i-1)*dxf
                yc = ylower + (j-1)*dyf
                zc = zlower + (k-1)*dzf

                call fclaw3dx_map_c2m(map_context_ptr, &
                                     blockno,xc,yc,zc,xd1,yd1,zd1)

                !! whole integer indices are cell centers. 
                if (abs(mod(i,2)) .eq. 1) then
                    !! # For odd values mesh values
                    !! # Physical location of cell vertices
                    xd((i-1)/2 + 1, (j-1)/2 + 1, (k-1)/2 + 1) = xd1
                    yd((i-1)/2 + 1, (j-1)/2 + 1, (k-1)/2 + 1) = yd1
                    zd((i-1)/2 + 1, (j-1)/2 + 1, (k-1)/2 + 1) = zd1
                else
                    !! # Physical locations of cell centers
                    xp(i/2,j/2,k/2) = xd1
                    yp(i/2,j/2,k/2) = yd1
                    zp(i/2,j/2,k/2) = zd1
                endif
            end do
        end do
    end do
end subroutine fclaw3dx_metric_fort_compute_mesh


!! ----------------------------------------------------------
!!> @brief @copybrief ::fclaw3dx_fort_compute_volume_t
!!>
!!> Default implementation
!!>
!!> @details @copydetails ::fclaw3dx_fort_compute_volume_t
!! ---------------------------------------------------------
subroutine fclaw3dx_metric_fort_compute_volume(mx,my,mz, mbc,blockno, & 
           dx,dy,dz, xlower, ylower, zlower, xd,yd,zd, & 
           volume, faceareas, hexsize, hexstore, ghost_only)
    implicit none

    integer mx,my,mz,mbc,blockno, hexsize
    double precision hexstore(0:hexsize,0:hexsize,0:hexsize,3)
    double precision dx,dy, dz, xlower, ylower, zlower
    integer ghost_only
    double precision volume(-mbc:mx+mbc+1,-mbc:my+mbc+1, -mbc:mz+mbc+1)
    double precision faceareas(-mbc:mx+mbc+1,-mbc:my+mbc+1, -mbc:mz+mbc+2,3)

    double precision xd(-mbc:mx+mbc+2,-mbc:my+mbc+2,-mbc:mz+mbc+2)
    double precision yd(-mbc:mx+mbc+2,-mbc:my+mbc+2,-mbc:mz+mbc+2)
    double precision zd(-mbc:mx+mbc+2,-mbc:my+mbc+2,-mbc:mz+mbc+2)

    logical isaffine


    if (isaffine()) then
        !! # We don't need to compute areas all the way to the
        !! # finest level.
        call fclaw3dx_metric_fort_compute_volume_affine(mx, my, mz, mbc, blockno, & 
                    dx,dy,dz, xlower, ylower, zlower, xd,yd,zd, volume, & 
                    faceareas, ghost_only)
    else
        call fclaw3dx_metric_fort_compute_volume_general(mx, my, mz, mbc, blockno, & 
                dx, dy, dz, xlower, ylower, zlower, & 
                volume,faceareas, &
                hexsize, hexstore, ghost_only)
    endif
end subroutine fclaw3dx_metric_fort_compute_volume

!! ---------------------------------------------------------
!!> @brief Compute the volume for each cell for general mappings
!!>
!!> @param[in] mx, my, mz the number of cells in the x, y, z directions
!!> @param[in] mbc the number of ghost cells
!!> @param[in] dx, dy, dz the spacings in the x, y and z direcitons
!!> @param[in] xlower, ylower, zlower the lower left coordinate of the patch
!!> @param[in] blockno the block number
!!> @param[out] volume the volume of each cell
!!> @param[in] hexsize the length of the hex
!!> @param[in] hexstore stores a group of cell values
!!> @param[in] ghost_only
!! ---------------------------------------------------------
subroutine fclaw3dx_metric_fort_compute_volume_general(mx,my,mz, mbc, blockno, & 
           dx, dy, dz, xlower, ylower, zlower, volume, &
           faceareas, hexsize, hexfine,ghost_only)
    implicit none

    integer :: mx,my,mz, mbc,blockno, hexsize
    double precision :: dx,dy, dz, xlower, ylower, zlower
    double precision :: hexfine(0:hexsize,0:hexsize,0:hexsize,3)
    integer :: ghost_only

    double precision volume(-mbc:mx+mbc+1,-mbc:my+mbc+1,-mbc:mz+mbc+1)
    double precision faceareas(-mbc:mx+mbc+1,-mbc:my+mbc+1,-mbc:mz+mbc+2,3)

    integer i,j,k,ii,jj, kk, m, rfactor, icell, jcell, kcell
    double precision sum_volume
    double precision xp1,yp1,zp1
    double precision hex(0:1,0:1,0:1,3)
    double precision area(3), sum_face_area(3)

    double precision hex_compute_volume

    double precision dxf, dyf, dzf
    double precision xef, yef, zef, xe,ye, ze
    logical hex_is_volume_interior

    integer*8 cont, fclaw_map_get_context

    !!logical isaffine

    cont = fclaw_map_get_context()

    rfactor = hexsize

    dxf = dx/rfactor
    dyf = dy/rfactor
    dzf = dz/rfactor

    do j = -mbc,my+mbc+1
        do i = -mbc,mx+mbc+1
            do k = -mbc,mz+mbc+1

                !! Skip interior cells if we are only trying to rebuild
                !! metric terms in ghost cells
                if (hex_is_volume_interior(mx,my,mz,i,j,k) .and. & 
                    ghost_only .eq. 1) then
                    cycle
                endif
                xe = xlower + (i-1)*dx
                ye = ylower + (j-1)*dy
                ze = zlower + (k-1)*dz

                !! For each coarse grid cell, construct a local finer mesh at
                !! maximum level of refinement
                do ii = 0,rfactor
                    do jj = 0,rfactor
                        do kk = 0,rfactor
                            xef = xe + ii*dxf
                            yef = ye + jj*dyf
                            zef = ze + kk*dzf

                            call fclaw3dx_map_c2m(cont, & 
                                blockno,xef,yef,zef, xp1,yp1,zp1)

                            hexfine(ii,jj,kk,1) = xp1
                            hexfine(ii,jj,kk,2) = yp1
                            hexfine(ii,jj,kk,3) = zp1
                        end do
                    end do
                end do

                !! Compute volume of each of the "sub-hexes"
                sum_volume = 0.d0
                do m = 1,3
                    sum_face_area(m) = 0.d0
                enddo
                do ii = 0,rfactor-1
                    do jj = 0,rfactor-1
                        do kk = 0, rfactor-1

                            !! Get single fine grid hex cell
                            do icell = 0,1
                                do jcell = 0,1
                                    do kcell = 0,1
                                        do m = 1,3
                                            hex(icell,jcell,kcell,m) =  & 
                                              hexfine(ii+icell,jj+jcell,kk+kcell,m)
                                        end do
                                    end do
                                end do
                            end do

                            !! Compute volume of sub-hex at finest level
                            sum_volume = sum_volume + hex_compute_volume(hex)

                            !! Compute face areas by summing areas over sub-hexes
                            !! at faces which intersect coarser grid face
                            if (ii .eq. 0 .or. & 
                                jj .eq. 0 .or. & 
                                kk .eq. 0) then
                                call hex_compute_surf_area(hex,area)
                            endif

                            if (ii .eq. 0) then
                                sum_face_area(1) = sum_face_area(1) + area(1)
                            endif
                            if (jj .eq. 0) then
                                sum_face_area(2) = sum_face_area(2) + area(2)
                            endif
                            if (kk .eq. 0) then
                                sum_face_area(3) = sum_face_area(3) + area(3)
                            endif
                        end do
                    end do
                end do
                volume(i,j,k) = sum_volume

                do m = 1,3
                    faceareas(i,j,k,m) = sum_face_area(m)
                end do

            end do
        end do
    end do
end subroutine fclaw3dx_metric_fort_compute_volume_general


!! ---------------------------------------------------------
!!> @brief Compute the area for each cell for affine mappings.
!!>
!!> If the mapping is affine, (e.g. Ax + b) then we don't need to sum
!!> the finer level areas.
!!>
!!> @param[in] mx, my the number of cells in the x and y directions
!!> @param[in] mbc the number of ghost cells
!!> @param[in] dx, dy the spacings in the x and y direcitons
!!> @param[in] xlower, ylower the lower left coordinate of the patch
!!> @param[in] blockno the block number
!!> @param[out] area the area of each cell
!!> @param[in] quadsize the length of the quad
!!> @param[in] quadstore stores a group of cell values
!!> @param[in] ghost_only
!! ---------------------------------------------------------
 


subroutine fclaw3dx_metric_fort_compute_volume_affine(mx,my,mz, mbc, blockno, & 
        dx,dy,dz,xlower, ylower, zlower, xd,yd,zd, & 
        volume, faceareas, ghost_only)
    implicit none

    integer mx,my,mz, mbc, blockno
    double precision xlower, ylower, zlower, dx, dy, dz
    integer ghost_only

    double precision volume(-mbc:mx+mbc+1,-mbc:my+mbc+1,-mbc:mz+mbc+1)
    double precision faceareas(-mbc:mx+mbc+1,-mbc:my+mbc+1,-mbc:mz+mbc+2,3)

    double precision xd(-mbc:mx+mbc+2,-mbc:my+mbc+2,-mbc:mz+mbc+2)
    double precision yd(-mbc:mx+mbc+2,-mbc:my+mbc+2,-mbc:mz+mbc+2)
    double precision zd(-mbc:mx+mbc+2,-mbc:my+mbc+2,-mbc:mz+mbc+2)

    integer i,j, k, m, ii, jj, kk
    double precision hex(0:1,0:1,0:1,3)
    double precision hex_compute_volume
    logical hex_is_volume_interior

    double precision xd1, yd1, zd1, area(3), xe, ye, ze

    integer*8 cont, fclaw_map_get_context

    cont = fclaw_map_get_context()

    !! We could use pre-defined (xd,yd,zd) arrays here, but they are not 
    !! always available when building a patch. For example, when we build 
    !! a parallel ghost patch, we don't call the meshing routine to get 
    !! (xd,yd,zd).  The (xd,yd,zd) routines passed may already be defined, 
    !! but get redefined here. 

    do j = -mbc,my+mbc+2
        do i = -mbc,mx+mbc+2
            do k = -mbc,mz+mbc+2
                xe = xlower + (i-1)*dx
                ye = ylower + (j-1)*dy
                ze = zlower + (k-1)*dz

                call fclaw3dx_map_c2m(cont, & 
                                blockno,xe,ye,ze, xd1,yd1,zd1)

                xd(i,j,k) = xd1
                yd(i,j,k) = yd1
                zd(i,j,k) = zd1
            end do
        end do
    end do


    !! This basically doubles the amount of work required, and we can compute the 
    !! affine mesh using (xd,yd,zd).  But we'd have to call the mesh generation
    !! routine
    do j = -mbc,my+mbc+1
        do i = -mbc,mx+mbc+1
            do k = -mbc,mz+mbc+1
                if (hex_is_volume_interior(mx,my,mz,i,j,k) .and. & 
                    ghost_only .eq. 1) then
                    cycle
                endif

                !! For each coarse grid cell, construct a local finer mesh at
                !! maximum level of refinement
                do ii = 0,1
                    do jj = 0,1
                        do kk = 0,1
                            hex(ii,jj,kk,1) = xd(i+ii,j+jj,k+kk)
                            hex(ii,jj,kk,2) = yd(i+ii,j+jj,k+kk)
                            hex(ii,jj,kk,3) = zd(i+ii,j+jj,k+kk)
                        end do
                    end do
                end do

                volume(i,j,k) = hex_compute_volume(hex)

                call hex_compute_surf_area(hex,area)
                do m = 1,3
                    faceareas(i,j,k,m) = area(m)                    
                end do
            end do
        end do
    end do
end subroutine fclaw3dx_metric_fort_compute_volume_affine

!! Since basis vectors are computed on a coarse grid (assuming coarse rule
!! surface approximation at coarse grid cell face), we can compute a basis
!! independent of the volume calculation.  This is needed, for example, if
!! we re-build a coarse grid from fine grids
subroutine fclaw3dx_metric_fort_compute_basis(mx, my, mz, mbc, xd, yd, zd, & 
          xrot, yrot, zrot, ghost_only)
    implicit none

    integer mx,my,mz, mbc
    integer ghost_only

    double precision xd(-mbc:mx+mbc+2,-mbc:my+mbc+2,-mbc:mz+mbc+2)
    double precision yd(-mbc:mx+mbc+2,-mbc:my+mbc+2,-mbc:mz+mbc+2)
    double precision zd(-mbc:mx+mbc+2,-mbc:my+mbc+2,-mbc:mz+mbc+2)

    double precision xrot(-mbc:mx+mbc+1,-mbc:my+mbc+1,-mbc:mz+mbc+2,3,3)
    double precision yrot(-mbc:mx+mbc+1,-mbc:my+mbc+1,-mbc:mz+mbc+2,3,3)
    double precision zrot(-mbc:mx+mbc+1,-mbc:my+mbc+1,-mbc:mz+mbc+2,3,3)

    integer i,j, k, ii,jj, icell, jcell, kcell
    !! double precision xcorner, ycorner, zcorner
    !! double precision xe, ye,ze, xp1, yp1, zp1
    double precision hex(0:1,0:1,0:1,3)

    logical hex_is_volume_interior

    double precision rot(3,3,3)

    integer*8 map_context_ptr, fclaw_map_get_context

    map_context_ptr = fclaw_map_get_context()

    do j = -mbc,my+mbc+1
        do i = -mbc,mx+mbc+1
            do k = -mbc,mz+mbc+1
                if (hex_is_volume_interior(mx,my,mz,i,j,k) .and. & 
                    ghost_only .eq. 1) then
                    cycle
                endif
                !! xe = xlower + (i-1)*dx
                !! ye = ylower + (j-1)*dy
                !! ze = zlower + (k-1)*dz
                do icell = 0,1
                    do jcell = 0,1
                        do kcell = 0,1
!!                            xcorner = xe + icell*dx
!!                            ycorner = ye + jcell*dy
!!                            zcorner = ze + kcell*dz
!!                            call fclaw3dx_map_c2m(map_context_ptr, & 
!!                                blockno,xcorner,ycorner,zcorner,xp1,yp1,zp1)
                            hex(icell,jcell,kcell,1) = xd(i+icell,j+jcell,k+kcell)
                            hex(icell,jcell,kcell,2) = yd(i+icell,j+jcell,k+kcell)
                            hex(icell,jcell,kcell,3) = zd(i+icell,j+jcell,k+kcell)
                        end do
                    end do
                end do
                call hex_compute_basis(hex,rot)
                do jj = 1,3
                    do ii = 1,3
                        xrot(i,j,k,ii,jj) = rot(1,ii,jj)
                        yrot(i,j,k,ii,jj) = rot(2,ii,jj)
                        zrot(i,j,k,ii,jj) = rot(3,ii,jj)
                    end do
                end do
            end do
        end do
    end do
end subroutine fclaw3dx_metric_fort_compute_basis


!! ---------------------------------------------------------
!!> @brief Averages the volume from a fine grid to a coarse grid (3dx only). 
!!>        
!!>
!!> @param[in] mx, my the number of cells in the x and y directions
!!> @param[in] mbc the number of ghost cells
!!> @param[in] volfine the cell areas of the fine patch
!!> @param[out] volcoarse the cell areas of the coarse patch
!!> @param[in] igrid the index of the fine grid in the child array
!! ---------------------------------------------------------
subroutine fclaw3dx_metric_fort_average_volume(mx,my,mz, mbc, & 
           volcoarse, volfine, igrid)
    implicit none

    integer mx,my,mz, mbc, igrid

    !! # these will be empty if we are not on a manifold.
    double precision volcoarse(-mbc:mx+mbc+1,-mbc:my+mbc+1,-mbc:mz+mbc+1)
    double precision   volfine(-mbc:mx+mbc+1,-mbc:my+mbc+1,-mbc:mz+mbc+1)

    integer i,j, k, ig, jg, ic_add, jc_add, ii, jj
    double precision sum

    !! # This should be refratio*refratio.
    integer i1,j1, r2, m
    integer rr2
    parameter(rr2 = 4)
    integer i2(0:rr2-1),j2(0:rr2-1)
    double precision kf

    integer p4est_refineFactor, refratio


    p4est_refineFactor = 2
    refratio = 2

    !! # 'iface' is relative to the coarse grid

    r2 = refratio*refratio
    if (r2 .ne. rr2) then
        write(6,*) 'average_face_ghost (claw2d_utils.f): Refratio**2 is not equal to rr2'
        stop
    endif


    !! # Get (ig,jg) for grid from linear (igrid) coordinates
    ig = mod(igrid,refratio)
    jg = (igrid-ig)/refratio

    !! # Get rectangle in coarse grid for fine grid.
    ic_add = ig*mx/p4est_refineFactor
    jc_add = jg*my/p4est_refineFactor

    r2 = refratio*refratio
    !! This only works for extruded mesh version.  
    !! We include one layer of ghost cells on the coarse grid 
    do k = 0,mz+1
        do j = 0,my/p4est_refineFactor+1
            do i = 0,mx/p4est_refineFactor +1
                i1 = i+ic_add
                j1 = j+jc_add
                m = 0
                do jj = 1,refratio
                    do ii = 1,refratio
                        i2(m) = (i-1)*refratio + ii
                        j2(m) = (j-1)*refratio + jj
                        m = m + 1
                    end do
                end do
                sum = 0
                do m = 0,r2-1
                    kf = volfine(i2(m),j2(m),k)
                    sum = sum + kf
                end do
                volcoarse(i1,j1,k) = sum
            end do
        end do
    end do

end subroutine fclaw3dx_metric_fort_average_volume


!! ---------------------------------------------------------
!!> @brief Averages the volume from a fine grid to a coarse grid (3dx only). 
!!>        
!!>
!!> @param[in] mx, my the number of cells in the x and y directions
!!> @param[in] mbc the number of ghost cells
!!> @param[in] volfine the cell areas of the fine patch
!!> @param[out] volcoarse the cell areas of the coarse patch
!!> @param[in] igrid the index of the fine grid in the child array
!! ---------------------------------------------------------
subroutine fclaw3dx_metric_fort_average_facearea(mx,my,mz, mbc, & 
           fa_coarse, fa_fine, igrid)
    implicit none

    integer mx,my,mz, mbc, igrid

    !! # these will be empty if we are not on a manifold.
    double precision fa_coarse(-mbc:mx+mbc+1,-mbc:my+mbc+1,-mbc:mz+mbc+2,3)
    double precision   fa_fine(-mbc:mx+mbc+1,-mbc:my+mbc+1,-mbc:mz+mbc+2,3)

    integer ic,jc, k, ig, jg, ic_add, jc_add, ii, jj
    double precision sum

    !! # This should be refratio*refratio.
    integer i1,j1, r2, m
    integer rr2
    parameter(rr2 = 4)
    integer i2(0:rr2-1),j2(0:rr2-1)
    double precision kf

    integer p4est_refineFactor, refratio


    p4est_refineFactor = 2
    refratio = 2

    !! # 'iface' is relative to the coarse grid

    r2 = refratio*refratio
    if (r2 .ne. rr2) then
        write(6,*) 'average_face_ghost (claw2d_utils.f): Refratio**2 is not equal to rr2'
        stop
    endif


    !! # Get (ig,jg) for grid from linear (igrid) coordinates
    ig = mod(igrid,refratio)
    jg = (igrid-ig)/refratio

    !! # Get rectangle in coarse grid for fine grid.
    ic_add = ig*mx/p4est_refineFactor
    jc_add = jg*my/p4est_refineFactor

    r2 = refratio*refratio
    !! This only works for extruded mesh version.
    do k = 0,mz+1
        !! Loop over cells in quadrant in coarse grid covered by fine grid.
        do jc = 0,my/2+1
            do ic = 0,mx/2+1
                !! Get region in coarse grid covered by fine grid
                i1 = ic+ic_add
                j1 = jc+jc_add

                !! Set up linear indexing for 2x2 quad grid arrangement
                m = 0
                do jj = 1,2
                    do ii = 1,2
                        i2(m) = 2*(ic-1) + ii
                        j2(m) = 2*(jc-1) + jj
                        m = m + 1
                    end do
                end do

                fa_coarse(i1,j1,k,1) = fa_fine(i2(0),j2(0),k,1) + & 
                                       fa_fine(i2(2),j2(2),k,1)

                fa_coarse(i1,j1,k,2) = fa_fine(i2(0),j2(0),k,2) + & 
                                       fa_fine(i2(1),j2(1),k,2)

                !! Area of z-face
                sum = 0
                do m = 0,3
                    kf = fa_fine(i2(m),j2(m),k,3)
                    sum = sum + kf
                end do
                fa_coarse(i1,j1,k,3) = sum
                if (sum .lt. 0) then
                    write(6,*) 'facearea ', sum
                    stop
                endif

            end do
        end do
    end do

end subroutine fclaw3dx_metric_fort_average_facearea
