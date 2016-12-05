/*
Copyright (c) 2012 Carsten Burstedde, Donna Calhoun
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

#include <fclaw2d_global.h>

#include <fclaw2d_regrid.h>
#include <fclaw2d_ghost_fill.h>
#include <fclaw_timer.h>

#include <fclaw2d_forestclaw.h>
#include <fclaw2d_partition.h>
#include <fclaw2d_exchange.h>
#include <fclaw2d_vtable.h>
#include <fclaw2d_clawpatch.h>

/* This is also called from fclaw2d_initialize, so is not made static */
void cb_fclaw2d_regrid_tag4refinement(fclaw2d_domain_t *domain,
                                      fclaw2d_patch_t *this_patch,
                                      int this_block_idx,
                                      int this_patch_idx,
                                      void *user)
{
    fclaw2d_vtable_t vt;
    int refine_patch, maxlevel, level;
    const amr_options_t* gparms;

    int domain_init = *((int*) user);

    vt = fclaw2d_get_vtable(domain);
    gparms = get_domain_parms(domain);

    maxlevel = gparms->maxlevel;
    level = this_patch->level;

    if (level < maxlevel)
    {
        refine_patch  =
            vt.regrid_tag4refinement(domain,this_patch,this_block_idx,
                                    this_patch_idx, domain_init);
        if (refine_patch == 1)
        {
            fclaw2d_patch_mark_refine(domain, this_block_idx, this_patch_idx);
        }
    }
}

/* Tag family for coarsening */
static
void cb_regrid_tag4coarsening(fclaw2d_domain_t *domain,
                              fclaw2d_patch_t *fine_patches,
                              int blockno, int fine0_patchno,
                              void *user)
{
    const amr_options_t *gparms = get_domain_parms(domain);
    fclaw2d_vtable_t vt;
    vt = fclaw2d_get_vtable(domain);

    int minlevel = gparms->minlevel;

    int level = fine_patches[0].level;

    if (level > minlevel)
    {
        int family_coarsened = 1;
        family_coarsened = vt.regrid_tag4coarsening(domain,&fine_patches[0],
                                                  blockno, fine0_patchno);
        if (family_coarsened == 1)
        {
            int igrid;
            for (igrid = 0; igrid < NumSiblings; igrid++)
            {
                int fine_patchno = fine0_patchno + igrid;
                fclaw2d_patch_mark_coarsen(domain,blockno, fine_patchno);
            }
        }
    }
}


/* ----------------------------------------------------------------
   Public interface
   -------------------------------------------------------------- */

void cb_fclaw2d_regrid_repopulate(fclaw2d_domain_t * old_domain,
                                  fclaw2d_patch_t * old_patch,
                                  fclaw2d_domain_t * new_domain,
                                  fclaw2d_patch_t * new_patch,
                                  fclaw2d_patch_relation_t newsize,
                                  int blockno,
                                  int old_patchno,
                                  int new_patchno,
                                  void *user)
{
    fclaw2d_vtable_t vt;
    vt = fclaw2d_get_vtable(new_domain);

    int domain_init = *((int*) user);

    fclaw2d_domain_data_t *ddata_old = fclaw2d_domain_get_data (old_domain);
    fclaw2d_domain_data_t *ddata_new = fclaw2d_domain_get_data (new_domain);
    fclaw2d_build_mode_t build_mode = FCLAW2D_BUILD_FOR_UPDATE;

    if (newsize == FCLAW2D_PATCH_SAMESIZE)
    {
        new_patch->user = old_patch->user;
        old_patch->user = NULL;
        ++ddata_old->count_delete_clawpatch;
        ++ddata_new->count_set_clawpatch;
    }
    else if (newsize == FCLAW2D_PATCH_HALFSIZE)
    {
        fclaw2d_patch_t *fine_siblings = new_patch;
        fclaw2d_patch_t *coarse_patch = old_patch;

        int i;
        for (i = 0; i < NumSiblings; i++)
        {
            fclaw2d_patch_t *fine_patch = &fine_siblings[i];
            int fine_patchno = new_patchno + i;
            fclaw2d_patch_data_new(new_domain,fine_patch);
            fclaw2d_clawpatch_build(new_domain,fine_patch,blockno,
                                    fine_patchno,(void*) &build_mode);
            if (domain_init)
            {
                vt.patch_initialize(new_domain,fine_patch,blockno,fine_patchno);
            }
        }

        if (!domain_init)
        {
            int coarse_patchno = old_patchno;
            int fine_patchno = new_patchno;

            vt.regrid_interpolate2fine(new_domain,coarse_patch,fine_siblings,
                                      blockno,coarse_patchno,fine_patchno);
        }
        fclaw2d_patch_data_delete(old_domain,coarse_patch);
    }
    else if (newsize == FCLAW2D_PATCH_DOUBLESIZE)
    {
        if (domain_init)
        {
            fclaw_debugf("fclaw2d_regrid.cpp (repopulate): We shouldn't end up here\n");
            exit(0);
        }

        /* Old grids are the finer grids;  new grid is the coarsened grid */
        fclaw2d_patch_t *fine_siblings = old_patch;
        int fine_patchno = old_patchno;

        fclaw2d_patch_t *coarse_patch = new_patch;
        int coarse_patchno = new_patchno;
        fclaw2d_patch_data_new(new_domain,coarse_patch);

        /* Area (and possibly other things) should be averaged to coarse grid. */
        fclaw2d_clawpatch_build_from_fine(new_domain,fine_siblings,coarse_patch,
                                          blockno,coarse_patchno,fine_patchno,
                                          build_mode);

        /* Average the solution. Does this need to be customizable? */
        vt.regrid_average2coarse(new_domain,fine_siblings,coarse_patch,
                                blockno,coarse_patchno, fine_patchno);

        int i;
        for(i = 0; i < 4; i++)
        {
            fclaw2d_patch_t* fine_patch = &fine_siblings[i];
            fclaw2d_patch_data_delete(old_domain,fine_patch);
        }
    }
    else
    {
        fclaw_global_essentialf("cb_adapt_domain : newsize not recognized\n");
        exit(1);
    }
    fclaw2d_patch_neighbors_reset(new_patch);
}



