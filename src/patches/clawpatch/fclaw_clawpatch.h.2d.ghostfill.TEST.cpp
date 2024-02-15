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

#include <fclaw_clawpatch.hpp>
#include <fclaw_clawpatch.h>
#include <fclaw_ghost_fill.h>
#include <fclaw_clawpatch_options.h>
#include <fclaw_clawpatch_output_vtk.h>
#include <fclaw2d_clawpatch46_fort.h>
#include <fclaw_clawpatch_output_ascii.h>
#include <fclaw_global.h>
#include <fclaw_regrid.h>
#include <fclaw_domain.h>
#include <fclaw_patch.h>
#include <fclaw_map.h>
#include <fclaw_map_brick.h>
#include <fclaw_convenience.h>
#include <fclaw2d_metric.hpp>
#include <fclaw2d_metric.h>
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
    fclaw_clawpatch_options_t* opts;
    fclaw_patch_vtable_t* patch_vt;
    fclaw_clawpatch_vtable_t * clawpatch_vt;

    TestData(fclaw_domain_t* domain, fclaw_map_context_t* map, int minlevel, int maxlevel){
        int size;
        int rank;
        sc_MPI_Comm_size(sc_MPI_COMM_WORLD, &size);
        sc_MPI_Comm_rank(sc_MPI_COMM_WORLD, &rank);
        glob = fclaw_global_new_comm(sc_MPI_COMM_WORLD, size, rank);
        opts = fclaw_clawpatch_options_new(2);
        memset(&fopts, 0, sizeof(fopts));
        fopts.mi=1;
        fopts.mj=1;
        fopts.minlevel=minlevel;
        fopts.maxlevel=maxlevel;
        fopts.manifold=false;
        fopts.refratio=2;
        fopts.bx = 1;
        fopts.by = 1;
        fopts.compute_error = true;
        fopts.subcycle = true;
        fopts.init_ghostcell = false;
        fclaw_options_store(glob, &fopts);

        opts->mx   = 5;
        opts->my   = 6;
        opts->mbc  = 2;
        opts->meqn = 1;
        opts->maux = 1;
        opts->rhs_fields = 1;
        fclaw_clawpatch_options_store(glob, opts);

        fclaw_global_store_domain(glob, domain);

        fclaw_map_store(glob, map);

        fclaw_vtables_initialize(glob);
        fclaw_clawpatch_vtable_initialize(glob, 4);
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

        clawpatch_vt = fclaw_clawpatch_vt(glob);

    }
    void setup(){
        fclaw_initialize(glob);
        if(test_indirect())
        {
            fclaw_clawpatch_vtable_t *vt = fclaw_clawpatch_vt(glob);
            vt->d2->fort_interpolate_face = TEST_2D_CLAWPATCH46_FORT_INTERPOLATE_FACE;
            vt->d2->fort_interpolate_corner = TEST_2D_CLAWPATCH46_FORT_INTERPOLATE_CORNER;
        }
    }
    ~TestData(){
        fclaw_clawpatch_options_destroy(opts);
        fclaw_domain_reset(glob);
        //fclaw_domain_destroy(glob->domain);
        //fclaw2d_map_destroy(map);
        fclaw_global_destroy(glob);
    }
};

int clawpatch_idx(fclaw_clawpatch_t* clawpatch, int i, int j, int m)
{
    int mx = clawpatch->mx;
    int my = clawpatch->my;
    int mbc = clawpatch->mbc;
    int idx = (i+mbc) + (j+mbc)*(mx+2*mbc) + m*(mx+2*mbc)*(my+2*mbc);
    return idx;
}
double fill_function(fclaw_clawpatch_t* clawpatch, int i, int j, int m)
{
    double x = clawpatch->xlower + (i+0.5)*clawpatch->dx;
    double y = clawpatch->ylower + (j+0.5)*clawpatch->dy;
    switch(m)
    {
        case 0:
            return 1;
        case 1:
            return 8*x + 2*y;
        case 2:
            return 9*x + 10*y;
    }
    return 0;
}
void get_bounds_with_ghost(fclaw_clawpatch_t* clawpatch, 
                          int *intersects_bc,
                          int *i_start,
                          int *i_stop,
                          int *j_start,
                          int *j_stop)
{
    *i_start = intersects_bc[0] ? 0 : -clawpatch->mbc;
    *i_stop  = intersects_bc[1] ? clawpatch->mx : clawpatch->mx + clawpatch->mbc;

    *j_start = intersects_bc[2] ? 0 : -clawpatch->mbc;
    *j_stop  = intersects_bc[3] ? clawpatch->my : clawpatch->my + clawpatch->mbc;
}
bool interior_cell(fclaw_clawpatch_t* clawpatch, int i, int j, int m)
{
    int mx = clawpatch->mx;
    int my = clawpatch->my;
    return i >= 0 && i < mx && j >= 0 && j < my;
}
bool face_ghost_cell(fclaw_clawpatch_t* clawpatch, int i, int j, int m)
{
    int mx = clawpatch->mx;
    int my = clawpatch->my;
    return ((i < 0 || i >= mx) && j >= 0 && j < my) ||
           (i >= 0 && i < mx && (j < 0 || j >= my));
}
bool corner_ghost_cell(fclaw_clawpatch_t* clawpatch, int i, int j, int m)
{
    int mx = clawpatch->mx;
    int my = clawpatch->my;
    return (i < 0 || i >= mx) && (j < 0 || j >= my);
}

