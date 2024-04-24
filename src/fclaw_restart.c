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
#include <fclaw_file.h>
#include <fclaw_convenience.h>
#include <fclaw_regrid.h>
#include <fclaw_partition.h>
#include <fclaw_forestclaw.h>

#define CHECK_ERROR_CODE(refine_dim, errcode, str) \
do { \
    int reslen, retval; \
    char err_str[sc_MPI_MAX_ERROR_STRING]; \
    if (errcode != FCLAW_FILE_ERR_SUCCESS) \
    { \
        retval = fclaw_file_error_string (refine_dim, errcode, err_str, &reslen); \
        SC_CHECK_ABORTF (!retval, "%s: error string function not successful", str); \
        SC_ABORTF ("%s: %*.*s", str, reslen, reslen, err_str); \
    } \
} while(0)

static char*
get_used_ini(fclaw_global_t * glob)
{
    char *buffer = NULL;
    long length;
    if(glob->mpirank == 0)
    {
        sc_options_t * options = fclaw_global_get_attribute(glob, "fclaw_options");
        int retval = sc_options_save (fclaw_get_package_id (),
                                      FCLAW_VERBOSITY_ERROR, 
                                      options, 
                                      "fclaw_options.ini.used");

        // read the entire file into a string
        FILE *file = fopen("fclaw_options.ini.used", "r");
        if (file == NULL)
        {
            printf("Cannot open file\n");
            return NULL;
        }

        fseek(file, 0, SEEK_END);
        length = ftell(file);
        fseek(file, 0, SEEK_SET);

        buffer = FCLAW_ALLOC(char, length + 1);
        if (buffer)
        {
            fread(buffer, 1, length, file);
        }
        fclose(file);

        buffer[length] = '\0';
    }
    //broadcast the lenth
    sc_MPI_Bcast(&length, 1, sc_MPI_LONG, 0, glob->mpicomm);
    //allocate the buffer on other ranks
    if(glob->mpirank != 0)
    {
        buffer = FCLAW_ALLOC(char, length + 1);
    }
    FCLAW_ASSERT(buffer != NULL);
    //broadcast the string
    sc_MPI_Bcast(buffer, length+1, sc_MPI_CHAR, 0, glob->mpicomm);

    return buffer;
    

}

static void
free_used_ini(void* data)
{
    char* buffer = (char*) data;
    FCLAW_FREE(buffer);
}

typedef struct pack_iter
{
    fclaw_global_t * glob;
    int curr_index;
    size_t size;
    sc_array_t* patches;
    int pointerno;
    int reading;
}pack_iter_t;

static void
get_patches(fclaw_domain_t * domain, fclaw_patch_t * patch, int blockno, int patchno,  void *user)
{
    pack_iter_t *user_data = (pack_iter_t*)user;
    sc_array_t *patches = user_data->patches;
    sc_array_t * current_arr = (sc_array_t *) sc_array_index (patches, user_data->curr_index);

    void* data = fclaw_patch_restart_get_pointer(user_data->glob, patch, blockno, patchno, user_data->pointerno);

    sc_array_init_data(current_arr, data, user_data->size, 1);

    user_data->curr_index++;
}

static void
set_patches(fclaw_domain_t * domain, fclaw_patch_t * patch, int blockno, int patchno,  void *user)
{
    pack_iter_t *user_data = (pack_iter_t*)user;
    sc_array_t *patches = user_data->patches;
    sc_array_t * current_arr = (sc_array_t *) sc_array_index (patches, user_data->curr_index);

    fclaw_build_mode_t build_mode = FCLAW_BUILD_FOR_UPDATE;

    if(user_data->pointerno == 0)
    {
	    fclaw_patch_build(user_data->glob, patch, blockno, patchno,(void*) &build_mode);
    }

    void* data = fclaw_patch_restart_get_pointer(user_data->glob, patch, blockno, patchno, user_data->pointerno);

    memcpy(data, sc_array_index(current_arr, 0), user_data->size);

    sc_array_reset(current_arr);

    user_data->curr_index++;
}