/* ----------------------------------------------------------------
   Public interface
   -------------------------------------------------------------- */
void fclaw2d_regrid(fclaw2d_domain_t **domain)
{
    fclaw2d_domain_data_t* ddata = fclaw2d_domain_get_data(*domain);
    fclaw2d_timer_start (&ddata->timers[FCLAW2D_TIMER_REGRID]);
    fclaw2d_vtable_t vt = fclaw2d_get_vtable(*domain);

    fclaw_global_infof("Regridding domain\n");


    fclaw2d_timer_start (&ddata->timers[FCLAW2D_TIMER_REGRID_TAGGING]);
    /* First determine which families should be coarsened. */
    fclaw2d_domain_iterate_families(*domain, cb_regrid_tag4coarsening,
                                    (void*) NULL);

    int domain_init = 0;
    fclaw2d_domain_iterate_patches(*domain, cb_fclaw2d_regrid_tag4refinement,
                                   (void *) &domain_init);

    fclaw2d_timer_stop (&ddata->timers[FCLAW2D_TIMER_REGRID_TAGGING]);

    /* Rebuild domain if necessary */
    /* Will return be NULL if no refining was done */

    fclaw2d_timer_stop (&ddata->timers[FCLAW2D_TIMER_REGRID]);
    fclaw2d_timer_start (&ddata->timers[FCLAW2D_TIMER_ADAPT_COMM]);
    fclaw2d_domain_t *new_domain = fclaw2d_domain_adapt(*domain);

    fclaw_bool have_new_refinement = new_domain != NULL;

    if (have_new_refinement)
    {
        /* allocate memory for user patch data and user domain data in the new
           domain;  copy data from the old to new the domain. */
        fclaw2d_domain_setup(*domain,new_domain);
        ddata = fclaw2d_domain_get_data(new_domain);
    }

    /* Stop the new timer (copied from old timer) */
    fclaw2d_timer_stop (&ddata->timers[FCLAW2D_TIMER_ADAPT_COMM]);
    fclaw2d_timer_start (&ddata->timers[FCLAW2D_TIMER_REGRID]);

    if (have_new_refinement)
    {
        fclaw_global_infof(" -- Have new refinement\n");

        /* Average to new coarse grids and interpolate to new fine grids */
        fclaw2d_timer_start (&ddata->timers[FCLAW2D_TIMER_REGRID_BUILD]);
        fclaw2d_domain_iterate_adapted(*domain, new_domain,
                                       cb_fclaw2d_regrid_repopulate,
                                       (void *) &domain_init);
        fclaw2d_timer_stop (&ddata->timers[FCLAW2D_TIMER_REGRID_BUILD]);

        /* free memory associated with old domain */
        fclaw2d_domain_reset(domain);
        *domain = new_domain;
        new_domain = NULL;

        /* Repartition for load balancing.  Second arg (mode) for vtk output */
        fclaw2d_partition_domain(domain, -1,FCLAW2D_TIMER_REGRID);

        /* Set up ghost patches. Communication happens for indirect ghost exchanges. */

        /* Need a new timer */
        ddata = fclaw2d_domain_get_data(*domain);

        fclaw2d_exchange_setup(*domain,FCLAW2D_TIMER_REGRID);

        /* Get new neighbor information.  This is used to short circuit
           ghost filling procedures in some cases */
        fclaw2d_regrid_set_neighbor_types(*domain);

        /* Update ghost cells.  This is needed because we have new coarse or fine
           patches without valid ghost cells.   Time_interp = 0, since we only
           only regrid when all levels are time synchronized. */
        int minlevel = (*domain)->global_minlevel;
        int maxlevel = (*domain)->global_maxlevel;
        int time_interp = 0;
        double sync_time = fclaw2d_domain_get_time(*domain);
        fclaw2d_ghost_update(*domain,
                             minlevel,
                             maxlevel,
                             sync_time,
                             time_interp,
                             FCLAW2D_TIMER_REGRID);

        ++ddata->count_amr_new_domain;
    }
    else
    {
#if 0
        /* We updated all the ghost cells when leaving advance, so don't need to do
           it here */
        /* Set up ghost patches. No parallel communication is done here */
        fclaw2d_timer_start (&ddata->timers[FCLAW2D_TIMER_GHOSTPATCH_BUILD]);
        fclaw2d_exchange_setup(*domain);
        fclaw2d_timer_stop (&ddata->timers[FCLAW2D_TIMER_GHOSTPATCH_BUILD]);
#endif
    }

    if (vt.after_regrid != NULL)
    {
        vt.after_regrid(*domain);
    }

    /* Stop timer.  Be sure to use timers from new grid, if one was
       created */
    ddata = fclaw2d_domain_get_data(*domain);
    fclaw2d_timer_stop (&ddata->timers[FCLAW2D_TIMER_REGRID]);

    /* Count calls to this function */
    ++ddata->count_amr_regrid;
}

#ifdef __cplusplus
#if 0
{
#endif
}
#endif