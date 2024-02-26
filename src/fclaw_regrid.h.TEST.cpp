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

#include <fclaw_ghost_fill.h>
#include <fclaw_global.h>
#include <fclaw_regrid.h>
#include <fclaw_domain.h>
#include <fclaw_patch.h>
#include <fclaw_map.h>
#include <fclaw_vtable.h>
#include <fclaw_convenience.h>
#include <fclaw_options.h>
#include <test.hpp>
#include <test/test.hpp>
#include <fstream>
#include <bitset>

#include <fclaw_forestclaw.h>

namespace {
struct TestData {
    fclaw_global_t* glob;
    fclaw_options_t fopts;
    fclaw_domain_t *domain;
    fclaw_map_context_t* map;
    fclaw_patch_vtable_t* patch_vt;
    fclaw_vtable_t* fc_vt;

    TestData(fclaw_domain_t* domain, int minlevel, int maxlevel){
        glob = fclaw_global_new_comm(domain->mpicomm,domain->mpisize,domain->mpirank);
        memset(&fopts, 0, sizeof(fopts));
        fopts.mi=1;
        fopts.mj=1;
        fopts.mk=1;
        fopts.minlevel=minlevel;
        fopts.maxlevel=maxlevel;
        fopts.manifold=false;
        fopts.refratio=2;
        fopts.bx = 1;
        fopts.by = 1;
        fopts.bz = 1;
        fopts.compute_error = true;
        fopts.subcycle = true;
        fopts.init_ghostcell = false;
        fclaw_options_store(glob, &fopts);

        fclaw_global_store_domain(glob, domain);

        if(domain->refine_dim == 2)
        {
            map = fclaw_map_new_nomap();
            fclaw_map_store(glob, map);
        }
        //map = fclaw3d_map_new_nomap();
        //fclaw_global_store_map_3d(glob, map);

        fclaw_vtables_initialize(glob);
        patch_vt = fclaw_patch_vt(glob);
        patch_vt->physical_bc = 
            [](fclaw_global_t* glob,
               fclaw_patch_t * patch,
               int blockno,
               int patchno,
               double t,
               double dt,
               int intersects_phys_bdry[],
               int time_interp)
            {
                //do nothing
            };
        fc_vt = fclaw_vt(glob);
    }
    void setup(){
        fclaw_initialize(glob);
    }
    ~TestData(){
        fclaw_domain_reset(glob);
        //fclaw_domain_destroy(glob->domain);
        //fclaw3d_map_destroy(map);
        fclaw_global_destroy(glob);
    }
};
}


