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

#ifndef FCLAW3D_DIAGNOSTICS_H
#define FCLAW3D_DIAGNOSTICS_H

#ifdef __cplusplus
extern "C"
{
#endif

struct fclaw3d_global;
struct fclaw3d_domain;

typedef struct fclaw3d_diagnostics_vtable  fclaw3d_diagnostics_vtable_t;

typedef struct fclaw3d_diagnostics_accumulator fclaw3d_diagnostics_accumulator_t;

struct fclaw3d_diagnostics_accumulator
{
    void* patch_accumulator;
    void* solver_accumulator;
    void* user_accumulator;
    void* gauge_accumulator;
    void* ray_accumulator;
};

/* Diagnostic information */
typedef void (*fclaw3d_diagnostics_initialize_t)(struct fclaw3d_global *glob,
                                                 void** acc);

typedef void (*fclaw3d_diagnostics_compute_t)(struct fclaw3d_global *glob,
                                              void* acc);

typedef void (*fclaw3d_diagnostics_gather_t)(struct fclaw3d_global *glob,
                                             void* acc,
                                             int init_flag);

typedef void (*fclaw3d_diagnostics_reset_t)(struct  fclaw3d_global *glob,
                                            void* acc);

typedef void (*fclaw3d_diagnostics_finalize_t)(struct  fclaw3d_global *glob,
                                               void** acc);

struct fclaw3d_diagnostics_vtable
{
    /* patch diagnostic functions (error, conservation, area, etc) */
    fclaw3d_diagnostics_initialize_t     patch_init_diagnostics;
    fclaw3d_diagnostics_compute_t        patch_compute_diagnostics;
    fclaw3d_diagnostics_gather_t         patch_gather_diagnostics;
    fclaw3d_diagnostics_reset_t          patch_reset_diagnostics;
    fclaw3d_diagnostics_finalize_t       patch_finalize_diagnostics;

    /* gauge diagnostic functions  */
    fclaw3d_diagnostics_initialize_t     solver_init_diagnostics;
    fclaw3d_diagnostics_compute_t        solver_compute_diagnostics;
    fclaw3d_diagnostics_gather_t         solver_gather_diagnostics;
    fclaw3d_diagnostics_reset_t          solver_reset_diagnostics;
    fclaw3d_diagnostics_finalize_t       solver_finalize_diagnostics;

    /* solver diagnostic functions (other solver functions) */
    fclaw3d_diagnostics_initialize_t     gauges_init_diagnostics;
    fclaw3d_diagnostics_compute_t        gauges_compute_diagnostics;
    fclaw3d_diagnostics_gather_t         gauges_gather_diagnostics;
    fclaw3d_diagnostics_reset_t          gauges_reset_diagnostics;
    fclaw3d_diagnostics_finalize_t       gauges_finalize_diagnostics;

    /* ray defined diagnostics */
    fclaw3d_diagnostics_initialize_t     ray_init_diagnostics;
    fclaw3d_diagnostics_compute_t        ray_compute_diagnostics;
    fclaw3d_diagnostics_gather_t         ray_gather_diagnostics;
    fclaw3d_diagnostics_reset_t          ray_reset_diagnostics;
    fclaw3d_diagnostics_finalize_t       ray_finalize_diagnostics;

    /* user defined diagnostics */
    fclaw3d_diagnostics_initialize_t     user_init_diagnostics;
    fclaw3d_diagnostics_compute_t        user_compute_diagnostics;
    fclaw3d_diagnostics_gather_t         user_gather_diagnostics;
    fclaw3d_diagnostics_reset_t          user_reset_diagnostics;
    fclaw3d_diagnostics_finalize_t       user_finalize_diagnostics;

    int is_set;
};

fclaw3d_diagnostics_vtable_t* fclaw3d_diagnostics_vt(struct fclaw3d_global* glob);

void fclaw3d_diagnostics_vtable_initialize(struct fclaw3d_global* glob);

double fclaw3d_domain_global_minimum (struct fclaw3d_domain* domain, double d);

void fclaw3d_diagnostics_initialize(struct fclaw3d_global *glob);

void fclaw3d_diagnostics_gather(struct fclaw3d_global *glob, int init_flag);

void fclaw3d_diagnostics_reset(struct fclaw3d_global *glob);

void fclaw3d_diagnostics_finalize(struct fclaw3d_global *glob);

#ifdef __cplusplus
}
#endif

#endif
