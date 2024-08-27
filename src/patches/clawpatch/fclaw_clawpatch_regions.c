/*
Copyright (c) 2012-2022 Carsten Burstedde, Donna Calhoun, Scott Aiton
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <fclaw_regions.h>

#include <fclaw_options.h>
#include <fclaw_global.h>
#include <fclaw_clawpatch.h>

#ifdef __cplusplus
extern "C"
{
#endif


/* This assumes that no mapping is used and that the region is 
   defined in coordinates relative to [ax,bx]x[ay,by].
   If mapped grids are used, then the patch intersection may be 
   more involved
*/
int fclaw_clawpatch_intersects_region(fclaw_global_t *glob,
                                      fclaw_patch_t *patch,
                                      int blockno, int patchno,
                                      fclaw_region_t *r,
                                      double t,
                                      int refine_flag)
{   
    int rmin,rmax,num,dim;
    double xlower_reg, xupper_reg;
    double ylower_reg, yupper_reg;
    double zlower_reg, zupper_reg;
    double t1, t2;
    fclaw_region_get_data(glob,r,&num,&dim,
                          &xlower_reg, &xupper_reg, 
                          &ylower_reg, &yupper_reg, 
                          &zlower_reg, &zupper_reg, 
                          &rmin, &rmax,
                          &t1, &t2);

    /* Do a quick check */
    if (t < t1 || t > t2)        
        return 0;

    // Time is in time interval;  now check geometric intervals
    int mx,my,mz,mbc;
    double dx,dy,dz;
    double xlower,ylower,zlower;
    double xupper,yupper,zupper;
    zlower = 0;
    zupper = 1;
    if (dim == 2)
    {
        // We are refining 
        fclaw_clawpatch_2d_grid_data(glob,patch,&mx,&my,&mbc,
                                         &xlower,&ylower,&dx,&dy);
    }
    else if (dim == 3)
    {
        fclaw_clawpatch_3d_grid_data(glob,patch,&mx,&my,&mz,&mbc,
                                     &xlower,&ylower,&zlower,
                                     &dx,&dy,&dz);
    }

    xupper = xlower + mx*dx;
    yupper = ylower + my*dy;
    if (dim == 3)
        zupper = zlower + mz*dz;

    if (refine_flag == 0)
    {
        /* We are coarsening and have to get upper edge from sibling patches */
        xupper += mx*dx;
        yupper += my*dy;
        if (dim == 3)
            zupper += mz*dz;
    }


    if (xlower > xupper_reg || xupper < xlower_reg)
        return 0;

    if (ylower > yupper_reg || yupper < ylower_reg)
        return 0;

    if (dim == 3)
        if (zlower > zupper_reg || zupper < zlower_reg)
            return 0;

    return 1;
}

#ifdef __cplusplus
}
#endif

