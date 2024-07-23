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

#include <fclaw_patch.h>

#include <fclaw_forestclaw.h>
#include <fclaw_global.h>
#include <fclaw_options.h>
#include <fclaw_advance.h>
#include <fclaw_regrid.h>
#include <fclaw_output.h>
#include <fclaw_diagnostics.h>
#include <fclaw_vtable.h>

#include "fclaw_math.h"
#include "user.h"

/*  -----------------------------------------------------------------
    Time stepping
    -- saving time steps
    -- restoring time steps
    -- Time stepping, based on when output files should be created.
    ----------------------------------------------------------------- */

static
void cb_restore_time_step(fclaw_domain_t *domain,
                          fclaw_patch_t *this_patch,
                          int this_block_idx,
                          int this_patch_idx,
                          void *user)
{
    fclaw_global_iterate_t* s = (fclaw_global_iterate_t*) user;
    fclaw_patch_restore_step(s->glob,this_patch);
}

static
void restore_time_step(fclaw_global_t *glob)
{
    fclaw_global_iterate_patches(glob,cb_restore_time_step,(void *) NULL);

    //fclaw_options_t *fopt = fclaw2d_get_options(glob);
    //fclaw2d_time_sync_reset(glob,fopt->minlevel,fopt->maxlevel,0);
}

static
void cb_save_time_step(fclaw_domain_t *domain,
                       fclaw_patch_t *this_patch,
                       int this_block_idx,
                       int this_patch_idx,
                       void *user)
{
    fclaw_global_iterate_t* s = (fclaw_global_iterate_t*) user;
    fclaw_patch_save_step(s->glob,this_patch);
}

static
void save_time_step(fclaw_global_t *glob)
{
    fclaw_global_iterate_patches(glob,cb_save_time_step,(void *) NULL);
}

typedef struct outstyle_1_context {
    int intialized;
    int init_flag;
    int iframe;
    double final_time;
    int nout;
    double initial_dt;
    int level_factor;
    double dt_minlevel;
    double t0;
    double dt_outer;
    double t_curr;
    int n_inner;
    int n;
    double tstart;
    double tend;
    double dt_step;
    double tol;
    int took_small_step;
    int took_big_step;
    double dt_step_desired;
    double maxcfl_step;
    double tc;
} outstyle_1_context_t;

/* -------------------------------------------------------------------------------
   Output style 1
   Output times are at times [0,dT, 2*dT, 3*dT,...,Tfinal], where dT = tfinal/nout
   -------------------------------------------------------------------------------- */
