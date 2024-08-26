/*
Copyright (c) 2012-2024 Carsten Burstedde, Donna Calhoun, Scott Aiton
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

/* -------------------------------------------------------------------------------------*/

/* Data structure to store in the glob */
typedef struct fclaw_region_info
{
    fclaw_region_t *regions;
    int num_regions;
    int region_dim;
} fclaw_region_info_t;


static
void region_set_data(fclaw_global_t* glob, 
                    fclaw_region_info_t *region_info);


void fclaw_regions_initialize(fclaw_global_t* glob)
{
    const fclaw_options_t * fclaw_opt = fclaw_get_options(glob);

    fclaw_region_info_t *region_info = FCLAW_ALLOC_ZERO(fclaw_region_info_t,1);

    if (!fclaw_opt->use_regions)
    {
        /* User does not want any region output, so no point in creating regions */
        region_info->regions = NULL;
        region_info->num_regions = 0;
        region_info->region_dim = -1;  
    }
    else
    {
        /* 
            Read custom regions file (e.g. regions.data).  After this call, all
            regions have current data 
        */
        region_set_data(glob, region_info);
    }

    fclaw_global_attribute_store(glob,
                                 "region_info",
                                 region_info,
                                 NULL,
                                 NULL);
}


int fclaw2d_regions_test(fclaw_global_t *glob, 
                         fclaw_patch_t *patch,
                         int blockno, int patchno,
                         double t, int refine_flag)
{
    fclaw_region_info_t* region_info = 
        (fclaw_region_info_t *) fclaw_global_get_attribute(glob,"region_info");

    fclaw_region_t *regions = region_info->regions;
    int num_regions = region_info->num_regions;

    // Get list of regions that this patch intersects
    // If we are coarsening, the "patch" dimensions are the dimensions of the 
    // quadrant occupied by parent quadrant, i.e. the coarsened patch.  But 'level'
    // is the level of the four siblings.
    int inregion[num_regions];
    int region_found = 0;
    for(int m = 0; m < num_regions; m++)
    {
        inregion[m] = fclaw_region_intersects_patch(glob,patch, 
                                                    blockno, patchno,
                                                    &regions[m],
                                                    t,refine_flag);
        if (inregion[m])
            region_found = 1;
    }

    int tag_patch = -1;
    if (!region_found)
    {
        // Patch does not intersect any region, and so  
        // refinement is based on usual tagging criteria.
        return tag_patch;
    }

    /* Find minimum and maximum levels for regions intersected by this patch
       1. We want to find the most restrictive minimum level this patch
       intersects. This means that we look for the region with the largest
       minimum level.  If this region says we should refine, then we refine.
       Criteria for smaller min_level values will automatically be 
       satisfied. 

       2.  We find the smallest max_level value of all regions intersecting
       this patch.  If we do not refine beyond this value, all larger max_level
       criteria will be satisfied.
    */

    int min_level = 0;    // larger than any possible number of levels
    int max_level = 0;
    for(int m = 0; m < num_regions; m++)
    {
        if (inregion[m])
        {
            int rmin,rmax,num,dim;
            double xlower,xupper,ylower,yupper,zlower,zupper;
            double t1, t2;
            fclaw_region_get_data(glob,&regions[m],&num,&dim,
                                  &xlower, &xupper, 
                                  &ylower, &yupper, 
                                  &zlower, &zupper, 
                                  &rmin, &rmax,
                                  &t1, &t2);

            min_level = fmax(min_level,rmin);
            max_level = fmax(max_level,rmax);
        }
    }

    // Determine if we are allowed to refine based on regions above.
    if (refine_flag != 0)
    {
        // We are tagging for refinement
        if (patch->level < min_level)
        {
            // At least one region says we have to refine.
            tag_patch = 1;      
        }
        else if (patch->level >= max_level)
        {
            // At least one region says we cannot refine
            tag_patch = 0;
        }
    }
    else
    {
        // We are tagging for coarsening
        if (patch->level <= min_level)
        {
            // At least one region says we cannot coarsen below
            // patch->level
            tag_patch = 0;
        }
        else if (patch->level > max_level)
        {
            // At least one region says we have to coarsen
            tag_patch = 1;            
        }
    }
    return tag_patch;
}


/* ---------------------------------- Virtual table  ---------------------------------- */
static
fclaw_regions_vtable_t* fclaw_regions_vt_new()
{
    return (fclaw_regions_vtable_t*) FCLAW_ALLOC_ZERO (fclaw_regions_vtable_t, 1);
}

static
void fclaw_regions_vt_destroy(void* vt)
{
    FCLAW_FREE (vt);
}

