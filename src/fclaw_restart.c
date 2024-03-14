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

#include <fclaw_restart.h>
#include <fclaw_global.h>
#include <fclaw_options.h>
#include <fclaw_vtable.h>
#include <fclaw_patch.h>
#include <fclaw3d_file.h>
#include <fclaw_clawpatch.h>

typedef struct pack_iter
{
    fclaw_global_t * glob;
    int curr_index;
    size_t packsize;
    sc_array_t* patches;
    fclaw_patch_vtable_t *patch_vt;
}pack_iter_t;
static void
get_patches(fclaw_domain_t * domain, fclaw_patch_t * patch, int blockno, int patchno, void *user)
{
    pack_iter_t *user_data = (pack_iter_t*)user;
    sc_array_t *patches = user_data->patches;
    sc_array_t * current_arr = (sc_array_t *) sc_array_index (patches, user_data->curr_index);
    sc_array_init_size (current_arr, user_data->packsize, 1);
    void* buffer = sc_array_index (current_arr, 0);
    user_data->patch_vt->partition_pack(user_data->glob, patch, blockno, patchno, buffer);
    user_data->curr_index++;
}
/* -----------------------------------------------------------------------
    Public interface
    -------------------------------------------------------------------- */

void
fclaw_restart_output_frame (fclaw_global_t * glob, int iframe)
{
    fclaw_patch_vtable_t *patch_vt = fclaw_patch_vt(glob);

    char filename[BUFSIZ];
    snprintf(filename, BUFSIZ, "fort_frame_%04d.restart", iframe);

    int errcode;
    fclaw3d_file_context_t *fc 
        = fclaw3d_file_open_write (filename, "ForestClaw data file",
                                    glob->domain->d3, &errcode);

    size_t packsize = patch_vt->partition_packsize(glob);
    sc_array_t *patches = sc_array_new_count(sizeof(sc_array_t), glob->domain->local_num_patches);
    pack_iter_t user;
    user.glob = glob;
    user.curr_index = 0;
    user.patches = patches;
    user.packsize = packsize;
    user.patch_vt = patch_vt;
    fclaw_domain_iterate_patches(glob->domain, get_patches, &user);

    
    fclaw3d_file_write_array(fc, "meqn", packsize, patches, &errcode);

    for(int i = 0; i < glob->domain->local_num_patches; i++)
    {
        sc_array_t * current_arr = (sc_array_t *) sc_array_index (patches, i);
        sc_array_reset(current_arr);
    }

    sc_array_destroy(patches);

    fclaw3d_file_close(fc, &errcode);
    fclaw_global_productionf("RESTATRTT!!\n");
}




