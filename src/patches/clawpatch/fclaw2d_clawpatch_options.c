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

#include "fclaw_base.h"
#ifndef REFINE_DIM
#define REFINE_DIM 2
#endif

#ifndef PATCH_DIM
#define PATCH_DIM 2
#endif

#if REFINE_DIM == 2 && PATCH_DIM == 2

#define CLAWPATCH_OPTION_NAME "fclaw2d_clawpatch"

#include <fclaw2d_clawpatch_options.h>

#elif REFINE_DIM == 2 && PATCH_DIM == 3

#define CLAWPATCH_OPTION_NAME "fclaw3dx_clawpatch"

#include <fclaw3dx_clawpatch_options.h>
#include <_fclaw2d_to_fclaw3dx.h>

#endif
#include <fclaw2d_global.h>
#include <fclaw_pointer_map.h>
#include <fclaw_packing.h>

static sc_keyvalue_t *
kv_refinement_criterea_new()
{
    sc_keyvalue_t *kv = sc_keyvalue_new ();
    sc_keyvalue_set_int (kv, "value",        FCLAW_REFINE_CRITERIA_VALUE);
    sc_keyvalue_set_int (kv, "difference",   FCLAW_REFINE_CRITERIA_DIFFERENCE);
    sc_keyvalue_set_int (kv, "minmax",       FCLAW_REFINE_CRITERIA_MINMAX);
    sc_keyvalue_set_int (kv, "gradient",     FCLAW_REFINE_CRITERIA_GRADIENT);
    sc_keyvalue_set_int (kv, "user",         FCLAW_REFINE_CRITERIA_USER);

    return kv;
}

static void *
clawpatch_register(fclaw2d_clawpatch_options_t *clawpatch_options,
                   sc_options_t * opt)
{
    sc_options_add_int (opt, 0, "mx", &clawpatch_options->mx, 8,
                        "Number of grid cells per patch in x [8]");

    sc_options_add_int (opt, 0, "my", &clawpatch_options->my, 8,
                        "Number of grid cells per patch in y [8]");

#if PATCH_DIM == 3
    sc_options_add_int (opt, 0, "mz", &clawpatch_options->mz, 1,
                        "Number of grid cells per patch in z [1]");
#endif

    sc_options_add_int (opt, 0, "maux", &clawpatch_options->maux, 0,
                        "Number of auxilliary variables [0]");

    sc_options_add_int (opt, 0, "mbc", &clawpatch_options->mbc, 2,
                        "Number of ghost cells [2]");

    sc_options_add_int (opt, 0, "meqn", &clawpatch_options->meqn, 1,
                        "Number of equations [1]");

    sc_options_add_int (opt, 0, "rhs-fields", &clawpatch_options->rhs_fields, 1,
                        "Number of fields in rhs [1]");

    /* ---------------------- advanced options -------------------------- */
    sc_options_add_int (opt, 0, "interp_stencil_width",
                        &clawpatch_options->interp_stencil_width,
                        3, "Interpolation stencil width [3]");

    sc_options_add_bool (opt, 0, "ghost_patch_pack_aux", 
                         &clawpatch_options->ghost_patch_pack_aux,0,
                         "Pack aux. variables for parallel comm. of ghost patches [F]");

    sc_options_add_bool (opt, 0, "save-aux", 
                         &clawpatch_options->save_aux,0,
                         "Save aux variables when re-taking a time step [F]");

    /* Set verbosity level for reporting timing */
    sc_keyvalue_t *kv = clawpatch_options->kv_refinement_criteria = kv_refinement_criterea_new();
    sc_options_add_keyvalue (opt, 0, "refinement-criteria", 
                             &clawpatch_options->refinement_criteria, "minmax",
                             kv, "Refinement criteria [minmax]");

    sc_options_add_int (opt, 0, "threshold-variable",
                        &clawpatch_options->threshold_variable,
                        1, "Index of variable used for tagging in [1,meqn] [1]");


    clawpatch_options->is_registered = 1;

    return NULL;
}

static fclaw_exit_type_t
clawpatch_postprocess(fclaw2d_clawpatch_options_t *clawpatch_opt)
{
    /* Convert strings to arrays (no strings to process here) */
    return FCLAW_NOEXIT;
}

static fclaw_exit_type_t
clawpatch_check(fclaw2d_clawpatch_options_t *clawpatch_opt)
{
#if 0
    if (clawpatch_opt->mx != clawpatch_opt->my)
    {
        fclaw_global_essentialf("Clawpatch error : mx != my\n");
        return FCLAW_EXIT_ERROR;    
    }
#endif

    if (2*clawpatch_opt->mbc > clawpatch_opt->mx)
    {
        fclaw_global_essentialf("Clawpatch error : 2*mbc > mx or 2*mbc > my\n");
        return FCLAW_EXIT_ERROR;
    }

    if (clawpatch_opt->interp_stencil_width/2 > clawpatch_opt->mbc)
    {
        fclaw_global_essentialf("Interpolation width is too large for number of " \
                                "ghost cells (mbc) specified.  We should have " \
                                "(width)/2 <= mbc");
    }

    /* Don't check value, in case use wants to set something */
    if (clawpatch_opt->refinement_criteria < 0 || clawpatch_opt->refinement_criteria > 4)
    {
        fclaw_global_essentialf("Clawpatch error : Default refinement criteria " \
                                "must be one of 'value','difference','minmax', " \
                                "'gradient', or 'user'.\n");
        return FCLAW_EXIT_ERROR;            
    }

    return FCLAW_NOEXIT;
}