void init_patch(fclaw_global_t* glob, fclaw_patch_t* patch)
{
    fclaw_clawpatch_options_t* opts = fclaw_clawpatch_get_options(glob);
    fclaw_clawpatch_t* clawpatch = fclaw_clawpatch_get_clawpatch(patch);
    // clear q
    double* q = fclaw_clawpatch_get_q(glob, patch);
    int size = fclaw_clawpatch_size(glob);
    memset(q, 0, sizeof(double)*size);
    //fill entire patch
    for(int m = 0; m < opts->meqn; m++)
    for(int j = -opts->mbc; j < opts->my+opts->mbc; j++)
    for(int i = -opts->mbc; i < opts->mx+opts->mbc; i++)
    {
        int idx = clawpatch_idx(clawpatch, i,j,m);
        //fill with some different linear functions
        q[idx] = fill_function(clawpatch, i, j, m);
    }

    //get physical boundaries
    int intersects_bc[fclaw_domain_num_faces(glob->domain)];
    for(int i=0;i<fclaw_domain_num_faces(glob->domain);i++)
    {
        fclaw_patch_relation_t type = fclaw_patch_get_face_type(patch, i);
        intersects_bc[i] = type == FCLAW_PATCH_BOUNDARY;
    }

    //zero out bounds with ghost
    int i_start, i_stop, j_start, j_stop;
    get_bounds_with_ghost(clawpatch, intersects_bc,&i_start,&i_stop,&j_start,&j_stop);
    for(int m = 0; m < opts->meqn; m++)
    for(int j = j_start; j < j_stop; j++)
    for(int i = i_start; i < i_stop; i++)
    {
        int idx = clawpatch_idx(clawpatch, i,j,m);
        q[idx]=0;
    }

    //fill interior
    for(int m = 0; m < opts->meqn; m++)
    for(int j = 0; j < opts->my; j++)
    for(int i = 0; i < opts->mx; i++)
    {
        int idx = clawpatch_idx(clawpatch, i,j,m);
        //fill with some different linear functions
        q[idx] = fill_function(clawpatch, i, j, m);
    }
}
void test_ghost_fill(TestData& tdata, TestData& tdata_out, std::string output_filename)
{
    //initialize patches
    fclaw_global_iterate_patches(
        tdata.glob, 
        [](fclaw_domain_t * domain, fclaw_patch_t * patch,
            int blockno, int patchno, void *user)
        {
            fclaw_global_iterate_t* g = (fclaw_global_iterate_t*)user;
            init_patch(g->glob, patch);
        }, 
        nullptr
    );

    fclaw_ghost_update(tdata.glob, tdata.fopts.minlevel, tdata.fopts.maxlevel, 0, 0, FCLAW_TIMER_NONE);

    //check ghost cells
    //fill output domain with error
    struct iterate_t {
        fclaw_global_t* error_glob;
        int num_incorrect_interior_cells;
        int num_incorrect_face_ghost_cells;
        int num_incorrect_edge_ghost_cells;
        int num_incorrect_corner_ghost_cells;
    };
    iterate_t iterate = {tdata_out.glob, 0, 0, 0, 0};
    fclaw_global_iterate_patches(
        tdata.glob, 
        [](fclaw_domain_t * domain, fclaw_patch_t * patch,
            int blockno, int patchno, void *user)
        {
            fclaw_global_iterate_t* g = (fclaw_global_iterate_t*)user;
            iterate_t* iterate = (iterate_t*)g->user;
            //clawpatch to store error in
            fclaw_patch_t* error_patch = &iterate->error_glob->domain->blocks[blockno].patches[patchno];  
            fclaw_clawpatch_t* error_clawpatch = fclaw_clawpatch_get_clawpatch(error_patch);
            int mbc = error_clawpatch->mbc;


            fclaw_clawpatch_options_t* opts = fclaw_clawpatch_get_options(g->glob);
            fclaw_clawpatch_t* clawpatch = fclaw_clawpatch_get_clawpatch(patch);

            // get q
            double* q = fclaw_clawpatch_get_q(g->glob, patch);
            double* error_q = fclaw_clawpatch_get_q(iterate->error_glob, error_patch);

            //get physical boundaries
            int intersects_bc[fclaw_domain_num_faces(domain)];
            for(int i=0;i<fclaw_domain_num_faces(domain);i++)
            {
                fclaw_patch_relation_t type = fclaw_patch_get_face_type(patch, i);
                intersects_bc[i] = type == FCLAW_PATCH_BOUNDARY;
            }

            //clear error q
            int size = fclaw_clawpatch_size(iterate->error_glob);
            memset(error_q, 0, sizeof(double)*size);

            //loop over all cells
            int i_start, i_stop, j_start, j_stop;
            get_bounds_with_ghost(clawpatch, intersects_bc,&i_start,&i_stop,&j_start,&j_stop);
            for(int m = 0; m < opts->meqn; m++)
            for(int j = j_start; j < j_stop; j++)
            for(int i = i_start; i < i_stop; i++)
            {
                int idx = clawpatch_idx(clawpatch, i,j,m);
                int error_idx = clawpatch_idx(error_clawpatch, i+mbc,j+mbc,m);
                //get expected value
                double expected = fill_function(clawpatch, i, j, m);
                error_q[error_idx] = expected - q[idx];
                if(q[idx] != doctest::Approx(expected))
                {
                    if(interior_cell(clawpatch, i,j,m))
                    {
                        iterate->num_incorrect_interior_cells++;
                    }
                    else if(face_ghost_cell(clawpatch, i,j,m))
                    {
                        iterate->num_incorrect_face_ghost_cells++;
                    }
                    else if(corner_ghost_cell(clawpatch, i,j,m))
                    {
                        iterate->num_incorrect_corner_ghost_cells++;
                    }
                    else{
                        SC_ABORT_NOT_REACHED();
                    }
                }
            }

        }, 
        &iterate
    );

    //check that patch was filled correctly
    CHECK_EQ(iterate.num_incorrect_interior_cells, 0);
    CHECK_EQ(iterate.num_incorrect_face_ghost_cells, 0);
    CHECK_EQ(iterate.num_incorrect_corner_ghost_cells, 0);

    //write output if --vtk option passed to test runner
    if(test_output_vtk())
    {
        INFO("Test failed output error to " << output_filename << ".vtu");
        fclaw_clawpatch_output_vtk_to_file(tdata_out.glob,output_filename.c_str());
    }
}

}// end anonymous namespace

