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

#ifndef FCLAW2D_REGRID_DEFAULT_H
#define FCLAW2D_REGRID_DEFAULT_H

#include <forestclaw2d.h>
#include <fclaw2d_regrid_default_fort.h>

#ifdef __cplusplus
extern "C"
{
#if 0
}
#endif
#endif

int fclaw2d_clawpatch_tag4refinement(fclaw2d_global_t *glob,
                                  fclaw2d_patch_t *this_patch,
                                  int blockno, int patchno,
                                  int initflag);

int fclaw2d_clawpatch_tag4coarsening(fclaw2d_global_t *glob,
                                  fclaw2d_patch_t *this_patch,
                                  int blockno, int patchno);

void fclaw2d_clawpatch_average2coarse(fclaw2d_domain_t *domain,
                                   fclaw2d_patch_t *fine_siblings,
                                   fclaw2d_patch_t *coarse_patch,
                                   int blockno, int fine_patchno,
                                   int coarse_patchno);

void fclaw2d_clawpatch_interpolate2fine(fclaw2d_domain_t* domain,
                                     fclaw2d_patch_t *coarse_patch,
                                     fclaw2d_patch_t* fine_patch,
                                     int this_blockno, int coarse_patchno,
                                     int fine_patchno);

#ifdef __cplusplus
#if 0
{
#endif
}
#endif

#endif
