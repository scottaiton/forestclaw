/*
  Copyright (c) 2012-2023 Carsten Burstedde, Donna Calhoun, Scott Aiton
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

#include "../all/advection_user.h"
#include <p4est_wrap.h>         /* just for testing */
#include <fclaw2d_convenience.h>

/* mark all patches for refinement */
static void
mark_refine_uniform (fclaw2d_domain_t * domain, fclaw2d_patch_t * patch,
                     int blockno, int patchno, void *user)
{
    fclaw2d_patch_mark_refine (domain, blockno, patchno);
}

/* mark all patches on rank 1 and 3 for refinement */
static void
mark_refine_rank (fclaw2d_domain_t * domain, fclaw2d_patch_t * patch,
                  int blockno, int patchno, void *user)
{
    /* refine only some processes to ensure repartitioning */
    if (domain->mpirank == 1 || domain->mpirank == 3)
    {
        fclaw2d_patch_mark_refine (domain, blockno, patchno);
    }
}

static void
alloc_patch_data (fclaw2d_domain_t * domain, fclaw2d_patch_t * patch,
                  int blockno, int patchno, void *user)
{
    patch->user = FCLAW_ALLOC (double, 1);
    double *patch_data = (double *) patch->user;
    *patch_data = (double) -1;
}

static void
set_patch_data (fclaw2d_domain_t * domain, fclaw2d_patch_t * patch,
                int blockno, int patchno, void *user)
{
    /* set artificial patch data that allows tracking rank of origin and treeid */
    double *patch_data = (double *) patch->user;
    *patch_data = (double) domain->mpirank * 1000 + blockno * 100 + patchno;
};

static int num_patches_packed;

static void
pack_patch_data (fclaw2d_domain_t * domain, fclaw2d_patch_t * patch,
                 int blockno, int patchno, void *pack_data_here, void *user)
{
    FCLAW_ASSERT (patch != NULL);
    num_patches_packed++;
    double *pack_double_here = (double *) pack_data_here;
    double *patch_data = (double *) patch->user;

    *pack_double_here = *patch_data;
}

static void
transfer_patch_data (fclaw2d_domain_t * old_domain,
                     fclaw2d_patch_t * old_patch,
                     fclaw2d_domain_t * new_domain,
                     fclaw2d_patch_t * new_patch, int blockno,
                     int old_patchno, int new_patchno, void *user)
{
    double *old_patch_data = (double *) old_patch->user;
    double *new_patch_data = (double *) new_patch->user;

    /* simply copy patch data from old to new location */
    *new_patch_data = *old_patch_data;
}

static void
unpack_patch_data (fclaw2d_domain_t * domain, fclaw2d_patch_t * patch,
                   int blockno, int patchno, void *unpack_data_from_here,
                   void *user)
{
    double *unpack_double_from_here = (double *) unpack_data_from_here;
    double *patch_data = (double *) patch->user;

    *patch_data = *unpack_double_from_here;
}

static void
print_patch_data (fclaw2d_domain_t * domain, fclaw2d_patch_t * patch,
                  int blockno, int patchno, void *user)
{
    double *patch_data = (double *) patch->user;
    if (patch_data == NULL)
    {
        return;
    }
    fclaw_infof ("Patch %d has patch data %f.\n", patchno, *patch_data);
};

static void
delete_patch_data (fclaw2d_domain_t * domain, fclaw2d_patch_t * patch,
                   int blockno, int patchno, void *user)
{
    FCLAW_FREE (patch->user);
};

int
main (int argc, char **argv)
{
    /* This demo highlights the effects of the different partitioning options
     * on the patch data partitioning. In particular, they differ in the amount
     * of patch_pack callback calls that have to be performed. */

    /* Initialize application */
    fclaw_app_t *app = fclaw_app_new (&argc, &argv, NULL);

    /* Run the program */
    /* Create global structure which stores the domain, timers, etc */
    int size, rank;
    sc_MPI_Comm mpicomm = fclaw_app_get_mpi_size_rank (app, &size, &rank);
    fclaw_global_t *glob = fclaw_global_new_comm (mpicomm, size, rank);

    fclaw2d_domain_t *domain, *refined_domain, *partitioned_domain;

    /* iterate through the different partitioning strategies for patch data */
    int output_patch_data = 0;
    for (int test_case = 0; test_case < 2; test_case++)
    {
        domain = fclaw2d_domain_new_brick (mpicomm, 2, 2, 0, 0, 2);

        /* set partitioning options */
        fclaw2d_domain_set_partitioning (domain, 1, (test_case % 2),
                                         (test_case / 2));

        if (domain->mpisize != 1)
        {
            /* refine all patches uniformly, to ensure the skip_refined option can
             * distinguish the most recent refinement from other refinement */
            fclaw2d_domain_iterate_patches (domain, mark_refine_uniform, NULL);
            refined_domain = fclaw2d_domain_adapt (domain);
            fclaw2d_domain_destroy (domain);

            /* partition the resulting domain */
            partitioned_domain = fclaw2d_domain_partition (refined_domain, 0);
            if (partitioned_domain != NULL) {
                fclaw2d_domain_destroy (refined_domain);
                fclaw2d_domain_complete (partitioned_domain);
                domain = partitioned_domain;
            } else {
                domain = refined_domain;
            }
            refined_domain = partitioned_domain = NULL;

            /* refine only on a few processes to ensure repartitioning */
            fclaw2d_domain_iterate_patches (domain, mark_refine_rank, NULL);
            refined_domain = fclaw2d_domain_adapt (domain);

            fclaw2d_domain_iterate_patches (refined_domain, alloc_patch_data,
                                            NULL);
            fclaw2d_domain_iterate_patches (refined_domain, set_patch_data,
                                            NULL);
            if (output_patch_data)
            {
                fclaw2d_domain_iterate_patches (refined_domain,
                                                print_patch_data, NULL);
            }
            partitioned_domain = fclaw2d_domain_partition (refined_domain, 0);

            fclaw_global_productionf
                ("Starting partitioning with skip_local = %d.\n",
                 domain->p.skip_local);
            fclaw2d_domain_iterate_patches (partitioned_domain,
                                            alloc_patch_data, NULL);

            num_patches_packed = 0;
            fclaw2d_domain_partition_t *p;
            p = fclaw2d_domain_iterate_pack (refined_domain, sizeof (double),
                                             pack_patch_data, NULL);
            fclaw_infof ("Packed %d of %d local patches.\n",
                         num_patches_packed,
                         refined_domain->local_num_patches);

            fclaw2d_domain_iterate_unpack (partitioned_domain, p,
                                           unpack_patch_data, NULL);
            fclaw2d_domain_iterate_transfer (refined_domain,
                                             partitioned_domain,
                                             transfer_patch_data, NULL);
            fclaw2d_domain_partition_free (p);

            if (output_patch_data)
            {
                fclaw2d_domain_iterate_patches (partitioned_domain,
                                                print_patch_data, NULL);
            }

            fclaw2d_domain_complete (partitioned_domain);

            fclaw2d_domain_iterate_patches (refined_domain, delete_patch_data,
                                            NULL);
            fclaw2d_domain_iterate_patches (partitioned_domain,
                                            delete_patch_data, NULL);

            fclaw2d_domain_destroy (partitioned_domain);
            fclaw2d_domain_destroy (refined_domain);
        }

        fclaw2d_domain_destroy (domain);
    }
    fclaw_global_destroy (glob);

    fclaw_app_destroy (app);

    return 0;
}
