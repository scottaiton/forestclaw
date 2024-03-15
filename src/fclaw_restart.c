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
#include <fclaw_domain.h>
#include <fclaw_exchange.h>
#include <fclaw3d_file.h>
#include <fclaw_clawpatch.h>
#include <fclaw_convenience.h>
#include <fclaw3d_wrap.h>

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
    fclaw_patch_partition_pack(user_data->glob, patch, blockno, patchno, buffer);
    user_data->curr_index++;
}
static void
set_patches(fclaw_domain_t * domain, fclaw_patch_t * patch, int blockno, int patchno, void *user)
{
    pack_iter_t *user_data = (pack_iter_t*)user;
    sc_array_t *patches = user_data->patches;
    sc_array_t * current_arr = (sc_array_t *) sc_array_index (patches, user_data->curr_index);
    void* buffer = sc_array_index (current_arr, 0);

    fclaw_patch_partition_unpack(user_data->glob, user_data->glob->domain, patch, blockno, patchno, buffer);

    sc_array_reset(current_arr);

    user_data->curr_index++;
}
//TODO move this to header
void
fclaw_restart_from_file (fclaw_global_t * glob,
                         const char* restart_filename,
                         const char* partition_filename);

/* -----------------------------------------------------------------------
    Public interface
    -------------------------------------------------------------------- */

void
fclaw_restart_output_frame (fclaw_global_t * glob, int iframe)
{
    fclaw_patch_vtable_t *patch_vt = fclaw_patch_vt(glob);

    char filename[BUFSIZ];
    char parition_filename[BUFSIZ];
    snprintf(filename, BUFSIZ, "fort_frame_%04d.restart", iframe);
    snprintf(parition_filename, BUFSIZ, "fort_frame_%04d.partition", iframe);

    int errcode;
    fclaw3d_file_context_t *fc 
        = fclaw3d_file_open_write (filename, "ForestClaw data file",
                                    glob->domain->d3, &errcode);
    
    size_t glob_packsize = fclaw_global_packsize(glob);

    sc_array_t globsize;
    sc_array_init_size(&globsize, sizeof(size_t), 1);
    *((size_t*) sc_array_index(&globsize, 0)) = glob_packsize;
    fclaw3d_file_write_block(fc, "glob_size", sizeof(size_t), &globsize, &errcode);
    sc_array_reset(&globsize);

    sc_array_t glob_buffer;
    sc_array_init_size(&glob_buffer, glob_packsize, 1);
    fclaw_global_pack(glob,(char *) sc_array_index(&glob_buffer, 0));
    fclaw3d_file_write_block(fc, "glob", glob_packsize, &glob_buffer, &errcode);
    sc_array_reset(&glob_buffer);

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
    fclaw3d_file_write_partition (parition_filename,
                                           "Test partition write",
                                           glob->domain->d3, &errcode);

    fclaw_restart_from_file(glob, filename, parition_filename);

    fclaw_global_productionf("RESTATRTT!!\n");
}


void
fclaw_restart_from_file (fclaw_global_t * glob,
                         const char* restart_filename,
                         const char* partition_filename)
{
    fclaw_domain_reset(glob);

    int errcode;
    fclaw_patch_vtable_t *patch_vt = fclaw_patch_vt(glob);
    sc_array_t* partition = sc_array_new(sizeof(p4est_gloidx_t));
    char user_string[FCLAW3D_FILE_USER_STRING_BYTES];
    if(partition_filename != NULL)
    {
        fclaw3d_file_read_partition(partition_filename, user_string, glob->mpicomm, partition, &errcode);

    }
    fclaw3d_domain_t * domain_3d;
    fclaw3d_file_context_t *fc 
        = fclaw3d_file_open_read (restart_filename, user_string, glob->mpicomm, partition, &domain_3d, &errcode);
    glob->domain = fclaw_domain_wrap_3d(domain_3d);
    fclaw_domain_setup(glob, glob->domain);
    sc_array_destroy(partition);

    sc_array_t globsize;
    sc_array_init_size(&globsize, sizeof(size_t), 1);
    fclaw3d_file_read_block(fc, user_string, sizeof(size_t), &globsize, &errcode);
    size_t glob_packsize = *((size_t*) sc_array_index(&globsize, 0));
    sc_array_reset(&globsize);

    sc_array_t glob_buffer;
    sc_array_init_size(&glob_buffer, glob_packsize, 1);
    fclaw3d_file_read_block(fc, user_string, glob_packsize, &glob_buffer, &errcode);
    sc_array_reset(&glob_buffer);


    size_t packsize = patch_vt->partition_packsize(glob);
    sc_array_t* patches = sc_array_new(sizeof(sc_array_t));

    fclaw3d_file_read_array(fc, user_string, packsize, patches, &errcode);

    pack_iter_t user;
    user.glob = glob;
    user.curr_index = 0;
    user.patches = patches;
    user.packsize = packsize;
    user.patch_vt = patch_vt;
    fclaw_domain_iterate_patches(glob->domain, set_patches, &user);

    sc_array_destroy(patches);

    fclaw3d_file_close(fc, &errcode);
    fclaw_exchange_setup(glob,FCLAW_TIMER_OUTPUT);
}


