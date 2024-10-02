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

#include "periodic_user.h"

static
void periodic_problem_setup(fclaw_global_t* glob)
{
    const user_options_t* user = periodic_get_options(glob);
    const fclaw_options_t *fclaw_opt = fclaw_get_options(glob);

    if (glob->mpirank == 0)
    {
        FILE *f = fopen("setprob.data","w");
        fprintf(f,"%-24.4f %s\n",user->uvel,"\% u-velocity");
        fprintf(f,"%-24.4f %s\n",user->vvel,"\% v-velocity");
        fprintf(f,"%-24.4f %s\n",user->wvel,"\% w-velocity");

        /* Used in Matlab plotting */
        fprintf(f,"%-24d %s\n",fclaw_opt->use_regions,"\% use_regions");
        fclose(f);
    }

    /* Make sure that node 0 has written 'setprob.data' before proceeding */
    fclaw_domain_barrier(glob->domain);

    SETPROB();  /* Reads file created above and sets common block variables*/
}


void periodic_link_solvers(fclaw_global_t *glob)
{
    fclaw_vtable_t *vt = fclaw_vt(glob);
    vt->problem_setup = &periodic_problem_setup;  /* Version-independent */

    const user_options_t* user = periodic_get_options(glob);
    if (user->claw_version == 4)
    {
        fc3d_clawpack46_vtable_t *clawpack46_vt = fc3d_clawpack46_vt(glob);        

        clawpack46_vt->fort_qinit     = &CLAWPACK46_QINIT;
        clawpack46_vt->fort_rpn3      = &CLAWPACK46_RPN3;
        clawpack46_vt->fort_rpt3      = &CLAWPACK46_RPT3;
        clawpack46_vt->fort_rptt3      = &CLAWPACK46_RPTT3;
    }
    else if (user->claw_version == 5)
    {
        fclaw_global_essentialf("periodic_user: Clawpack 5.x layout not implemented in 3d.");
        exit(0);
    }
}