static
void restart (fclaw_global_t * glob,
              const char* restart_filename,
              const char* partition_filename,
              int timer)
{
    int refine_dim = glob->domain->refine_dim;
    fclaw_domain_reset(glob);

    int errcode;
    sc_array_t* partition = NULL;
    char user_string[FCLAW3D_FILE_USER_STRING_BYTES];
    if(partition_filename != NULL)
    {
        partition = sc_array_new(sizeof(p4est_gloidx_t));
        fclaw_file_read_partition(refine_dim, 
                                  partition_filename, 
                                  user_string, 
                                  glob->mpicomm, 
                                  partition, 
                                  &errcode);
        CHECK_ERROR_CODE(refine_dim, errcode, "restart read_partition");
    }

    fclaw_file_context_t *fc 
        = fclaw_file_open_read (refine_dim,
                                restart_filename, 
                                user_string, 
                                glob->mpicomm, 
                                partition, 
                                &glob->domain, 
                                &errcode);
    CHECK_ERROR_CODE(refine_dim, errcode, "restart open_file");

    fclaw_domain_setup(glob, glob->domain);

    if(partition != NULL)
    {
        sc_array_destroy(partition);
    }

        sc_array_t globsize;
    sc_array_init_size(&globsize, sizeof(size_t), 1);

    fc = fclaw_file_read_block(fc, user_string, sizeof(size_t), &globsize, &errcode);
    CHECK_ERROR_CODE(refine_dim, errcode, "restart read globsize");

    size_t glob_packsize = *((size_t*) sc_array_index(&globsize, 0));
    sc_array_reset(&globsize);

    sc_array_t glob_buffer;
    sc_array_init_size(&glob_buffer, glob_packsize, 1);

    fc = fclaw_file_read_block(fc, user_string, glob_packsize, &glob_buffer, &errcode);
    CHECK_ERROR_CODE(refine_dim, errcode, "restart read glob buffer");

    fclaw_global_unpack((char *) sc_array_index(&glob_buffer, 0), glob);

    sc_array_reset(&glob_buffer);

    int num_pointers = fclaw_patch_restart_num_pointers(glob);
    size_t sizes[num_pointers];
    fclaw_patch_restart_pointer_sizes(glob, sizes);
    const char* names[num_pointers];
    fclaw_patch_restart_names(glob, names);
    for(int i = 0; i < 1; i++)
    {
        sc_array_t *patches = sc_array_new_count(sizeof(sc_array_t), glob->domain->local_num_patches);
        pack_iter_t user;
        user.glob = glob;
        user.curr_index = 0;
        user.patches = patches;
        user.size = sizes[i];
        user.pointerno = i;
        user.reading = 1;

        fc = fclaw_file_read_array(fc, user_string, sizes[i], patches, &errcode);
        if(strncmp(user_string, names[i], strlen(names[i])) != 0)
        {
            fclaw_abortf("User string mismatch: %s != %s\n", user_string, names[i]);
        }
        CHECK_ERROR_CODE(refine_dim, errcode, "restart read patches");

        fclaw_domain_iterate_patches(glob->domain, set_patches, &user);

        sc_array_destroy(patches);
    }

    fclaw_file_close(fc, &errcode);
    CHECK_ERROR_CODE(refine_dim, errcode, "restart close file");

    fclaw_initialize_domain_flags(glob);
    fclaw_exchange_setup(glob,timer);
    fclaw_regrid_set_neighbor_types(glob);
}
/* -----------------------------------------------------------------------
    Public interface
    -------------------------------------------------------------------- */

