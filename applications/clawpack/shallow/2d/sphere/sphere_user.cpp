/*
Copyright (c) 2012-2023 Carsten Burstedde, Donna Calhoun
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
#include <fclaw_forestclaw.h>

#include <fclaw2d_clawpatch_pillow.h>

#if 0
// Fix syntax highlighting
#endif


static
void sphere_problem_setup(fclaw2d_global_t* glob)
{
    const user_options_t* user = sphere_get_options(glob);

    if (glob->mpirank == 0)
    {
        FILE *f = fopen("setprob.data","w");
        fprintf(f,  "%-24d   %s",user->example,"\% example\n");
        fprintf(f,  "%-24.16f   %s",user->gravity,"\% gravity\n");
        fprintf(f,  "%-24d   %s",user->mapping,"\% mapping\n");
        fprintf(f,  "%-24d   %s",user->init_cond,"\% initial_condition\n");        
        fprintf(f,  "%-24.16f   %s",user->amp,"\% amp\n");

        fprintf(f,  "%-24.16f   %s",user->hin,"\% hin\n");
        fprintf(f,  "%-24.16f   %s",user->hout,"\% hout\n");

        fprintf(f,  "%-24.16f   %s",user->ring_inner,"\% ring-inner\n");
        fprintf(f,  "%-24.16f   %s",user->ring_outer,"\% ring-outer\n");

        fprintf(f,  "%-24.16f   %s",user->latitude[0],"\% latitude\n");
        fprintf(f,  "%-24.16f   %s",user->latitude[1],"\% latitude\n");
        fprintf(f,  "%-24.16f   %s",user->longitude[0],"\% longitude\n");
        fprintf(f,  "%-24.16f   %s",user->longitude[1],"\% longitude\n");
        fclose(f);
    }
    fclaw2d_domain_barrier (glob->domain);
    SETPROB();
}

static
void sphere_patch_setup_manifold(fclaw2d_global_t *glob,
                                 fclaw2d_patch_t *patch,
                                 int blockno,
                                 int patchno)
{
    //const user_options_t *user = sphere_get_options(glob);

    int mx, my, mbc;
    double xlower, ylower, dx,dy;
    fclaw2d_clawpatch_grid_data(glob,patch,&mx,&my,&mbc,
                                &xlower,&ylower,&dx,&dy);

    int maux;
    double *aux;
    fclaw2d_clawpatch_aux_data(glob,patch,&aux,&maux);

    double *xd,*yd,*zd,*area;
    double *xp,*yp,*zp;
    fclaw2d_clawpatch_metric_data(glob,patch,&xp,&yp,&zp,
                                  &xd,&yd,&zd,&area);

    double *xnormals, *ynormals,*xtangents,*ytangents;
    double *surfnormals, *edgelengths, *curvature;
    fclaw2d_clawpatch_metric_data2(glob,patch,
                                   &xnormals,&ynormals,
                                   &xtangents,&ytangents,
                                   &surfnormals,
                                   &edgelengths,&curvature);

    SPHERE_SETAUX(&mx,&my,&mbc,&xlower,&ylower,
                  &dx,&dy,area,xnormals,ynormals,
                  xtangents,ytangents,surfnormals, curvature,
                  edgelengths,
                  aux, &maux);

}


void sphere_link_solvers(fclaw2d_global_t *glob)
{
    /* ForestClaw core functions */
    fclaw2d_vtable_t *vt = fclaw2d_vt(glob);
    vt->problem_setup = &sphere_problem_setup;  /* Version-independent */

    fclaw2d_patch_vtable_t *patch_vt = fclaw2d_patch_vt(glob);
    patch_vt->setup   = &sphere_patch_setup_manifold;

    const user_options_t* user_opt = sphere_get_options(glob);
    if (user_opt->mapping == 1)
        fclaw2d_clawpatch_use_pillowsphere(glob);

    if (user_opt->claw_version == 4)
    {
        fc2d_clawpack46_vtable_t  *clawpack46_vt = fc2d_clawpack46_vt(glob);
        // clawpack46_vt->b4step2        = sphere_b4step2;
        clawpack46_vt->fort_qinit     = CLAWPACK46_QINIT;
        clawpack46_vt->fort_rpn2    = &CLAWPACK46_RPN2;
        clawpack46_vt->fort_rpt2    = &CLAWPACK46_RPT2;
        // clawpack46_vt->fort_rpn2_cons = &RPN2CONS_UPDATE_MANIFOLD;

        /* Clawpatch functions */    
        // fclaw2d_clawpatch_vtable_t *clawpatch_vt = fclaw2d_clawpatch_vt(glob);

    }
    else
    {
        printf("Clawpack 5 not yet implemented\n");
    }
 }