static
double outstyle_1(outstyle_1_context_t* ctx, double t_pause, fclaw_global_t *glob)
{
    fclaw_domain_t** domain = &glob->domain;
    const fclaw_options_t *fclaw_opt = fclaw_get_options(glob);

    if(ctx->intialized){
        goto AFTER_TIMESTEP;
    }
    ctx->intialized = 1;

    /* Set error to 0 */
    ctx->init_flag = 1;  /* Store anything that needs to be stored */
    fclaw_diagnostics_gather(glob,ctx->init_flag);
    ctx->init_flag = 0;

    ctx->iframe = 0;
    fclaw_output_frame(glob,ctx->iframe);


    ctx->final_time = fclaw_opt->tfinal;
    ctx->nout = fclaw_opt->nout;
    ctx->initial_dt = fclaw_opt->initial_dt;
    ctx->level_factor = pow_int(2,fclaw_opt->maxlevel - fclaw_opt->minlevel);
    ctx->dt_minlevel = ctx->initial_dt;

    ctx->t0 = 0;

    ctx->dt_outer = (ctx->final_time-ctx->t0)/((double) ctx->nout);
    ctx->t_curr = ctx->t0;
    ctx->n_inner = 0;

    for(ctx->n = 0; ctx->n < ctx->nout; ctx->n++)
    {
        ctx->tstart = ctx->t_curr;

        glob->curr_time = ctx->t_curr;
        ctx->tend = ctx->tstart + ctx->dt_outer;
        while (ctx->t_curr < ctx->tend)
        {
            /* In case we have to reject this step */
            if (!fclaw_opt->use_fixed_dt)
            {
                save_time_step(glob);
            }

            /* Use the tolerance to make sure we don't take a tiny time
               step just to hit 'tend'.   We will take a slightly larger
               time step now (dt_cfl + tol) rather than taking a time step
               of 'dt_minlevel' now, followed a time step of only 'tol' in
               the next step.  Of course if 'tend - t_curr > dt_minlevel',
               then dt_minlevel doesn't change. */

            ctx->dt_step = ctx->dt_minlevel;
            if (fclaw_opt->advance_one_step)
            {
                ctx->dt_step /= ctx->level_factor;
            }

            ctx->tol = 1e-2*ctx->dt_step;
            ctx->took_small_step = 0;
            ctx->took_big_step = 0;
            ctx->dt_step_desired = ctx->dt_step;
            if (!fclaw_opt->use_fixed_dt)
            {
                double small_step = ctx->tend-(ctx->t_curr+ctx->dt_step);
                if (small_step  < ctx->tol)
                {
                    ctx->dt_step = ctx->tend - ctx->t_curr;  // <= 'dt_minlevel + tol'
                    if (small_step < 0)
                    {
                        /* We have (tend-t_curr) < dt_minlevel, and
                           we have to take a small step to hit tend */
                        ctx->took_small_step = 1;
                    }
                    else
                    {
                        /* Take a bigger step now to avoid small step
                           in next time step. */
                        ctx->took_big_step = 1;
                    }
                }
            }
            glob->curr_dt = ctx->dt_step;  
            ctx->maxcfl_step = fclaw_advance_all_levels(glob, ctx->t_curr,ctx->dt_step);

            if (fclaw_opt->reduce_cfl)
            {
                /* If we are taking a variable time step, we have to reduce the 
                   maxcfl so that every processor takes the same size dt */
                fclaw_timer_start (&glob->timers[FCLAW_TIMER_CFL_COMM]);
                ctx->maxcfl_step = fclaw_domain_global_maximum (*domain, ctx->maxcfl_step);
                fclaw_timer_stop (&glob->timers[FCLAW_TIMER_CFL_COMM]);                
            }


            ctx->tc = ctx->t_curr + ctx->dt_step;
            fclaw_global_productionf("Level %d (%d-%d) step %5d : dt = %12.3e; maxcfl (step) = " \
                                     "%8.3f; Final time = %12.4f\n",
                                     fclaw_opt->minlevel,
                                     (*domain)->global_minlevel,
                                     (*domain)->global_maxlevel,
                                     ctx->n_inner+1,ctx->dt_step,
                                     ctx->maxcfl_step, ctx->tc);

            if ((ctx->maxcfl_step > fclaw_opt->max_cfl) & fclaw_opt->reduce_cfl)
            {
                fclaw_global_essentialf("   WARNING : Maximum CFL exceeded; "    \
                                        "retaking time step\n");

                if (!fclaw_opt->use_fixed_dt)
                {
                    restore_time_step(glob);

                    /* Modify dt_level0 from step used. */
                    ctx->dt_minlevel = ctx->dt_minlevel*fclaw_opt->desired_cfl/ctx->maxcfl_step;

                    /* Got back to start of loop, without incrementing
                       step counter or time level */
                    continue;
                }
            }

            // this can be an opprotunity to return
            if(ctx->tc >= t_pause){
                return ctx->tc;
            }
            AFTER_TIMESTEP:

            /* We are happy with this step */
            ctx->n_inner++;
            ctx->t_curr += ctx->dt_step;


            /* Update this step, if necessary */
            if (!fclaw_opt->use_fixed_dt)
            {
                double step_fraction = 100.0*ctx->dt_step/ctx->dt_step_desired;
                if (ctx->took_small_step)
                {
                    fclaw_global_infof("   WARNING : Took small time step which was " \
                                       "%6.1f%% of desired dt.\n",
                                       step_fraction);
                }
                if (ctx->took_big_step)
                {
                    fclaw_global_infof("   WARNING : Took big time step which was " \
                                       "%6.1f%% of desired dt.\n",step_fraction);

                }


                /* New time step, which should give a cfl close to the
                   desired cfl. */
                double dt_new = ctx->dt_minlevel*fclaw_opt->desired_cfl/ctx->maxcfl_step;
                if (!ctx->took_small_step)
                {
                    ctx->dt_minlevel = dt_new;
                }
                else
                {
                    /* use time step that would have been used had we
                       not taken a small step */
                }
            }
            glob->curr_time = ctx->t_curr;

            if (fclaw_opt->advance_one_step)
            {
                fclaw_diagnostics_gather(glob, ctx->init_flag);                
            }

            if (fclaw_opt->regrid_interval > 0)
            {
                if (ctx->n_inner % fclaw_opt->regrid_interval == 0)
                {
                    fclaw_global_infof("regridding at step %d\n",ctx->n);
                    fclaw_regrid(glob);
                }
            }
        }

        /* Output file at every outer loop iteration */
        fclaw_diagnostics_gather(glob, ctx->init_flag);
        glob->curr_time = ctx->t_curr;
        ctx->iframe++;
        fclaw_output_frame(glob,ctx->iframe);
    }
    return ctx->final_time;
}


/* ------------------------------------------------------------------
   Public interface
   ---------------------------------------------------------------- */

void user_run(fclaw_global_t * globs[],int nglobs)
{

    const fclaw_options_t* fclaw_opts[nglobs];
    outstyle_1_context_t   contexts[nglobs];

    int    finished[nglobs];
    double dt[nglobs];
    double tcurr[nglobs];

    for(int i=0; i< nglobs; i++){
        fclaw_opts[i] = fclaw_get_options(globs[i]);

        contexts[i].intialized = 0;

        finished[i] = 0;
        tcurr[i]    = 0;
        dt[i]       = fclaw_opts[i]->tfinal/10;
    }

    int all_finished = 0;
    int n = 1;
    while(!all_finished){
        for(int i=0; i< nglobs; i++){
            if(!finished[i]){
                fclaw_set_global_context(globs[i]);
                fclaw_problem_setup(globs[i]);

                tcurr[i] = outstyle_1(&contexts[i], n*dt[i], globs[i]);
                finished[i] = tcurr[i] >= fclaw_opts[i]->tfinal;
                fclaw_global_productionf("Paused at time %12.4f\n\n\n",tcurr[i]);

                fclaw_clear_global_context(globs[i]);
            }
        }
        n++;
        // this can be were solvers communicate with eatch other

        // check if all are finished
        all_finished = 1;
        for(int i=0; i< nglobs; i++){
            if(!finished[i]){
                all_finished = 0;
            }
        }
    }
}