fclaw_regions_vtable_t* fclaw_regions_vt(fclaw_global_t* glob)
{
	fclaw_regions_vtable_t* regions_vt = (fclaw_regions_vtable_t*) 
	   							fclaw_global_get_vtable(glob, "fclaw_regions");
	FCLAW_ASSERT(regions_vt != NULL);
	FCLAW_ASSERT(regions_vt->is_set != 0);

    return regions_vt;
}

void fclaw_regions_vtable_initialize(fclaw_global_t* glob)
{
    fclaw_regions_vtable_t* regions_vt = fclaw_regions_vt_new();

    regions_vt->is_set = 1;

	fclaw_global_vtable_store(glob, "fclaw_regions", regions_vt, 
                              fclaw_regions_vt_destroy);
}
/* ---------------------------- Virtualized Functions --------------------------------- */

static
void region_set_data(fclaw_global_t* glob, 
                     fclaw_region_info_t *region_info)
{
    const fclaw_regions_vtable_t* region_vt = fclaw_regions_vt(glob);
    FCLAW_ASSERT(region_vt->set_region_data != NULL);    
    region_vt->set_region_data(glob, 
                               &region_info->regions, 
                               &region_info->num_regions,
                               &region_info->region_dim);  
}

void fclaw_region_normalize_coordinates(fclaw_global_t *glob, 
                                      fclaw_block_t *block,
                                      int blockno, 
                                      fclaw_region_t *r,
                                      double *xlower, double *xupper, 
                                      double *ylower, double *yupper, 
                                      double *zlower, double *zupper) 
{
    const fclaw_regions_vtable_t* region_vt = fclaw_regions_vt(glob);
    FCLAW_ASSERT(region_vt->normalize_coordinates != NULL);
    region_vt->normalize_coordinates(glob, block,blockno,r,
                                     xlower,xupper,
                                     ylower,yupper,
                                     zlower,zupper);    
}

int fclaw_region_intersects_patch(struct fclaw_global *glob,
                                  struct fclaw_patch *patch,
                                  int blockno, int patchno,
                                  struct fclaw_region *r,
                                  double t, int refine_flag)
{
    const fclaw_regions_vtable_t* region_vt = fclaw_regions_vt(glob);
    FCLAW_ASSERT(region_vt->patch_intersects_region != NULL);    
    int in_region = region_vt->patch_intersects_region(glob,patch,
                                                       blockno,patchno,
                                                       r,t,refine_flag);
    return in_region;
}



/* ---------------------------- Get Access Functions ---------------------------------- */

void fclaw_regions_allocate(fclaw_global_t *glob, 
                           int num_regions,
                           fclaw_region_t **r)
{
    *r = (fclaw_region_t*) FCLAW_ALLOC(fclaw_region_t,num_regions);
}

void fclaw_regions_finalize(fclaw_global_t *glob)
{
    fclaw_region_info_t* region_info = 
        (fclaw_region_info_t *) fclaw_global_get_attribute(glob,"region_info");

    fclaw_region_t *regions = region_info->regions;
    FCLAW_FREE(regions);
    FCLAW_FREE(region_info);
    region_info = NULL;
    regions = NULL;
}


void fclaw_region_set_data(fclaw_global_t *glob, 
                             fclaw_region_t *r,
                             int num, int dim,
                             double xlower, double xupper, 
                             double ylower, double yupper, 
                             double zlower, double zupper, 
                             int min_level, int max_level,
                             double  t1, double t2)
{
    r->num = num;
    r->dim = dim;
    r->xlower = xlower;
    r->xupper = xupper;
    r->ylower = ylower;
    r->yupper = yupper;
    r->zlower = zlower;
    r->zupper = zupper;
    r->min_level = min_level;
    r->max_level = max_level;
    r->t1 = t1;
    r->t2 = t2;
}

void fclaw_region_get_data(fclaw_global_t *glob, 
                          fclaw_region_t *r,
                          int *num, int *dim,
                          double *xlower, double *xupper, 
                          double *ylower, double *yupper, 
                          double *zlower, double *zupper, 
                          int *min_level, int *max_level,
                          double  *t1, double *t2)
{
    *num = r->num;
    *dim = r->dim;
    *xlower  = r->xlower;
    *xupper  = r->xupper;
    *ylower  = r->ylower;
    *yupper  = r->yupper;
    *zlower  = r->zlower;
    *zupper  = r->zupper;
    *min_level = r->min_level;
    *max_level = r->max_level;
    *t1  = r->t1;
    *t2  = r->t2;
}

int fclaw_region_get_id(fclaw_global_t *glob, 
                        fclaw_region_t *r)
{
    return r->num;
}


void fclaw_region_set_user_data(fclaw_global_t *glob,
                               fclaw_region_t* r,
                               void* user)
{
    r->user_data = user;
}

void* fclaw_region_get_user_data(fclaw_global_t *glob,
                                 fclaw_region_t* r)
{
    return r->user_data;
}


#ifdef __cplusplus
}
#endif