//TODO move this somewhere else
TEST_CASE("2d clawpatch ghost pack/unpack")
{
    for(int mx   : {8,9,10})
    for(int my   : {8,9,10})
    for(int mbc  : {1,2})
    {
        int minlevel = 1;
        int maxlevel = 1;
        fclaw_domain_t* domain = fclaw_domain_new_unitsquare(sc_MPI_COMM_WORLD, minlevel);
        fclaw_map_context_t* map = fclaw_map_new_nomap();
        TestData test_data(domain, map, minlevel, maxlevel);

        test_data.opts->mx   = mx;
        test_data.opts->my   = my;
        test_data.opts->mbc  = mbc;
        test_data.opts->meqn = 3;

        test_data.setup();

        size_t expected_packsize = 
            ((mx+2*mbc)*(my+2*mbc)-(mx-2*mbc*2)*(my-2*mbc*2)) //1-interior
                                    *3*sizeof(double);
        
        REQUIRE_EQ(fclaw_patch_ghost_packsize(test_data.glob), expected_packsize);

        fclaw_patch_t * patch = &test_data.glob->domain->blocks[0].patches[0];
        fclaw_clawpatch_t* clawpatch = fclaw_clawpatch_get_clawpatch(patch);
        double * q = fclaw_clawpatch_get_q(test_data.glob, patch);

        for(int m = 0; m < 3; m++)
        for(int j = -mbc; j < my+mbc; j++)
        for(int i = -mbc; i < mx+mbc; i++)
        {
            int idx = clawpatch_idx(clawpatch, i,j,m);
            //fill with some different linear functions
            q[idx] = fill_function(clawpatch, i, j, m);
        }

        double pack_buffer[expected_packsize/sizeof(double)];
        fclaw_patch_local_ghost_pack(test_data.glob, patch, pack_buffer,0);

        memset(q, 0, sizeof(double)*fclaw_clawpatch_size(test_data.glob));

        fclaw_patch_remote_ghost_unpack(test_data.glob,patch, 0,0,pack_buffer,0);

        for(int m = 0; m < test_data.opts->meqn; m++)
        for(int j = -mbc; j < test_data.opts->my+mbc; j++)
        for(int i = -mbc; i < test_data.opts->mx+mbc; i++)
        {
            int idx = clawpatch_idx(clawpatch, i,j,m);
            if((i>=2*mbc && i < mx-2*mbc) && (j>=2*mbc && j < my-2*mbc))
            {
                CHECK_EQ(q[idx], 0);
            }
            else 
            {
                CHECK_EQ(q[idx], fill_function(clawpatch, i, j, m));
            }
        }
    }
}

TEST_CASE("2d clawpatch ghost fill on uniform square")
{
    int test_no= 0;
    for(int mx   : {10})
    for(int my   : {10})
    for(int mbc  : {1,2})
    {
        int minlevel = 2;
        int maxlevel = 2;
        fclaw_domain_t* domain = fclaw_domain_new_unitsquare(sc_MPI_COMM_WORLD, minlevel);
        fclaw_map_context_t* map = fclaw_map_new_nomap();
        TestData test_data(domain, map, minlevel, maxlevel);

        test_data.opts->mx   = mx;
        test_data.opts->my   = my;
        test_data.opts->mbc      = mbc;
        test_data.opts->meqn     = 3;

        test_data.setup();

        //create output domain with bigger size, so that we can see ghost cells
        //in the vtk output
        fclaw_domain_t* domain_out = fclaw_domain_new_unitsquare(sc_MPI_COMM_WORLD, minlevel);
        TestData test_data_out(domain_out, map, minlevel, maxlevel);

        test_data_out.opts->mx   = mx+2*mbc;
        test_data_out.opts->my   = my+2*mbc;
        test_data_out.opts->mbc      = mbc;
        test_data_out.opts->meqn     = 3;

        test_data_out.setup();

        
        char test_no_str[5];
        snprintf(test_no_str, 5, "%04d", test_no);
        std::string filename = "2d_clawpatch_ghost_fill_on_uniform_square_"+std::string(test_no_str);

        test_ghost_fill(test_data, test_data_out, filename);

        test_no++;

    }

}