void
fclaw_restart_output_frame (fclaw_global_t * glob, int iframe)
{
    int refine_dim = glob->domain->refine_dim;

    char filename[BUFSIZ];
    char parition_filename[BUFSIZ];
    snprintf(filename, BUFSIZ, "fort_frame_%04d.restart", iframe);
    snprintf(parition_filename, BUFSIZ, "fort_frame_%04d.partition", iframe);

    int errcode;
    fclaw_file_context_t *fc 
        = fclaw_file_open_write (filename, "ForestClaw data file",
                                 glob->domain, &errcode);
    CHECK_ERROR_CODE(refine_dim , errcode, "restart open file");

    char* used_ini = fclaw_global_get_attribute(glob, "fclaw_used_ini");
    if(used_ini == NULL)
    {
        used_ini = get_used_ini(glob);
        fclaw_global_attribute_store(glob, 
                                     "fclaw_used_ini", 
                                     used_ini, 
                                     NULL, 
                                     free_used_ini);
    }

    size_t used_ini_length = strlen(used_ini);
    sc_array_t array;
    sc_array_init_data(&array, &used_ini_length, sizeof(size_t), 1);

    fc = fclaw_file_write_block(fc, "used_ini_length", sizeof(size_t), &array, &errcode);
    CHECK_ERROR_CODE(refine_dim , errcode, "write used_ini_length");

    sc_array_init_data(&array, used_ini, used_ini_length, 1);
    fc = fclaw_file_write_block(fc, "used_ini", used_ini_length, &array, &errcode);
    CHECK_ERROR_CODE(refine_dim , errcode, "write used_ini");

    size_t glob_packsize = fclaw_global_packsize(glob);

    sc_array_t globsize;
    sc_array_init_size(&globsize, sizeof(size_t), 1);
    *((size_t*) sc_array_index(&globsize, 0)) = glob_packsize;

    fc = fclaw_file_write_block(fc, "glob_size", sizeof(size_t), &globsize, &errcode);
    CHECK_ERROR_CODE(refine_dim , errcode, "write globsize");

    sc_array_reset(&globsize);

    sc_array_t glob_buffer;
    sc_array_init_size(&glob_buffer, glob_packsize, 1);
    fclaw_global_pack(glob,(char *) sc_array_index(&glob_buffer, 0));

    fc = fclaw_file_write_block(fc, "glob", glob_packsize, &glob_buffer, &errcode);
    CHECK_ERROR_CODE(refine_dim , errcode, "write glob buffer");

    sc_array_reset(&glob_buffer);


    int num_pointers = fclaw_patch_restart_num_pointers(glob);
    size_t sizes[num_pointers];
    fclaw_patch_restart_pointer_sizes(glob, sizes);
    const char* names[num_pointers];
    fclaw_patch_restart_names(glob, names);
    for(int i = 0; i < 1; i++)
    {
        sc_array_t *patches = sc_array_new_count(sizeof(sc_array_t), glob->domain->local_num_patches);
        pack_iter_t user;
        user.glob = glob;
        user.curr_index = 0;
        user.patches = patches;
        user.size = sizes[i];
        user.pointerno = i;
        user.reading = 0;
        fclaw_domain_iterate_patches(glob->domain, get_patches, &user);

    
        fc = fclaw_file_write_array(fc, names[i], sizes[i], patches, &errcode);
        CHECK_ERROR_CODE(refine_dim , errcode, "write patches");

        for(int i = 0; i < glob->domain->local_num_patches; i++)
        {
            sc_array_t * current_arr = (sc_array_t *) sc_array_index (patches, i);
            sc_array_reset(current_arr);
        }
        sc_array_destroy(patches);
    }


    fclaw_file_close(fc, &errcode);
    fclaw_file_write_partition (parition_filename,
                                "Test partition write",
                                glob->domain, &errcode);
    CHECK_ERROR_CODE(refine_dim , errcode, "close file");

    //fclaw_restart_test_from_file(glob, filename, parition_filename);
}


void
fclaw_restart_from_file (fclaw_global_t * glob,
                         const char* restart_filename,
                         const char* partition_filename)
{
    restart(glob, restart_filename, partition_filename, FCLAW_TIMER_INIT);
}

void fclaw_output_checkpoint(fclaw_global_t* glob, int iframe)
{
    const fclaw_options_t *fclaw_opt = fclaw_get_options(glob);

    if(fclaw_opt->checkpoint_out)
    {
        fclaw_timer_start (&glob->timers[FCLAW_TIMER_OUTPUT]);

        fclaw_restart_output_frame(glob,iframe);

        fclaw_timer_stop (&glob->timers[FCLAW_TIMER_OUTPUT]);
    }
}