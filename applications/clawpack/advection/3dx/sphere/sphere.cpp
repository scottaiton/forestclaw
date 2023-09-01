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

#include "sphere_user.h"

static
fclaw_domain_t* create_domain(sc_MPI_Comm mpicomm, 
                                fclaw_options_t* fclaw_opt,
                                user_options_t* user_opt,
                               fclaw_clawpatch_options_t *claw_opt)
{
    /* Mapped, multi-block domain */
    p4est_connectivity_t     *conn = NULL;
    fclaw_domain_t         *domain;
    fclaw2d_map_context_t    *cont = NULL;

    FCLAW_ASSERT(fclaw_opt->manifold != 0);
    FCLAW_ASSERT(claw_opt->maux == 4);


    /* Used locally */
    double pi = M_PI;
    double rotate[2];

    rotate[0] = pi*fclaw_opt->theta/180.0;
    rotate[1] = pi*fclaw_opt->phi/180.0;

    switch (user_opt->example) {
    case 1:
        conn = p4est_connectivity_new_cubed();
        cont = fclaw2d_map_new_cubedsphere(fclaw_opt->scale,
                                           rotate);
        break;
    case 2:
        conn = p4est_connectivity_new_pillow();
        cont = fclaw2d_map_new_pillowsphere(fclaw_opt->scale,
                                            rotate);
        break;
    default:
        SC_ABORT_NOT_REACHED (); /* must be checked in torus_checkparms */
    }

    sphere_map_extrude(cont,user_opt->maxelev);

    domain = fclaw2d_domain_new_conn_map (mpicomm, fclaw_opt->minlevel, conn, cont);
    fclaw_domain_list_levels(domain, FCLAW_VERBOSITY_ESSENTIAL);
    fclaw_domain_list_neighbors(domain, FCLAW_VERBOSITY_DEBUG);  
    return domain;
}

static
void run_program(fclaw_global_t* glob)
{
    const user_options_t           *user_opt;

    /* ---------------------------------------------------------------
       Set domain data.
       --------------------------------------------------------------- */
    fclaw_domain_data_new(glob->domain);

    user_opt = sphere_get_options(glob);

    /* Initialize virtual table for ForestClaw */
    fclaw_vtables_initialize(glob);

    /* Initialize virtual tables for solvers */
    if (user_opt->claw_version == 4)
    {
        fc3d_clawpack46_solver_initialize(glob);
    }
    else if (user_opt->claw_version == 5)
    {
        printf("sphere.cpp : 3d example not implemented for Claw version 5.\n");
        exit(0);
    }

    sphere_link_solvers(glob);

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
    fclaw_app_t *app;
    int first_arg;
    fclaw_exit_type_t vexit;

    /* Options */
    user_options_t              *user_opt;
    fclaw_options_t             *fclaw_opt;
   fclaw_clawpatch_options_t *clawpatch_opt;
    fc3d_clawpack46_options_t   *claw46_opt;

    fclaw_global_t            *glob;
    fclaw_domain_t            *domain;
    sc_MPI_Comm mpicomm;

    /* Initialize application */
    app = fclaw_app_new (&argc, &argv, NULL);

    /* Create new options packages */
    fclaw_opt =                   fclaw_options_register(app,NULL, "fclaw_options.ini");
    clawpatch_opt =   fclaw_clawpatch_options_register_3d(app,"clawpatch", "fclaw_options.ini");
    claw46_opt =        fc3d_clawpack46_options_register(app,"claw3", "fclaw_options.ini");
    user_opt =                   sphere_options_register(app,"fclaw_options.ini");  

    /* Read configuration file(s) and command line, and process options */
    vexit =  fclaw_app_options_parse (app, &first_arg,"fclaw_options.ini.used");

    if (!vexit)
    {
        /* Options have been checked and are valid */

        mpicomm = fclaw_app_get_mpi_size_rank (app, NULL, NULL);
        domain = create_domain(mpicomm, fclaw_opt, user_opt,clawpatch_opt);
    
        /* Create global structure which stores the domain, timers, etc */
        glob = fclaw_global_new();
        fclaw_global_store_domain(glob, domain);

        /* Store option packages in glob */
        fclaw_options_store           (glob, fclaw_opt);
        fclaw_clawpatch_options_store (glob, clawpatch_opt);
        fc3d_clawpack46_options_store   (glob, claw46_opt);
        sphere_options_store            (glob, user_opt);

        /* Run the program */        
        run_program(glob);
        
        fclaw_global_destroy(glob);
    }
    
    fclaw_app_destroy (app);

    return 0;
}

