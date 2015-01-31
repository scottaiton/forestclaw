/*
Copyright (c) 2012 Carsten Burstedde, Donna Calhoun
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

#ifndef CLAWPACK46_OPTIONS_H
#define CLAWPACK46_OPTIONS_H

#include <fclaw_options.h>

#ifdef __cplusplus
extern "C"
{
#if 0
}
#endif
#endif

typedef struct clawpack46_options clawpack46_options_t;

/* Only one copy of clawpack46_options for each run */
struct clawpack46_options
{
    int mwaves;
    int maux;

    int claw_verbosity;

    const char *order_string;
    int *order;   /* order[2] */

    int *mthlim;  /* mthlim[mwaves] */
    const char *mthlim_string;

    int method[7];  /* method[7] */
    int mcapa;
    int src_term;

    int is_registered;
};

void clawpack46_app_options_register (fclaw_app_t * app,
                                      const char *configfile,
                                      clawpack46_options_t* clawopt);



#ifdef __cplusplus
#if 0
{
#endif
}
#endif


#endif