TEST_CASE("2d clawpatch ghost fill on square with refinement")
{
    int test_no= 0;
    for(int mx   : {8})
    for(int my   : {8})
    for(int mbc  : {2})
    {
        int minlevel = 2;
        int maxlevel = 4;
        fclaw_domain_t* domain = fclaw_domain_new_unitsquare(sc_MPI_COMM_WORLD, minlevel);
        fclaw_map_context_t* map = fclaw_map_new_nomap();
        TestData test_data(domain, map, minlevel, maxlevel);

        test_data.opts->mx   = mx;
        test_data.opts->my   = my;
        test_data.opts->mbc      = mbc;
        test_data.opts->meqn     = 3;

        //refine on if interior patch
        auto tag4refinement 
          = [](fclaw_global_t* glob,
               fclaw_patch_t * patch,
               int blockno,
               int patchno,
               int initflag)
        {
            if(patch->xlower == doctest::Approx(0.5)&&
               patch->ylower == doctest::Approx(0.5))
            {
                return 1;
            }
            else 
            {
                return 0;
            }
        };
        auto tag4coarsening
          = [](fclaw_global_t* glob,
               fclaw_patch_t * patch,
               int blockno,
               int patchno,
               int initflag)
        {
            return 0;
        };
        auto fort_interpolate2fine
          =[](const int* mx, 
              const int* my, 
              const int* mbc, 
              const int* meqn,
              double qcoarse[], 
              double qfine[],
              double areacoarse[], 
              double areafine[],
              const int* igrid, 
              const int* manifold)
              {
                //do nothing
              };

        test_data.patch_vt->tag4refinement = tag4refinement;
        test_data.patch_vt->tag4coarsening = tag4coarsening;
        // don't want to test interpolation
        test_data.clawpatch_vt->d2->fort_interpolate2fine = fort_interpolate2fine;

        CHECK_EQ(test_data.glob->domain->global_num_patches, 16);
        test_data.setup();
        CHECK_EQ(test_data.glob->domain->global_num_patches, 31);
        domain = fclaw_domain_new_unitsquare(sc_MPI_COMM_WORLD, minlevel);

        //create output domain with bigger size, so that we can see ghost cells
        //in the vtk output
        fclaw_domain_t* domain_out = fclaw_domain_new_unitsquare(sc_MPI_COMM_WORLD, minlevel);
        TestData test_data_out(domain_out, map, minlevel, maxlevel);

        test_data_out.opts->mx   = mx+2*mbc;
        test_data_out.opts->my   = my+2*mbc;
        test_data_out.opts->mbc      = mbc;
        test_data_out.opts->meqn     = 3;

        test_data_out.patch_vt->tag4refinement = tag4refinement;
        test_data_out.patch_vt->tag4coarsening = tag4coarsening;
        // don't want to test interpolation
        test_data_out.clawpatch_vt->d2->fort_interpolate2fine = fort_interpolate2fine;

        CHECK_EQ(test_data_out.glob->domain->global_num_patches, 16);
        test_data_out.setup();
        CHECK_EQ(test_data_out.glob->domain->global_num_patches, 31);

        char test_no_str[5];
        snprintf(test_no_str, 5, "%04d", test_no);
        std::string filename = "2d_clawpatch_ghost_fill_on_square_with_refinement_"+std::string(test_no_str);

        test_ghost_fill(test_data, test_data_out, filename);

        test_no++;
    }

}
TEST_CASE("2d clawpatch ghost fill on square with refinement coarse interior")
{
    int test_no= 0;
    for(int mx   : {8})
    for(int my   : {8})
    for(int mbc  : {2})
    {
        int minlevel = 2;
        int maxlevel = 3;
        fclaw_domain_t* domain = fclaw_domain_new_unitsquare(sc_MPI_COMM_WORLD, minlevel);
        fclaw_map_context_t* map = fclaw_map_new_nomap();
        TestData test_data(domain, map, minlevel, maxlevel);

        test_data.opts->mx   = mx;
        test_data.opts->my   = my;
        test_data.opts->mbc      = mbc;
        test_data.opts->meqn     = 3;

        //refine on if interior patch
        auto tag4refinement 
          = [](fclaw_global_t* glob,
               fclaw_patch_t * patch,
               int blockno,
               int patchno,
               int initflag)
        {
            if(patch->xlower == 0 ||
                patch->ylower == 0 ||
                patch->xupper == 1 ||
                patch->yupper == 1)
            {
                return 1;
            }
            else 
            {
                return 0;
            }
        };
        auto tag4coarsening
          = [](fclaw_global_t* glob,
               fclaw_patch_t * patch,
               int blockno,
               int patchno,
               int initflag)
        {
            return 0;
        };
        auto fort_interpolate2fine
          =[](const int* mx, 
              const int* my, 
              const int* mbc, 
              const int* meqn,
              double qcoarse[], 
              double qfine[],
              double areacoarse[], 
              double areafine[],
              const int* igrid, 
              const int* manifold)
              {
                //do nothing
              };

        test_data.patch_vt->tag4refinement = tag4refinement;
        test_data.patch_vt->tag4coarsening = tag4coarsening;
        // don't want to test interpolation
        test_data.clawpatch_vt->d2->fort_interpolate2fine = fort_interpolate2fine;

        CHECK_EQ(test_data.glob->domain->global_num_patches, 16);
        test_data.setup();
        CHECK_EQ(test_data.glob->domain->global_num_patches, 52);

        //create output domain with bigger size, so that we can see ghost cells
        //in the vtk output
        fclaw_domain_t* domain_out = fclaw_domain_new_unitsquare(sc_MPI_COMM_WORLD, minlevel);
        TestData test_data_out(domain_out, map, minlevel, maxlevel);

        test_data_out.opts->mx   = mx+2*mbc;
        test_data_out.opts->my   = my+2*mbc;
        test_data_out.opts->mbc      = mbc;
        test_data_out.opts->meqn     = 3;

        test_data_out.patch_vt->tag4refinement = tag4refinement;
        test_data_out.patch_vt->tag4coarsening = tag4coarsening;
        // don't want to test interpolation
        test_data_out.clawpatch_vt->d2->fort_interpolate2fine = fort_interpolate2fine;

        CHECK_EQ(test_data_out.glob->domain->global_num_patches, 16);
        test_data_out.setup();
        CHECK_EQ(test_data_out.glob->domain->global_num_patches, 52);

        char test_no_str[5];
        snprintf(test_no_str, 5, "%04d", test_no);
        std::string filename = "2d_clawpatch_ghost_fill_on_square_with_refinement_coarse_interior_"+std::string(test_no_str);

        test_ghost_fill(test_data, test_data_out, filename);

        test_no++;

    }

}

