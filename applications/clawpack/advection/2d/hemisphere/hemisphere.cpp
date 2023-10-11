/*
Copyright (c) 2012-2023 Carsten Burstedde, Donna Calhoun, Scott Aiton
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

#include "hemisphere_user.h"

/* ------------- Create the domain --------------------- */
static
void create_domain(fclaw_global_t *glob)

{
    /* Used locally */
    const fclaw_options_t* fclaw_opt = fclaw_get_options(glob);
    double pi = M_PI;
    double rotate[2];
    rotate[0] = pi*fclaw_opt->theta/180.0;
    rotate[1] = 0;

#if 0
    const fclaw2d_clawpatch_options_t *clawpatch_opt = 
           fclaw2d_clawpatch_get_options(glob);

    int mi = fclaw_opt->mi;
    int mx = clawpatch_opt->mx;
    int minlevel = fclaw_opt->minlevel;
#endif    

    /* Mapped, multi-block domain */
    fclaw_domain_t *domain;
    fclaw2d_map_context_t *cont = NULL;

    user_options_t *user_opt = (user_options_t*) hemisphere_get_options(glob);
    switch (user_opt->example) {
    case 0:
#if 0    
        /* We should really be checking this - to avoid problems with ghost cells at 
           the triple points in the mapping 
        */
        if (mi*mx*pow_int(2,minlevel) < 32)
        {
            fclaw_global_essentialf("The five patch mapping requires mi*mx*2^minlevel > 32\n");
            exit(0);

        }
#endif        
        /* Five patch square domain */
        domain =
            fclaw_domain_new_2d_disk(glob->mpicomm, 0, 0,
                                    fclaw_opt->minlevel);
        cont =
            fclaw2d_map_new_pillowsphere5(fclaw_opt->scale, 
                                          rotate,
                                          user_opt->alpha);
        break;

    case 1:
        /* Map unit square to disk using mapc2m_disk.f */
        /* Map unit square to the pillow disk using mapc2m_pillowdisk.f */
        domain =
            fclaw_domain_new_unitsquare (glob->mpicomm,
                                           fclaw_opt->minlevel);

        cont = fclaw2d_map_new_pillowsphere(fclaw_opt->scale,
                                            rotate);
        break;
    default:
        SC_ABORT_NOT_REACHED (); /* must be checked in torus_checkparms */
    }


    /* Store mapping in the glob */
    fclaw2d_map_store (glob, cont);            

    /* Store the domain in the glob */
    fclaw_global_store_domain(glob, domain);

    /* print out some info */
    fclaw_domain_list_levels(domain, FCLAW_VERBOSITY_ESSENTIAL);
    fclaw_domain_list_neighbors(domain, FCLAW_VERBOSITY_DEBUG);  
}

static
void run_program(fclaw_global_t* glob)
{
    /* ---------------------------------------------------------------
       Set domain data.
       --------------------------------------------------------------- */
    /* Initialize virtual table for ForestClaw */
    fclaw_vtables_initialize(glob);

    /* Initialize virtual tables for solvers */
    const user_options_t *user_opt = hemisphere_get_options(glob);
    if (user_opt->claw_version == 4)
        fc2d_clawpack46_solver_initialize(glob);
    else if (user_opt->claw_version == 5)
        fc2d_clawpack5_solver_initialize(glob);

    hemisphere_link_solvers(glob);

    /* ---------------------------------------------------------------
       Run
       --------------------------------------------------------------- */
    fclaw_initialize(glob);
    fclaw_run(glob);
    fclaw_finalize(glob);
}

int
main (int argc, char **argv)
{
    /* Initialize application */
    fclaw_app_t *app = fclaw_app_new (&argc, &argv, NULL);

    /* Options */
    user_options_t              *user_opt;
    fclaw_options_t             *fclaw_opt;
    fclaw_clawpatch_options_t *clawpatch_opt;
    fc2d_clawpack46_options_t   *claw46_opt;
    fc2d_clawpack5_options_t    *claw5_opt;

    /* Create new options packages */
    fclaw_opt =                   fclaw_options_register(app,  NULL,        "fclaw_options.ini");
    clawpatch_opt =   fclaw_clawpatch_2d_options_register(app, "clawpatch",  "fclaw_options.ini");
    claw46_opt =        fc2d_clawpack46_options_register(app, "clawpack46", "fclaw_options.ini");
    claw5_opt =          fc2d_clawpack5_options_register(app, "clawpack5",  "fclaw_options.ini");
    user_opt =               hemisphere_options_register(app,               "fclaw_options.ini");  

    /* Read configuration file(s) and command line, and process options */
    int first_arg;
    fclaw_exit_type_t vexit = 
           fclaw_app_options_parse (app, &first_arg,"fclaw_options.ini.used");

    if (!vexit)
    {

        /* Options have been checked and are valid */
        /* Create glob */
        int size, rank;
        sc_MPI_Comm mpicomm = fclaw_app_get_mpi_size_rank (app, &size, &rank);
        fclaw_global_t *glob = fclaw_global_new_comm (mpicomm, size, rank);

        /* Store option packages in glob */
        fclaw_options_store           (glob, fclaw_opt);
        fclaw_clawpatch_options_store (glob, clawpatch_opt);
        fc2d_clawpack46_options_store   (glob, claw46_opt);
        fc2d_clawpack5_options_store    (glob, claw5_opt);
        hemisphere_options_store        (glob, user_opt);

        create_domain(glob);

        /* Run the program */
        run_program(glob);

        fclaw_global_destroy(glob);
    }
    
    fclaw_app_destroy (app);

    return 0;
}


