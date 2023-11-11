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

#ifndef SPHERE_USER_H
#define SPHERE_USER_H

#include <fclaw_clawpatch.h>

#ifdef __cplusplus
extern "C"
{
#endif

#if 0
/* For syntax highlighting */
#endif

typedef struct user_options
{
    int example;
    double gravity;
    int mapping;

    int init_cond;

    // In and out values
    double hin;
    double hout;


    //const char* omega_string;
    //double *omega;
    
    // Gaussian (initial condition 0)
    double amp;

    // Disk
    double disk_diameter;

    // Ring
    double ring_inner;
    double ring_outer;


    const char* latitude_string;
    double *latitude;

    const char* longitude_string;
    double *longitude;

    int claw_version;

    int is_registered;

} user_options_t;


user_options_t* sphere_options_register (fclaw_app_t * app,
                                          const char *configfile);

void sphere_options_store (fclaw2d_global_t* glob, user_options_t* user);
user_options_t* sphere_get_options(fclaw2d_global_t* glob);


#define SPHERE_SETAUX FCLAW_F77_FUNC(sphere_setaux,SPHERE_SETAUX)
void SPHERE_SETAUX(const int* mx, const int* my,const int* mbc,
                   const double* xlower, const double* ylower,
                   const double* dx, const double* dy,
                   double area[],
                   double xnormals[], double ynormals[],
                   double xtangents[], double ytangents[],
                   double surnormals[],double edgelengths[],
                   double aux[],int* maux);

void sphere_link_solvers(fclaw_domain_t *domain);

void sphere_patch_manifold_setup(fclaw_domain_t *domain,
                                  fclaw_patch_t *this_patch,
                                  int this_block_idx,
                                  int this_patch_idx);

double sphere_patch_update(fclaw_domain_t *domain,
                            fclaw_patch_t *this_patch,
                            int this_block_idx,
                            int this_patch_idx,
                            double t,
                            double dt);

fclaw_map_context_t *
    fclaw2d_map_new_latlong (fclaw_map_context_t* brick,
                             const double scale[],
                             const double lat[],
                             const double longitude[],
                             const int a, const int b);

fclaw_map_context_t * fclaw2d_map_new_cubedsphere (const double scale[],
                                                     const double shift[],
                                                     const double rotate[]);
#endif



#ifdef __cplusplus
}
#endif

#endif
