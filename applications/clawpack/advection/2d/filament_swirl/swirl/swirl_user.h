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

#ifndef SWIRL_USER_H
#define SWIRL_USER_H

#include "../../all/advection_user.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct swirl_options
{
    double period;
    int claw_version;
    int is_registered;

} swirl_options_t;

void swirl_link_solvers(fclaw_global_t *glob);

/* ------------------------------------- Options ---------------------------------------*/
swirl_options_t* swirl_options_register (fclaw_app_t * app,
                                        const char *section,
                                        const char *configfile);

void swirl_options_store (fclaw_global_t* glob, swirl_options_t* user);

const swirl_options_t* swirl_get_options(fclaw_global_t* glob);

/* Swirl */
void swirl_create_domain(fclaw_global_t *glob);
void swirl_initialize(fclaw_global_t* glob);
void swirl_finalize(fclaw_global_t* glob);


#ifdef __cplusplus
}
#endif

#endif