TEST_CASE("2d clawpatch ghost fill on uniform brick")
{
    int test_no= 0;
    for(int mi   : {1,2,3})
    for(int mj   : {1,2,3})
    for(int mx   : {10})
    for(int my   : {10})
    for(int mbc  : {1,2})
    {
        int minlevel = 2;
        int maxlevel = 2;
        fclaw_domain_t* domain = fclaw_domain_new_2d_brick(sc_MPI_COMM_WORLD, mi, mj,  0, 0, minlevel);
        fclaw_map_context_t* brick = fclaw_map_new_2d_brick(domain, mi, mj,  0, 0);
        fclaw_map_context_t* map = fclaw_map_new_nomap_brick(brick);
        TestData test_data(domain, map, minlevel, maxlevel);

        test_data.fopts.mi = mi;
        test_data.fopts.mj = mj;
        test_data.opts->mx   = mx;
        test_data.opts->my   = my;
        test_data.opts->mbc      = mbc;
        test_data.opts->meqn     = 3;

        test_data.setup();

        //create output domain with bigger size, so that we can see ghost cells
        //in the vtk output
        fclaw_domain_t* domain_out = fclaw_domain_new_2d_brick(sc_MPI_COMM_WORLD, mi, mj, 0, 0, minlevel);
        TestData test_data_out(domain_out, map, minlevel, maxlevel);

        test_data_out.fopts.mi = mi;
        test_data_out.fopts.mj = mj;
        test_data_out.opts->mx   = mx+2*mbc;
        test_data_out.opts->my   = my+2*mbc;
        test_data_out.opts->mbc      = mbc;
        test_data_out.opts->meqn     = 3;

        test_data_out.setup();

        
        char test_no_str[5];
        snprintf(test_no_str, 5, "%04d", test_no);
        std::string filename = "2d_clawpatch_ghost_fill_on_uniform_2x2_brick_" + std::string(test_no_str);

        test_ghost_fill(test_data, test_data_out, filename);

        test_no++;

    }

}
TEST_CASE("2d clawpatch ghost fill on 2x2 brick with refinement on one block")
{
    int test_no= 0;
    for(int mx   : {8})
    for(int my   : {8})
    for(int mbc  : {2})
    for(int block_to_refine = 0; block_to_refine < 4; block_to_refine++)
    {
        int minlevel = 1;
        int maxlevel = 2;
        fclaw_domain_t* domain = fclaw_domain_new_2d_brick(sc_MPI_COMM_WORLD, 2, 2, 0, 0, minlevel);
        fclaw_map_context_t* brick = fclaw_map_new_2d_brick(domain, 2, 2, 0, 0);
        fclaw_map_context_t* map = fclaw_map_new_nomap_brick(brick);
        TestData test_data(domain, map, minlevel, maxlevel);

        test_data.fopts.mi = 2;
        test_data.fopts.mj = 2;
        test_data.opts->mx   = mx;
        test_data.opts->my   = my;
        test_data.opts->mbc      = mbc;
        test_data.opts->meqn     = 3;

        test_data.glob->user = &block_to_refine;

        //refine on if interior patch
        auto tag4refinement 
          = [](fclaw_global_t* glob,
               fclaw_patch_t * patch,
               int blockno,
               int patchno,
               int initflag)
        {
            int* block_to_refine = (int*)glob->user;
            return (int) (blockno == *block_to_refine);
        };
        auto tag4coarsening
          = [](fclaw_global_t* glob,
               fclaw_patch_t * patch,
               int blockno,
               int patchno,
               int initflag)
        {
            return 0;
        };
        auto fort_interpolate2fine
          =[](const int* mx, 
              const int* my, 
              const int* mbc, 
              const int* meqn,
              double qcoarse[], 
              double qfine[],
              double areacoarse[], 
              double areafine[],
              const int* igrid, 
              const int* manifold)
              {
                //do nothing
              };

        test_data.patch_vt->tag4refinement = tag4refinement;
        test_data.patch_vt->tag4coarsening = tag4coarsening;
        // don't want to test interpolation
        test_data.clawpatch_vt->d2->fort_interpolate2fine = fort_interpolate2fine;

        CHECK_EQ(test_data.glob->domain->global_num_patches, (4*4));
        test_data.setup();
        CHECK_EQ(test_data.glob->domain->global_num_patches, (4*3 + 4*4));
        domain = fclaw_domain_new_unitsquare(sc_MPI_COMM_WORLD, minlevel);

        //create output domain with bigger size, so that we can see ghost cells
        //in the vtk output
        fclaw_domain_t* domain_out = fclaw_domain_new_2d_brick(sc_MPI_COMM_WORLD, 2, 2, 0, 0, minlevel);
        TestData test_data_out(domain_out, map, minlevel, maxlevel);

        test_data_out.fopts.mi = 2;
        test_data_out.fopts.mj = 2;
        test_data_out.opts->mx   = mx+2*mbc;
        test_data_out.opts->my   = my+2*mbc;
        test_data_out.opts->mbc      = mbc;
        test_data_out.opts->meqn     = 3;

        test_data_out.glob->user = &block_to_refine;

        test_data_out.patch_vt->tag4refinement = tag4refinement;
        test_data_out.patch_vt->tag4coarsening = tag4coarsening;
        // don't want to test interpolation
        test_data_out.clawpatch_vt->d2->fort_interpolate2fine = fort_interpolate2fine;

        CHECK_EQ(test_data_out.glob->domain->global_num_patches, (4*4));
        test_data_out.setup();
        CHECK_EQ(test_data_out.glob->domain->global_num_patches, (4*3 + 4*4));

        char test_no_str[5];
        snprintf(test_no_str, 5, "%04d", test_no);
        std::string filename = "2d_clawpatch_ghost_fill_on_2x2_brick_with_refinement_on_one_block_"+std::string(test_no_str);

        test_ghost_fill(test_data, test_data_out, filename);

        test_no++;
    }

}

