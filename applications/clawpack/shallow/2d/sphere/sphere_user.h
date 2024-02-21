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
#include <fclaw_global.h>

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

    // Ring
    double ring_inner;
    double ring_outer;

    int ring_units; /**< ring units : degrees, radians, meters */
    sc_keyvalue_t *kv_ring_units; /**< The refinement criteria */

    const char* latitude_string;
    double *latitude;

    const char* longitude_string;
    double *longitude;

    int claw_version;

    int is_registered;

} user_options_t;


user_options_t* sphere_options_register (fclaw_app_t * app,
                                          const char *configfile);

void sphere_options_store (fclaw_global_t* glob, user_options_t* user);
user_options_t* sphere_get_options(fclaw_global_t* glob);

void sphere_link_solvers(fclaw_global_t *glob);


#define SPHERE_SETAUX FCLAW_F77_FUNC(sphere_setaux,SPHERE_SETAUX)
void SPHERE_SETAUX(const int* mx, const int* my,const int* mbc,
                   const double* xlower, const double* ylower,
                   const double* dx, const double* dy,
                   double area[],
                   double xnormals[], double ynormals[],
                   double xtangents[], double ytangents[],
                   double surnormals[],double edgelengths[],
                   double curvature[],
                   double aux[],int* maux);


#define RPN2CONS_UPDATE FCLAW_F77_FUNC(rpn2cons_update, \
                                       RPN2CONS_UPDATE)

void RPN2CONS_UPDATE(const int* meqn, const int* maux, 
                     const int* idir, const int* iface,
                     double q[], double aux_center[], 
                     double aux_edge[], double flux[]);



#define USER_EXCEEDS_THRESHOLD \
                  FCLAW_F77_FUNC(user_exceeds_threshold, \
                                  USER_EXCEEDS_THRESHOLD)
int USER_EXCEEDS_THRESHOLD(const int *blockno,
                                        const int* meqn, 
                                        const double *qval, 
                                        const double *qmin, 
                                        const double *qmax,
                                        const double quad[], 
                                        const double *dx, 
                                        const double *dy, 
                                        const double *xc, 
                                        const double *yc, 
                                        const int* ivar_threshold,
                                        const double *tag_threshold,
                                        const int *init_flag,
                                        const int *is_ghost);


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

#if 0
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
