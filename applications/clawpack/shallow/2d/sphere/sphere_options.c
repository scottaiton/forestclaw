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

static void *
options_register_user (fclaw_app_t * app, void *package, sc_options_t * opt)
{
    user_options_t* user = (user_options_t*) package;

    /* [user] User options */
    sc_options_add_int (opt, 0, "example", &user->example, 0,
                        "[user] 0,1 = cubedsphere; 2 = latlong [0]");

    /* [user] User options */
    sc_options_add_int (opt, 0, "mapping", &user->mapping, 1,
                        "[user] 1 = cubedsphere; 2 = latlong; 3=pillow [2]");

    /* [user] User options */
    sc_options_add_int (opt, 0, "initial_condition", &user->init_cond, 0,
                        "[user] Initial condition [0]");


    sc_options_add_double (opt, 0, "gravity", &user->gravity, 1.0, "[user] gravity [1.0]");
    sc_options_add_double (opt, 0, "hmax", &user->hmax, 1, "[user] hmax (Gaussian) [1.0]");
    sc_options_add_double (opt, 0, "amp", &user->amp, 5.0, "[user] a (Gaussian) [1.0]");

    sc_options_add_double (opt, 0, "r0", &user->r0, 1.0, "[user] r0 [0.2]");
    sc_options_add_double (opt, 0, "hin", &user->hin, 1.0, "[user] hin [2.0]");
    sc_options_add_double (opt, 0, "hout", &user->hout, 1.0, "[user] hout [1.0]");

    fclaw_options_add_double_array(opt, 0, "omega", &user->omega_string,
                                   "0 0 0", &user->omega, 3,
                                   "[user] Axis of rotation [0 0 0]");

    fclaw_options_add_double_array(opt, 0, "latitude", &user->latitude_string,
                                   "-50 50", &user->latitude, 2,
                                   "[user] Latitude range (degrees) [-50 50]");

    fclaw_options_add_double_array(opt, 0, "longitude", &user->longitude_string,
                                   "0 360", &user->longitude, 2,
                                   "[user] Longitude range (degrees) [0 360]");

    sc_options_add_int (opt, 0, "claw-version", &user->claw_version, 4,
                        "[user] Clawpack version (4 or 5) [4]");

    user->is_registered = 1;
    return NULL;
}

static fclaw_exit_type_t
options_postprocess_user (fclaw_app_t * a, void *package, void *registered)
{
    user_options_t* user = (user_options_t*) package;

    fclaw_options_convert_double_array (user->latitude_string, &user->latitude,2);
    fclaw_options_convert_double_array (user->longitude_string, &user->longitude,2);
    fclaw_options_convert_double_array (user->omega_string, &user->omega,3);
    return FCLAW_NOEXIT;
}

static fclaw_exit_type_t
options_check_user (fclaw_app_t * app, void *package, void *registered)
{
    user_options_t* user = (user_options_t*) package;
    if (user->example < 0 || user->example > 2) {
        fclaw_global_essentialf ("Option --user:example must be 0, 1, 2, 3 or 4\n");
        return FCLAW_EXIT_QUIET;
    }
    return FCLAW_NOEXIT;
}

static void
options_destroy_user (fclaw_app_t * a, void *package, void *registered)
{
    user_options_t* user = (user_options_t*) package;
    /* Destroy arrays used in options  */
    if (user->example == 0)
    {
        fclaw_options_destroy_array((void*) user->latitude);
        fclaw_options_destroy_array((void*) user->longitude);
        fclaw_options_destroy_array((void*) user->omega);
    }
}


static const
fclaw_app_options_vtable_t options_vtable_user =
{
    options_register_user,
    options_postprocess_user,
    options_check_user,
    options_destroy_user
};

#if 0
static
void register_user_options (fclaw_app_t * app,
                            const char *configfile,
                            user_options_t* user)
{
    FCLAW_ASSERT (app != NULL);

    /* sneaking the version string into the package pointer */
    /* when there are more parameters to pass, create a structure to pass */
    fclaw_app_options_register (app,"user", configfile, &options_vtable_user,
                                user);
}
#endif

/* ------------- User options access functions --------------------- */

user_options_t* sphere_options_register (fclaw_app_t * app,
                                          const char *configfile)
{
    user_options_t *user;
    FCLAW_ASSERT (app != NULL);

    user = FCLAW_ALLOC (user_options_t, 1);
    fclaw_app_options_register (app,"user", configfile, &options_vtable_user,
                                user);

    fclaw_app_set_attribute(app,"user",user);
    return user;
}

void sphere_options_store (fclaw2d_global_t* glob, user_options_t* user)
{
    fclaw2d_global_options_store(glob, "user", user);
}

user_options_t* sphere_get_options(fclaw2d_global_t* glob)
{
    return (user_options_t*) fclaw2d_global_get_options(glob, "user");
}

#if 0
void sphere_global_post_process(fclaw_options_t *fclaw_opt,
                                fclaw2d_clawpatch_options_t *clawpatch_opt,
                                user_options_t *user_opt)
{
    if (user_opt->example == 2)
        if (clawpatch_opt->mx*pow_int(2,fclaw_opt->minlevel) < 32)
        {
            fclaw_global_essentialf("The five patch mapping requires mx*2^minlevel >= 32\n");
            exit(0);
        }
}
#endif