TEST_CASE("2d clawpatch ghost fill on 2x2 brick with refinement on all but one block")
{
    // This test will catch hanging edge case across block edge
    int test_no= 0;
    for(int mx   : {8})
    for(int my   : {8})
    for(int mbc  : {2})
    for(int block_to_leave_coarse = 0; block_to_leave_coarse < 4; block_to_leave_coarse++)
    {
        int minlevel = 1;
        int maxlevel = 2;
        fclaw_domain_t* domain = fclaw_domain_new_2d_brick(sc_MPI_COMM_WORLD, 2, 2, 0, 0, minlevel);
        fclaw_map_context_t* brick = fclaw_map_new_2d_brick(domain, 2, 2, 0, 0);
        fclaw_map_context_t* map = fclaw_map_new_nomap_brick(brick);
        TestData test_data(domain, map, minlevel, maxlevel);

        test_data.fopts.mi = 2;
        test_data.fopts.mj = 2;
        test_data.opts->mx   = mx;
        test_data.opts->my   = my;
        test_data.opts->mbc      = mbc;
        test_data.opts->meqn     = 3;

        test_data.glob->user = &block_to_leave_coarse;

        //refine on if interior patch
        auto tag4refinement 
          = [](fclaw_global_t* glob,
               fclaw_patch_t * patch,
               int blockno,
               int patchno,
               int initflag)
        {
            int* block_to_leave_coarse = (int*)glob->user;
            return (int) (blockno != *block_to_leave_coarse);
        };
        auto tag4coarsening
          = [](fclaw_global_t* glob,
               fclaw_patch_t * patch,
               int blockno,
               int patchno,
               int initflag)
        {
            return 0;
        };
        auto fort_interpolate2fine
          =[](const int* mx, 
              const int* my, 
              const int* mbc, 
              const int* meqn,
              double qcoarse[], 
              double qfine[],
              double areacoarse[], 
              double areafine[],
              const int* igrid, 
              const int* manifold)
              {
                //do nothing
              };

        test_data.patch_vt->tag4refinement = tag4refinement;
        test_data.patch_vt->tag4coarsening = tag4coarsening;
        // don't want to test interpolation
        test_data.clawpatch_vt->d2->fort_interpolate2fine = fort_interpolate2fine;

        CHECK_EQ(test_data.glob->domain->global_num_patches, 4*4);
        test_data.setup();
        CHECK_EQ(test_data.glob->domain->global_num_patches, 4*1 + 4*4*3);
        domain = fclaw_domain_new_unitsquare(sc_MPI_COMM_WORLD, minlevel);

        //create output domain with bigger size, so that we can see ghost cells
        //in the vtk output
        fclaw_domain_t* domain_out = fclaw_domain_new_2d_brick(sc_MPI_COMM_WORLD, 2, 2, 0, 0, minlevel);
        TestData test_data_out(domain_out, map, minlevel, maxlevel);

        test_data_out.fopts.mi = 2;
        test_data_out.fopts.mj = 2;
        test_data_out.opts->mx   = mx+2*mbc;
        test_data_out.opts->my   = my+2*mbc;
        test_data_out.opts->mbc      = mbc;
        test_data_out.opts->meqn     = 3;

        test_data_out.glob->user = &block_to_leave_coarse;

        test_data_out.patch_vt->tag4refinement = tag4refinement;
        test_data_out.patch_vt->tag4coarsening = tag4coarsening;
        // don't want to test interpolation
        test_data_out.clawpatch_vt->d2->fort_interpolate2fine = fort_interpolate2fine;

        CHECK_EQ(test_data_out.glob->domain->global_num_patches, 4*4);
        test_data_out.setup();
        CHECK_EQ(test_data_out.glob->domain->global_num_patches, 4*1 + 4*4*3);

        char test_no_str[5];
        snprintf(test_no_str, 5, "%04d", test_no);
        std::string filename = "2d_clawpatch_ghost_fill_on_2x2_brick_with_refinement_on_all_but_one_block_"+std::string(test_no_str);

        test_ghost_fill(test_data, test_data_out, filename);

        test_no++;
    }

}

