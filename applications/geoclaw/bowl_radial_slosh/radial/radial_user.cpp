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

#include "radial_user.h"
#include "fclaw_global.h"

#include <fclaw_include_all.h>

#include <fclaw_clawpatch.h>
#include <fc2d_geoclaw.h>

#include <fclaw_filesystem.h>

void radial_link_solvers(fclaw_global_t *glob)
{

	/* User customizations ... */

}

fclaw_domain_t* radial_create_domain(sc_MPI_Comm mpicomm, 
                                       fclaw_options_t* gparms)
{
    p4est_connectivity_t     *conn = NULL;
    fclaw_domain_t         *domain;
    fclaw2d_map_context_t    *cont = NULL;

    /* Size is set by [ax,bx] x [ay, by], set in .ini file */
    conn = p4est_connectivity_new_unitsquare();
    cont = fclaw2d_map_new_nomap();

    domain = fclaw_domain_wrap_2d(fclaw2d_domain_new_conn_map (mpicomm, gparms->minlevel, conn, cont));
    fclaw_domain_list_levels(domain, FCLAW_VERBOSITY_ESSENTIAL);
    fclaw_domain_list_neighbors(domain, FCLAW_VERBOSITY_DEBUG);

    return domain;
}

void radial_run_program(fclaw_global_t* glob)
{
    fclaw_set_global_context(glob);

    fclaw_domain_t    **domain = &glob->domain;

    fclaw_vtables_initialize(glob);

    fc2d_geoclaw_solver_initialize(glob);

    radial_link_solvers(glob);

    /* ---------------------------------------------------------------
       Run
       --------------------------------------------------------------- */
	// change cwd to subdirectory
    

    fc2d_geoclaw_module_setup(glob);


    fclaw_initialize(glob);
    fc2d_geoclaw_run(glob);
    fclaw_finalize(glob);

    fclaw_clear_global_context(glob);
}