static void
clawpatch_destroy (fclaw2d_clawpatch_options_t *clawpatch_opt)
{
    FCLAW_ASSERT (clawpatch_opt->kv_refinement_criteria != NULL);
    sc_keyvalue_destroy (clawpatch_opt->kv_refinement_criteria);
    FCLAW_FREE(clawpatch_opt);
}

static void
clawpatch_destroy_void(void *clawpatch_opt)
{
    clawpatch_destroy ((fclaw2d_clawpatch_options_t *) clawpatch_opt);
}

static size_t 
options_packsize(void* user)
{
    return sizeof(fclaw2d_clawpatch_options_t);
}

static size_t 
options_pack(void* user, char* buffer)
{
    fclaw2d_clawpatch_options_t* opts = (fclaw2d_clawpatch_options_t*) user;

    //pack entire struct
    return FCLAW_PACK(*opts,buffer);
}

static size_t 
options_unpack(char* buffer, void** user)
{
    fclaw2d_clawpatch_options_t** opts_ptr = (fclaw2d_clawpatch_options_t**) user;

    *opts_ptr = FCLAW_ALLOC(fclaw2d_clawpatch_options_t,1);

    FCLAW_UNPACK(buffer,*opts_ptr);

    fclaw2d_clawpatch_options_t* opts = *opts_ptr;

    *opts = *(fclaw2d_clawpatch_options_t*) buffer;

    opts->kv_refinement_criteria = kv_refinement_criterea_new();

    return sizeof(fclaw2d_clawpatch_options_t);
}

static fclaw_packing_vtable_t packing_vt = 
{
	options_pack,
	options_unpack,
	options_packsize,
	clawpatch_destroy_void
};

const fclaw_packing_vtable_t* 
fclaw2d_clawpatch_options_get_packing_vtable()
{
    return &packing_vt;
}

/* ------------------------------------------------------------------------
  Generic functions - these call the functions above
  ------------------------------------------------------------------------ */

static void *
options_register(fclaw_app_t * a, void *optpkg, sc_options_t * opt)
{
    FCLAW_ASSERT (a != NULL);
    FCLAW_ASSERT (optpkg != NULL);
    FCLAW_ASSERT (opt != NULL);

    fclaw2d_clawpatch_options_t *clawpatch_opt = 
                                 (fclaw2d_clawpatch_options_t *) optpkg;

    fclaw_app_register_options_packing_vtable(CLAWPATCH_OPTION_NAME, &packing_vt);

    return clawpatch_register(clawpatch_opt,opt);
}

static fclaw_exit_type_t
options_postprocess(fclaw_app_t * a, void *optpkg, void *registered)
{
    FCLAW_ASSERT (a != NULL);
    FCLAW_ASSERT (optpkg != NULL);
    FCLAW_ASSERT (registered == NULL);

    fclaw2d_clawpatch_options_t *clawpatch_opt = 
                                  (fclaw2d_clawpatch_options_t *) optpkg;

    FCLAW_ASSERT (clawpatch_opt->is_registered);

    return clawpatch_postprocess(clawpatch_opt);
}

static fclaw_exit_type_t
options_check (fclaw_app_t * app, void *package, void *registered)
{
    FCLAW_ASSERT (app != NULL);
    FCLAW_ASSERT (package != NULL);
    FCLAW_ASSERT (registered == NULL);

    fclaw2d_clawpatch_options_t *clawpatch_opt = 
                                (fclaw2d_clawpatch_options_t *) package;

    FCLAW_ASSERT(clawpatch_opt->is_registered);

    return clawpatch_check (clawpatch_opt);
}

static void
options_destroy (fclaw_app_t * a, void *package, void *registered)
{
    FCLAW_ASSERT (a != NULL);
    FCLAW_ASSERT (package != NULL);
    FCLAW_ASSERT (registered == NULL);

    fclaw2d_clawpatch_options_t *clawpatch_opt = 
                                 (fclaw2d_clawpatch_options_t*) package;

    FCLAW_ASSERT (clawpatch_opt->is_registered);

    /* Destroy option arrays created in post-process */
    clawpatch_destroy (clawpatch_opt);
}


static
const fclaw_app_options_vtable_t fclaw2d_clawpatch_options_vtable = {
    options_register,
    options_postprocess,
    options_check,
    options_destroy
};


/* ---------------------------------------------------------
   Public interface to clawpatch options
   --------------------------------------------------------- */

fclaw2d_clawpatch_options_t *
fclaw2d_clawpatch_options_register(fclaw_app_t* app, const char* name, const char* configfile)
{
    fclaw2d_clawpatch_options_t* clawpatch_options;

    FCLAW_ASSERT (app != NULL);

    /* allocate storage for fclaw_options */
    clawpatch_options = FCLAW_ALLOC(fclaw2d_clawpatch_options_t,1);

    fclaw_app_options_register (app,
                                name,
                                configfile,
                                &fclaw2d_clawpatch_options_vtable,
                                clawpatch_options);

    fclaw_app_set_attribute(app, name, clawpatch_options);
    return clawpatch_options;
}

void 
fclaw2d_clawpatch_options_store (fclaw2d_global_t *glob, 
                                 fclaw2d_clawpatch_options_t* clawpatch_options)
{
    fclaw2d_global_options_store(glob, CLAWPATCH_OPTION_NAME, clawpatch_options);
}

fclaw2d_clawpatch_options_t* 
fclaw2d_clawpatch_get_options(fclaw2d_global_t* glob)
{
    return (fclaw2d_clawpatch_options_t*) 
            fclaw2d_global_get_options(glob, CLAWPATCH_OPTION_NAME);
}
