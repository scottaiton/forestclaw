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

#ifndef FCLAW_PACKAGE_H
#define FCLAW_PACKAGE_H

#include <fclaw_base.h>

#ifdef __cplusplus
extern "C"
{
#if 0
}
#endif
#endif

struct fclaw2d_global;

/* Opaque pointers */
typedef struct fclaw_package_container fclaw_package_container_t;
typedef struct fclaw_package           fclaw_package_t;

fclaw_package_container_t *fclaw_package_container_new (void);
void fclaw_package_container_destroy (fclaw_package_container_t * pkgs);
int fclaw_package_container_add (fclaw_package_container_t * pkg_container,
                                 void *opt);

/*********************** CODE BELOW STILL USING APP ********************/

/* Create, destroy and add packages */
void fclaw_package_container_new_app (fclaw_app_t *app);
void fclaw_package_container_destroy_app (fclaw_app_t *app);

#define FCLAW_PACKAGE_MESSAGE "\n\n\
    WARNING: The way of storing options has changed. Use fclaw2d_global_options_store and fclaw2d_global_get_options.  \n\
\n\
    Update the options_store and get_options functions in your application:\n\
\n\
        void swirl_options_store (fclaw2d_global_t* glob, user_options_t* user)\n\
        {\n\
            FCLAW_ASSERT(s_user_options_package_id == -1);\n\
            int id = fclaw_package_container_add_pkg(glob,user);\n\
            s_user_options_package_id = id;\n\
        }\n\
\n\
        const user_options_t* swirl_get_options(fclaw2d_global_t* glob)\n\
        {\n\
            int id = s_user_options_package_id;\n\
            return (user_options_t*) \n\
                    fclaw_package_get_options(glob, id);\n\
        }\n\
\n\
    with the updated calls:\n\
\n\
        void swirl_options_store (fclaw2d_global_t* glob, user_options_t* user)\n\
        {\n\
            fclaw2d_global_options_store(glob, \"user\", user);\n\
        }\n\
\n\
        const user_options_t* swirl_get_options(fclaw2d_global_t* glob)\n\
        {\n\
            return (user_options_t*) fclaw2d_global_get_options(glob, \"user\");\n\
        }\n\
\n\
    "

int fclaw_package_container_add_pkg(struct fclaw2d_global* glob,
                                    void* opt) 
FCLAW_DEPRECATED_MESSAGE(FCLAW_PACKAGE_MESSAGE);

void* fclaw_package_get_options(struct fclaw2d_global *glob, int id)
FCLAW_DEPRECATED_MESSAGE(FCLAW_PACKAGE_MESSAGE);


#ifdef __cplusplus
#if 0
{
#endif
}
#endif

#endif