TEST_CASE("2d clawpatch ghost fill on 2x2 brick with refinement coarse interior")
{
    // This test will catch hanging edge case across block faces also
    int test_no= 0;
    for(int mx   : {8})
    for(int my   : {8})
    for(int mbc  : {2})
    for(int block_to_leave_coarse = 0; block_to_leave_coarse < 4; block_to_leave_coarse++)
    {
        int minlevel = 1;
        int maxlevel = 2;
        fclaw_domain_t* domain = fclaw_domain_new_2d_brick(sc_MPI_COMM_WORLD, 2, 2, 0, 0, minlevel);
        fclaw_map_context_t* brick = fclaw_map_new_2d_brick(domain, 2, 2, 0, 0);
        fclaw_map_context_t* map = fclaw_map_new_nomap_brick(brick);
        TestData test_data(domain, map, minlevel, maxlevel);

        test_data.fopts.mi = 2;
        test_data.fopts.mj = 2;
        test_data.opts->mx   = mx;
        test_data.opts->my   = my;
        test_data.opts->mbc      = mbc;
        test_data.opts->meqn     = 3;

        test_data.glob->user = &block_to_leave_coarse;

        //refine on if interior patch
        auto tag4refinement 
          = [](fclaw_global_t* glob,
               fclaw_patch_t * patch,
               int blockno,
               int patchno,
               int initflag)
        {
            fclaw_clawpatch_t* cp = fclaw_clawpatch_get_clawpatch(patch);
            int* block_to_leave_coarse = (int*)glob->user;
            int retval = (blockno != *block_to_leave_coarse)
                ||!((cp->xlower == doctest::Approx(0.5) || cp->xupper == doctest::Approx(0.5))
                && (cp->ylower == doctest::Approx(0.5) || cp->yupper == doctest::Approx(0.5)));
            return retval;
        };
        auto tag4coarsening
          = [](fclaw_global_t* glob,
               fclaw_patch_t * patch,
               int blockno,
               int patchno,
               int initflag)
        {
            return 0;
        };
        auto fort_interpolate2fine
          =[](const int* mx, 
              const int* my, 
              const int* mbc, 
              const int* meqn,
              double qcoarse[], 
              double qfine[],
              double areacoarse[], 
              double areafine[],
              const int* igrid, 
              const int* manifold)
              {
                //do nothing
              };

        test_data.patch_vt->tag4refinement = tag4refinement;
        test_data.patch_vt->tag4coarsening = tag4coarsening;
        // don't want to test interpolation
        test_data.clawpatch_vt->d2->fort_interpolate2fine = fort_interpolate2fine;

        CHECK_EQ(test_data.glob->domain->global_num_patches, 4*4);
        test_data.setup();
        CHECK_EQ(test_data.glob->domain->global_num_patches, 61);
        domain = fclaw_domain_new_unitsquare(sc_MPI_COMM_WORLD, minlevel);

        //create output domain with bigger size, so that we can see ghost cells
        //in the vtk output
        fclaw_domain_t* domain_out = fclaw_domain_new_2d_brick(sc_MPI_COMM_WORLD, 2, 2, 0, 0, minlevel);
        TestData test_data_out(domain_out, map, minlevel, maxlevel);

        test_data_out.fopts.mi = 2;
        test_data_out.fopts.mj = 2;
        test_data_out.fopts.mk = 2;
        test_data_out.opts->mx   = mx+2*mbc;
        test_data_out.opts->my   = my+2*mbc;
        test_data_out.opts->mbc      = mbc;
        test_data_out.opts->meqn     = 3;

        test_data_out.glob->user = &block_to_leave_coarse;

        test_data_out.patch_vt->tag4refinement = tag4refinement;
        test_data_out.patch_vt->tag4coarsening = tag4coarsening;
        // don't want to test interpolation
        test_data_out.clawpatch_vt->d2->fort_interpolate2fine = fort_interpolate2fine;

        CHECK_EQ(test_data_out.glob->domain->global_num_patches, 4*4);
        test_data_out.setup();
        CHECK_EQ(test_data_out.glob->domain->global_num_patches, 61);

        char test_no_str[5];
        snprintf(test_no_str, 5, "%04d", test_no);
        std::string filename = "2d_clawpatch_ghost_fill_on_2x2_brick_with_refinement_coarse_interior_"+std::string(test_no_str);

        test_ghost_fill(test_data, test_data_out, filename);

        test_no++;
    }

}

