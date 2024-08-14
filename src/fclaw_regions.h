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

#ifndef FCLAW_REGIONS_H
#define FCLAW_REGIONS_H

#ifdef __cplusplus
extern "C"
{
#endif

#if 0
/* Fix syntax highlighting */
#endif    

/**
 * @file
 * @brief Regions
 */


/**
 * @brief Gauge structure
 */
typedef struct fclaw_region
{
    /** @{ @brief Relative to [ax,ay]x[bx,by] (x[az,bz]) **/
    double xlower;   
    double xupper;
    double ylower;
    double yupper;
    double zlower;
    double zupper;
    /** @} */

    int min_level;
    int max_level;

    /** @brief Tstart */
    double t1;

    /** @brief Tend */
    double t2;

    /** @brief Region ID */
    int num;

    /** @brief region dimension (2 or 3) */
    int dim;

    /** @brief User data */
    void* user_data;

} fclaw_region_t;

struct fclaw_global;
struct fclaw_patch;
struct fclaw_block;

/**
 * @brief Sets the data for each region
 * 
 * @param[in] glob the global context
 * @param[in,out] regions the array of regions
 * @param[in] num_regions the number of regions
 */
typedef void (*fclaw_region_set_data_t)(struct fclaw_global *glob, 
                                       struct fclaw_region **regions, 
                                       int *num,
                                       int *dim);

/**
 * @brief Maps region to normalized coordinates in a global [0,1]x[0,1]  domain.
 * 
 * @param[in] glob the global context
 * @param[in] block the block that the region is in
 * @param[in] blockno the block number that the region is in
 * @param[in] r the region
 * @param[out] xlower,xupper,ylower,yupper,zlower,zupper the normalized coordinates
 */
typedef void (*fclaw_region_normalize_t)(struct fclaw_global *glob, 
                                         struct fclaw_block *block,
                                         int blockno, 
                                         struct fclaw_region *r,
                                         double *xlower, double *xupper, 
                                         double *ylower, double *yupper, 
                                         double *zlower, double *zupper);


typedef int (*fclaw_region_intersects_patch_t)(struct fclaw_global *glob,
                                               struct fclaw_patch *patch,
                                               int blockno, int patchno,
                                               struct fclaw_region *r,
                                               double t,
                                               int refine_flag);


/**
 * @brief vtable for regions
 */
typedef struct fclaw_regions_vtable
{
    /** @brief Sets the data for each region */
    fclaw_region_set_data_t      set_region_data;

    /** @brief Maps region to normalized coordinates in a 
        global [0,1]x[0,1] (x[0,1]) domain. */
    fclaw_region_normalize_t     normalize_coordinates;

    /** @brief Routine to determine if patch intersections a 
        region.  */
    fclaw_region_intersects_patch_t   patch_intersects_region;

    /** @brief true if vtable has been set */
    int is_set;
} fclaw_regions_vtable_t;

/**
 * @brief Initialize the regions vtable
 * 
 * @param glob the global context
 */
void fclaw_regions_vtable_initialize(struct fclaw_global *glob);

/**
 * @brief Initialize the regions
 * 
 * @param glob the global context
 */
void fclaw_regions_initialize(struct fclaw_global *glob);


/**
 * @brief Get the regions vtable
 * 
 * @param glob the global context
 * @return fclaw_regions_vtable_t* the vtable
 */
fclaw_regions_vtable_t* fclaw_regions_vt(struct fclaw_global *glob);



/* ------------------------ Virtualized region functions ------------------------------- */

/**
 * @brief Set the data for each region
 * 
 * @param glob the global context
 * @param regions the array of regions
 * @param num_regions the number of regions
 */
#if 0
void fclaw_set_region_data(struct fclaw_global* glob, 
                          struct fclaw_region **regions, 
                          int *num_regions);
#endif

/**
 * @brief Map region to normalized coordinates in a global 
 * [0,1]x[0,1] (x[0,1])  domain.
 * 
 * @param[in] glob the global context
 * @param[in] block the block that the region is in
 * @param[in] blockno the block number that the region is in
 * @param[in] r the region
 * @param[out] xc,yc,zc the normalized coordinates
 */
void fclaw_region_normalize_coordinates(struct fclaw_global *glob, 
                                      struct fclaw_block *block,
                                      int blockno, 
                                      struct fclaw_region *r,
                                      double *xlower, double *xupper, 
                                      double *ylower, double *yupper, 
                                      double *zlower, double *zupper);


int fclaw_region_intersects_patch(struct fclaw_global *glob,
                                  struct fclaw_patch *patch,
                                  int blockno, int patchno,
                                  struct fclaw_region *r,
                                  double t, int refine_flag);

/* ---------------------------------- Regions ------------------------------------------ */

/**
 * @brief Test whether patch is in set of regions
 * 
 * @param[in] glob Global context
 * @param[in] patch Patch to check
 * @param[in] regions List of regions
 * @param[in] num_regions Number of regions
 * @param[in] t Current time
 * @param[in] refine_flag Flag determining whether we are refining or
 *                        coarsening.
 */

int fclaw2d_regions_test(struct fclaw_global *glob, 
                         struct fclaw_patch *patch,
                         int blockno, int patchno,
                         double t, int refine_flag);


/**
 * @brief Allocate region structures
 * 
 * @param[in] glob the global context
 * @param[in] num_regions the number of regions
 * @param[out] r allocated array of regions 
 */
void fclaw_regions_allocate(struct fclaw_global *glob, 
                           int num_regions,
                           struct fclaw_region **r);


/**
 * @brief Deallocate region structures
 * 
 * @param[in] glob the global context
 */
void fclaw_regions_finalize(struct fclaw_global *glob);

/**
 * @brief Set the data for a single region in 2d/3d
 * 
 * @param[in] glob the global context
 * @param[in,out] r the region
 * @param[in] num the region number (index the the guage array)
 * @param[in] xc, yc, zc Relative to [ax,bx]x[ay,by]x[az,bz] set in fclaw_options 
 * @param[in] t1 Tstart
 * @param[in] t2 Tend
 */
void fclaw_region_set_data(struct fclaw_global *glob, 
                          struct fclaw_region *r,  
                          int num, int dim,
                          double xlower, double xupper, 
                          double ylower, double yupper, 
                          double zlower, double zupper, 
                          int min_level, int max_level,
                          double  t1, double t2);


/**
 * @brief Get the data for a single region
 * 
 * @param[in] glob the global context
 * @param[in] r the region
 * @param[out] num the region number (index the the guage array)
 * @param[out] xc, yc, zc Relative to [ax,bx]x[ay,by]x[az,bz] set in fclaw2d_options 
 * @param[out] t1 Tstart
 * @param[out] t2 Tend
 */
void fclaw_region_get_data(struct fclaw_global *glob, 
                           struct fclaw_region *r,                             
                           int *num, int *dim,
                           double *xlower, double *xupper, 
                           double *ylower, double *yupper, 
                           double *zlower, double *zupper, 
                           int *min_level, int *max_level,
                           double  *t1, double *t2);

/**
 * @brief Get the region number (index the the region array)
 * 
 * @param glob the global context
 * @param r the region
 * @return int the region number
 */
int fclaw_region_get_id(struct fclaw_global *glob, 
                       struct fclaw_region *r);
    
/**
 * @brief the the user data for a region
 * 
 * @param[in] glob the global context
 * @param[in] r the region
 * @param[in] user the user data
 */
void fclaw_region_set_user_data(struct fclaw_global *glob,
                               struct fclaw_region* g,
                               void* user);

/**
 * @brief Get the user data for region
 * 
 * @param glob the global context
 * @param r the region
 * @return void* the user data
 */
void* fclaw_region_get_user_data(struct fclaw_global *glob,
                                struct fclaw_region* g);


#ifdef __cplusplus
}
#endif

#endif
