/*
Copyright (c) 2012-2024 Carsten Burstedde, Donna Calhoun, Scott Aiton
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
/** 
 * @file
 * @brief Functions needed for underlying wrapped types
 *        Most users will not need to include the file, unless doing something more advanced.
 */


#ifndef FCLAW2D_WRAP_H
#define FCLAW2D_WRAP_H

#include <fclaw_wrap.h>
#include <forestclaw2d.h>  /* Contains definition of patch-iterator callback */

#ifdef __cplusplus
extern "C"
{
#if 0
}                               /* need this because indent is dumb */
#endif
#endif

/**
 * @brief Wraps a 2d domain in a fclaw_domain_t struct.
 *
 * This overwrites the user pointer in the domain and domain patches with a
 * pointer to the dimension independent types.
 * 
 * @param[in,out] domain2d The domain to wrap
 * @return struct fclaw_domain* the wrapped domain
 */
struct fclaw_domain* fclaw_domain_wrap_2d(fclaw2d_domain_t *domain2d);

/**
 * @brief Wraps a patch callback for 2d domains.
 * This is used by passing this callback to a function that takes a @ref fclaw2d_patch_callback_t.
 * The user pointer should be a pointer to a @ref fclaw_patch_wrap_user_t.
 */
void fclaw2d_patch_wrap_cb(fclaw2d_domain_t * domain, 
                           fclaw2d_patch_t * patch,
                           int blockno, int patchno, void *user);

/**
 * @brief Wraps a transfer callback for 2d domains.
 * This is used by passing this callback to a function that takes a @ref fclaw2d_transfer_callback_t.
 * The user pointer should be a pointer to a @ref fclaw_transfer_wrap_user_t.
 */
void
fclaw2d_transfer_wrap_cb(fclaw2d_domain_t * old_domain,
                         fclaw2d_patch_t * old_patch,
                         fclaw2d_domain_t * new_domain,
                         fclaw2d_patch_t * new_patch,
                         int blockno,
                         int old_patchno, int new_patchno,
                         void *user);

/**
 * @brief Wraps a match callback for 2d domains.
 * This is used by passing this callback to a function that takes a @ref fclaw2d_match_callback_t.
 * The user pointer should be a pointer to a @ref fclaw_match_wrap_user_t.
 */
void
fclaw2d_match_wrap_cb(fclaw2d_domain_t * old_domain,
                      fclaw2d_patch_t * old_patch_2d,
                      fclaw2d_domain_t * new_domain,
                      fclaw2d_patch_t * new_patch_2d,
                      fclaw2d_patch_relation_t newsize,
                      int blockno,
                      int old_patchno, int new_patchno,
                      void *user);

/** 
 * @brief Wraps a partition pack callback for 2d domains.
 * This is used by passing this callback to a function that takes a @ref fclaw2d_pack_callback_t.
 * The user pointer should be a pointer to a @ref fclaw_pack_wrap_user_t.
 */
void
fclaw2d_pack_wrap_cb(fclaw2d_domain_t * domain,
                     fclaw2d_patch_t * patch,
                     int blockno, int patchno,
                     void *pack_data_here,
                     void *user);

/**
 * @brief Wraps a intersect callback for 2d domains.
 * This is used by passing this callback to a function that takes a @ref fclaw2d_intersect_callback_t.
 * The user pointer should be a pointer to a @ref fclaw_intersect_wrap_user_t.
 */
int 
fclaw2d_intersect_wrap_cb (fclaw2d_domain_t * domain,
                           fclaw2d_patch_t * patch,
                           int blockno, int patchno,
                           void *ray, double *integral,
                           void *user);
/**
 * @brief Wraps a interpolate_point callback for 2d domains.
 * This is used by passing this callback to a function that takes a @ref fclaw2d_interpolate_point_callback_t.
 * The user pointer should be a pointer to a @ref fclaw_interpolate_point_wrap_user_t.
 */
int
fclaw2d_interpolate_point_wrap_cb (fclaw2d_domain_t * domain,
                                   fclaw2d_patch_t * patch,
                                   int blockno, int patchno,
                                   void *point, void *user);

/**
 * @brief Wrap get dimension independent domain from a 2d error code.
 * 
 * @param errcode the 2d error code
 * @return int the dimension independent error code
 */
int 
fclaw2d_file_error_wrap(int errcode);

#ifdef __cplusplus
#if 0
{
#endif
}
#endif

#endif
