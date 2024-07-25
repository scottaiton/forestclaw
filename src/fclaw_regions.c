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
//#include <fclaw_convenience.h>  /* Needed to get search function for gaugess */
//#include <fclaw_diagnostics.h>

/* Some mapping functions */
// #include <fclaw_map_brick.h>
// #include <fclaw_map.h>
// #include <fclaw_map_query.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* -------------------------------------------------------------------------------------*/

/* This is only used for blocks */
typedef struct fclaw_region_info
{
    fclaw_region_t *regions;
    int num_regions;
} fclaw_region_info_t;


static
void region_set_data(fclaw_global_t* glob, 
                    fclaw_region_t **regions, 
                    int *num_regions,
                    int* dim);


void fclaw_regions_initialize(fclaw_global_t* glob)
{
    const fclaw_options_t * fclaw_opt = fclaw_get_options(glob);

    /* ------------------------------------------------------------------
       These two calls are the only calls that should worry about the format
       GeoClaw of the files regions.data (created with make_data.py) and 
       region output files (e.g. region00123.txt)
       ---------------------------------------------------------------- */
 
    fclaw_region_info_t *region_info = FCLAW_ALLOC_ZERO(fclaw_region_info_t,1);

    int num_regions, region_dim;
    if (!fclaw_opt->use_regions)
    {
        /* User does not want any region output, so no point in creating regions */
        region_info = NULL;
        num_regions = 0;
        region_dim = -1;
    }
    else
    {
        /* 
            Read custom regions file (e.g. regions.data).  After this call, all
            regions have current data 
        */
        region_set_data(glob, &region_info->regions, &num_regions, &region_dim);
    }

    fclaw_global_attribute_store(glob,
                                 "region_info",
                                 region_info,
                                 NULL,
                                 NULL);
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
                     fclaw_region_t **regions, 
                     int *num_regions,
                     int *dim)
    {
    const fclaw_regions_vtable_t* region_vt = fclaw_regions_vt(glob);
    if (region_vt->set_region_data == NULL)
    {
        *regions = NULL;
        *num_regions = 0;
        *dim = 0;
    }
    else
    {
        region_vt->set_region_data(glob, regions, num_regions,dim);  
    }
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


/* ---------------------------- Get Access Functions ---------------------------------- */

void fclaw_region_allocate(fclaw_global_t *glob, 
                           int num_regions,
                           fclaw_region_t **r)
{
    *r = (fclaw_region_t*) FCLAW_ALLOC(fclaw_region_t,num_regions);
}

void fclaw_region_set_data(fclaw_global_t *glob, 
                             fclaw_region_t *r,
                             int num, int dim,
                             double xlower, double xupper, 
                             double ylower, double yupper, 
                             double zlower, double zupper, 
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
    r->t1 = t1;
    r->t2 = t2;
}

void fclaw_region_get_data(fclaw_global_t *glob, 
                          fclaw_region_t *r,
                          int *num, int *dim,
                          double *xlower, double *xupper, 
                          double *ylower, double *yupper, 
                          double *zlower, double *zupper, 
                          double  *t1, double *t2)
{
    *num = r->num;
    *dim = r->dim;
    *xlower  = r->xlower;
    *ylower  = r->ylower;
    *zlower  = r->zlower;
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
