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

#ifndef FILAMENT_USER_H
#define FILAMENT_USER_H

#include <fclaw2d_clawpatch.h>
#include <fc2d_clawpack46.h>


#ifdef __cplusplus
extern "C"
{
#if 0
}
#endif
#endif

#define SETAUX_MANIFOLD FCLAW_F77_FUNC(setaux_manifold,SETAUX_MANIFOLD)
void SETAUX_MANIFOLD(const int* mx, const int* my,const int* mbc,
                     const double* xlower, const double* ylower,
                     const double* dx, const double* dy,
                     const int* maux, double aux[],
                     const int* blockno,
                     double xd[], double yd[], double zd[],
                     double area[]);

#define SETAUX_NOMAP FCLAW_F77_FUNC(setaux_nomap,SETAUX_NOMAP)
void SETAUX(const int* maxmx, const int* maxmy,
            const int* mbc,const int* mx, const int* my,
            const double* xlower,const double* ylower,
            const double* dx,const double* dy,
            const int* maux, double aux[]);


#define TAG4REFINEMENT_DQ FCLAW_F77_FUNC(tag4refinement_dq,             \
                                         TAG4REFINEMENT_DQ)

void TAG4REFINEMENT_DQ(const int* mx,const int* my,
                       const int* mbc,const int* meqn,
                       const double* xlower, const double* ylower,
                       const double* dx, const double* dy,
                       const int* blockno,
                       double q[],
                       const double* tag_threshold,
                       const int* init_flag,
                       int* tag_patch);



#define TAG4COARSENING_DQ FCLAW_F77_FUNC(tag4coarsening_dq, \
                                         TAG4COARSENING_DQ)

void TAG4COARSENING_DQ(const int* mx, const int* my,
                       const int* mbc, const int* meqn,
                       const double* xlower, const double* ylower,
                       const double* dx, const double* dy,
                       const int* blockno,
                       double q0[],double q1[],
                       double q2[],double q3[],
                       const double* tag_threshold,
                       int* tag_patch);


fclaw2d_map_context_t* fclaw2d_map_new_nomap();

fclaw2d_map_context_t* fclaw2d_map_new_cart (fclaw2d_map_context_t* brick,
                                             const double scale[],
                                             const double shift[],
                                             const double rotate[]);

fclaw2d_map_context_t* fclaw2d_map_new_pillowdisk(const double scale[],
                                                  const double shift[],
                                                  const double rotate[]);

fclaw2d_map_context_t* fclaw2d_map_new_squareddisk(const double scale[],
                                                   const double shift[],
                                                   const double rotate[],
                                                   const double alpha);

fclaw2d_map_context_t* fclaw2d_map_new_pillowdisk5(const double scale[],
                                                   const double shift[],
                                                   const double rotate[],
                                                   const double alpha);

fclaw2d_map_context_t* fclaw2d_map_new_fivepatch(const double scale[],
                                                 const double shift[],
                                                 const double rotate[],
                                                 const double alpha);


void filament_link_solvers(fclaw2d_domain_t *domain);

void filament_patch_setup(fclaw2d_domain_t *domain,
                          fclaw2d_patch_t *this_patch,
                          int this_block_idx,
                          int this_patch_idx);


#ifdef __cplusplus
#if 0
{
#endif
}
#endif

#endif