TEST_CASE("2d clawpatch ghost fill on 2x2 brick with refinement 2")
{
    // This test will catch hanging edge case across block faces also
    int test_no= 0;
    for(int mx   : {8})
    for(int my   : {8})
    for(int mbc  : {2})
    for(int block_to_leave_coarse = 0; block_to_leave_coarse < 4; block_to_leave_coarse++)
    {
        int minlevel = 1;
        int maxlevel = 2;
        fclaw_domain_t* domain = fclaw_domain_new_2d_brick(sc_MPI_COMM_WORLD, 2, 2, 0, 0, minlevel);
        fclaw_map_context_t* brick = fclaw_map_new_2d_brick(domain, 2, 2, 0, 0);
        fclaw_map_context_t* map = fclaw_map_new_nomap_brick(brick);
        TestData test_data(domain, map, minlevel, maxlevel);

        test_data.fopts.mi = 2;
        test_data.fopts.mj = 2;
        test_data.opts->mx   = mx;
        test_data.opts->my   = my;
        test_data.opts->mbc      = mbc;
        test_data.opts->meqn     = 3;

        test_data.glob->user = &block_to_leave_coarse;

        //refine on if interior patch
        auto tag4refinement 
          = [](fclaw_global_t* glob,
               fclaw_patch_t * patch,
               int blockno,
               int patchno,
               int initflag)
        {
            fclaw_clawpatch_t* cp = fclaw_clawpatch_get_clawpatch(patch);
            int* block_to_leave_coarse = (int*)glob->user;
            int retval = (blockno != *block_to_leave_coarse)
                && (cp->xlower == doctest::Approx(0.5) || cp->xupper == doctest::Approx(0.5))
                && (cp->ylower == doctest::Approx(0.5) || cp->yupper == doctest::Approx(0.5));
            return retval;
        };
        auto tag4coarsening
          = [](fclaw_global_t* glob,
               fclaw_patch_t * patch,
               int blockno,
               int patchno,
               int initflag)
        {
            return 0;
        };
        auto fort_interpolate2fine
          =[](const int* mx, 
              const int* my, 
              const int* mbc, 
              const int* meqn,
              double qcoarse[], 
              double qfine[],
              double areacoarse[], 
              double areafine[],
              const int* igrid, 
              const int* manifold)
              {
                //do nothing
              };

        test_data.patch_vt->tag4refinement = tag4refinement;
        test_data.patch_vt->tag4coarsening = tag4coarsening;
        // don't want to test interpolation
        test_data.clawpatch_vt->d2->fort_interpolate2fine = fort_interpolate2fine;

        CHECK_EQ(test_data.glob->domain->global_num_patches, 16);
        test_data.setup();
        CHECK_EQ(test_data.glob->domain->global_num_patches, 25);
        domain = fclaw_domain_new_unitsquare(sc_MPI_COMM_WORLD, minlevel);

        //create output domain with bigger size, so that we can see ghost cells
        //in the vtk output
        fclaw_domain_t* domain_out = fclaw_domain_new_2d_brick(sc_MPI_COMM_WORLD, 2, 2, 0, 0, minlevel);
        TestData test_data_out(domain_out, map, minlevel, maxlevel);

        test_data_out.fopts.mi = 2;
        test_data_out.fopts.mj = 2;
        test_data_out.opts->mx   = mx+2*mbc;
        test_data_out.opts->my   = my+2*mbc;
        test_data_out.opts->mbc      = mbc;
        test_data_out.opts->meqn     = 3;

        test_data_out.glob->user = &block_to_leave_coarse;

        test_data_out.patch_vt->tag4refinement = tag4refinement;
        test_data_out.patch_vt->tag4coarsening = tag4coarsening;
        // don't want to test interpolation
        test_data_out.clawpatch_vt->d2->fort_interpolate2fine = fort_interpolate2fine;

        CHECK_EQ(test_data_out.glob->domain->global_num_patches, 16);
        test_data_out.setup();
        CHECK_EQ(test_data_out.glob->domain->global_num_patches, 25);

        char test_no_str[5];
        snprintf(test_no_str, 5, "%04d", test_no);
        std::string filename = "2d_clawpatch_ghost_fill_on_2x2_brick_with_refinement_2_"+std::string(test_no_str);

        test_ghost_fill(test_data, test_data_out, filename);

        test_no++;
    }

}