TEST_CASE("after_regrid have_new_refinement is false if domain doesn't change")
{
    int minlevel = 1;
    int maxlevel = 3;
    fclaw_domain_t* domain = fclaw_domain_new_unitsquare(sc_MPI_COMM_WORLD, minlevel);
    TestData test_data(domain,minlevel, maxlevel);

    test_data.patch_vt->patch_new = 
        []()->void*
        {
            return NULL;
        };
    test_data.patch_vt->build = 
        [](fclaw_global_t* glob,
           fclaw_patch_t * patch,
           int blockno,
           int patchno,
           void*)
        {
            //do nothing
        };
    test_data.patch_vt->ghost_packsize = 
        [](fclaw_global_t* glob)->size_t
        {
            return 0;
        };
    test_data.patch_vt->patch_delete = 
        [](void*)
        {
            //do nothing
        };
    test_data.patch_vt->partition_packsize = 
        [](fclaw_global_t* glob)->size_t
        {
            return 0;
        };
    test_data.patch_vt->partition_pack = 
        [](fclaw_global_t* glob,
           fclaw_patch_t * patch,
           int blockno,
           int patchno,
           void* buffer)
        {
            //do nothing
        };
    test_data.patch_vt->transform_init_data
        = [](fclaw_global_t* glob,
             fclaw_patch_t * patch,
             int blockno,
             int patchno,
             fclaw_patch_transform_data_t* transform)
        {
            //do nothing
        };
    test_data.patch_vt->transform_face = 
        [](int,int,int*)
        {
            //do nothing
        };
    test_data.patch_vt->transform_face_intra = 
        [](int*)
        {
            //do nothing
        };
    test_data.patch_vt->average_face = 
        [](fclaw_global_t*,
           fclaw_patch_t *,
           fclaw_patch_t *,
           int,
           int,
           int,
           int,
           int,
           int,
           fclaw_patch_transform_data *)
        {
            //do nothing
        };
    test_data.patch_vt->average_corner = 
        [](fclaw_global_t*,
           fclaw_patch_t *,
           fclaw_patch_t *,
           int,
           int,
           int,
           int,
           fclaw_patch_transform_data *)
        {
            //do nothing
        };
    test_data.patch_vt->copy_face = 
        [](fclaw_global_t*,
           fclaw_patch_t *,
           fclaw_patch_t *,
           int,
           int,
           fclaw_patch_transform_data *)
        {
            //do nothing
        };
    test_data.patch_vt->copy_corner = 
        [](fclaw_global_t*,
           fclaw_patch_t *,
           fclaw_patch_t *,
           int,
           int,
           int,
           int,
           fclaw_patch_transform_data *)
        {
            //do nothing
        };
    test_data.patch_vt->interpolate_face = 
        [](fclaw_global_t*,
           fclaw_patch_t *,
           fclaw_patch_t *,
           int,
           int,
           int,
           int,
           int,
           int,
           fclaw_patch_transform_data *)
        {
            //do nothing
        };
    test_data.patch_vt->interpolate_corner = 
        [](fclaw_global_t*,
           fclaw_patch_t *,
           fclaw_patch_t *,
           int,
           int,
           int,
           int,
           fclaw_patch_transform_data *)
        {
            //do nothing
        };
    test_data.patch_vt->tag4coarsening = 
        [](fclaw_global_t* glob,
           fclaw_patch_t * patch,
           int blockno,
           int patchno,
           int init_flag)->int
        {
            if(!init_flag)
            {
                return patch->xlower >= 0.5 && patch->ylower >= 0.5;
            }
            return 0;
        };
    test_data.patch_vt->tag4refinement = 
        [](fclaw_global_t* glob,
           fclaw_patch_t * patch,
           int blockno,
           int patchno,
           int init_flag)->int
        {
            return patch->xupper <= 0.5 && patch->yupper <= 0.5;
        };

    CHECK_EQ(test_data.glob->domain->local_num_patches, 4);
    test_data.setup();
    CHECK_EQ(test_data.glob->domain->local_num_patches, 12+16);

    test_data.fc_vt->after_regrid = 
        [](fclaw_global_t* glob, int have_new_refinement)
        {
            CHECK_EQ(have_new_refinement , 0);
        };
    fclaw_regrid(test_data.glob);
    CHECK_EQ(test_data.glob->domain->local_num_patches, 12+16);

}
TEST_CASE("after_regrid have_new_refinement is true if domain changes")
{
    int minlevel = 1;
    int maxlevel = 3;
    fclaw_domain_t* domain = fclaw_domain_new_unitsquare(sc_MPI_COMM_WORLD, minlevel);
    TestData test_data(domain,minlevel, maxlevel);

    test_data.patch_vt->patch_new = 
        []()->void*
        {
            return NULL;
        };
    test_data.patch_vt->build = 
        [](fclaw_global_t* glob,
           fclaw_patch_t * patch,
           int blockno,
           int patchno,
           void*)
        {
            //do nothing
        };
    test_data.patch_vt->ghost_packsize = 
        [](fclaw_global_t* glob)->size_t
        {
            return 0;
        };
    test_data.patch_vt->patch_delete = 
        [](void*)
        {
            //do nothing
        };
    test_data.patch_vt->partition_packsize = 
        [](fclaw_global_t* glob)->size_t
        {
            return 0;
        };
    test_data.patch_vt->partition_pack = 
        [](fclaw_global_t* glob,
           fclaw_patch_t * patch,
           int blockno,
           int patchno,
           void* buffer)
        {
            //do nothing
        };
    test_data.patch_vt->transform_init_data
        = [](fclaw_global_t* glob,
             fclaw_patch_t * patch,
             int blockno,
             int patchno,
             fclaw_patch_transform_data_t* transform)
        {
            //do nothing
        };
    test_data.patch_vt->transform_face = 
        [](int,int,int*)
        {
            //do nothing
        };
    test_data.patch_vt->transform_face_intra = 
        [](int*)
        {
            //do nothing
        };
    test_data.patch_vt->average_face = 
        [](fclaw_global_t*,
           fclaw_patch_t *,
           fclaw_patch_t *,
           int,
           int,
           int,
           int,
           int,
           int,
           fclaw_patch_transform_data *)
        {
            //do nothing
        };
    test_data.patch_vt->average_corner = 
        [](fclaw_global_t*,
           fclaw_patch_t *,
           fclaw_patch_t *,
           int,
           int,
           int,
           int,
           fclaw_patch_transform_data *)
        {
            //do nothing
        };
    test_data.patch_vt->copy_face = 
        [](fclaw_global_t*,
           fclaw_patch_t *,
           fclaw_patch_t *,
           int,
           int,
           fclaw_patch_transform_data *)
        {
            //do nothing
        };
    test_data.patch_vt->copy_corner = 
        [](fclaw_global_t*,
           fclaw_patch_t *,
           fclaw_patch_t *,
           int,
           int,
           int,
           int,
           fclaw_patch_transform_data *)
        {
            //do nothing
        };
    test_data.patch_vt->interpolate_face = 
        [](fclaw_global_t*,
           fclaw_patch_t *,
           fclaw_patch_t *,
           int,
           int,
           int,
           int,
           int,
           int,
           fclaw_patch_transform_data *)
        {
            //do nothing
        };
    test_data.patch_vt->interpolate_corner = 
        [](fclaw_global_t*,
           fclaw_patch_t *,
           fclaw_patch_t *,
           int,
           int,
           int,
           int,
           fclaw_patch_transform_data *)
        {
            //do nothing
        };
    test_data.patch_vt->interpolate2fine = 
        [](fclaw_global_t*,
           fclaw_patch_t *,
           fclaw_patch_t *,
           int,
           int,
           int)
        {
            //do nothing
        };
    test_data.patch_vt->tag4coarsening = 
        [](fclaw_global_t* glob,
           fclaw_patch_t * patch,
           int blockno,
           int patchno,
           int init_flag)->int
        {
            return 0;
        };
    test_data.patch_vt->tag4refinement = 
        [](fclaw_global_t* glob,
           fclaw_patch_t * patch,
           int blockno,
           int patchno,
           int init_flag)->int
        {
            int retval = patch->xupper <= 0.5 && patch->yupper <= 0.5;
            if(!init_flag)
            {
                retval |= patch->xlower >= 0.5 && patch->ylower >= 0.5;
            }
            return retval;
        };

    CHECK_EQ(test_data.glob->domain->local_num_patches, 4);
    test_data.setup();
    CHECK_EQ(test_data.glob->domain->local_num_patches, 12+16);

    test_data.fc_vt->after_regrid = 
        [](fclaw_global_t* glob, int have_new_refinement)
        {
            CHECK_EQ(have_new_refinement , 1);
        };
    fclaw_regrid(test_data.glob);
    CHECK_EQ(test_data.glob->domain->local_num_patches, 8+2*16);

}