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

typedef struct partitioning_patch_data
{
    double xlower, xupper;
    double area;
    int level;
    int index;
}
patch_data_t;

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
    patch->user = FCLAW_ALLOC_ZERO (patch_data_t, 1);
}

static void
compute_patch_data (fclaw2d_domain_t * domain, fclaw2d_patch_t * patch,
                    int blockno, int patchno, patch_data_t * patch_data)
{
    /* compute artificial patch data that allows tracking rank and treeid */
    patch_data->xlower = patch->xlower;
    patch_data->xupper = patch->xupper;
    patch_data->area = (patch->xupper - patch->xlower) *
        (patch->ylower - patch->ylower);
    FCLAW_ASSERT (patch_data->area >= 0.);
    patch_data->level = patch->level;
    FCLAW_ASSERT (patch_data->level >= 0
                  && patch_data->level < P4EST_QMAXLEVEL);
    patch_data->index = domain->mpirank * 1000 + blockno * 100 + patchno;
}

static void
compute_child_patch_data (patch_data_t * parent_data,
                          patch_data_t * child_data, int childid)
{
    double xshift = (parent_data->xupper - parent_data->xlower) / 2.;
    child_data->xlower = parent_data->xlower + (childid % 2) ? xshift : 0.;
    child_data->xupper = parent_data->xupper - (childid % 2) ? 0 : xshift;
    child_data->area = parent_data->area / (double) P4EST_CHILDREN;
    child_data->level = parent_data->level + 1;

    /* we assume either all or no patches were refined,
     * so child_patchno = parent_patchno * 4 + childid */
    int parent_patchno = parent_data->index % 100;
    child_data->index = parent_data->index + parent_patchno * 3 + childid;
}

static void
set_patch_data (fclaw2d_domain_t * domain, fclaw2d_patch_t * patch,
                int blockno, int patchno, void *user)
{
    compute_patch_data (domain, patch, blockno, patchno,
                        (patch_data_t *) patch->user);
};

static int num_patches_packed;

static void
pack_patch_data (fclaw2d_domain_t * domain, fclaw2d_patch_t * patch,
                 int blockno, int patchno, void *pack_data_here, void *user)
{
    FCLAW_ASSERT (patch != NULL);
    num_patches_packed++;       /* keep track of number of callback calls */

    /* copy the pack_data into contiguous memory */
    char *pdh = (char *) pack_data_here;
    patch_data_t *patch_data = (patch_data_t *) patch->user;
    memcpy (pdh, &patch_data->xlower, sizeof (double));
    memcpy (pdh + sizeof (double), &patch_data->xupper, sizeof (double));
    memcpy (pdh + 2 * sizeof (double), &patch_data->area, sizeof (double));
    memcpy (pdh + 3 * sizeof (double), &patch_data->level, sizeof (int));
    memcpy (pdh + 3 * sizeof (double) + sizeof (int), &patch_data->index,
            sizeof (int));
}

static void
transfer_patch_data (fclaw2d_domain_t * old_domain,
                     fclaw2d_patch_t * old_patch,
                     fclaw2d_domain_t * new_domain,
                     fclaw2d_patch_t * new_patch, int blockno,
                     int old_patchno, int new_patchno, void *user)
{
    patch_data_t *old_patch_data = (patch_data_t *) old_patch->user;
    patch_data_t *new_patch_data = (patch_data_t *) new_patch->user;

    /* simply copy patch data from old to new location */
    if (old_patch->level == old_patch_data->level)
    {
        memcpy (new_patch_data, old_patch_data, sizeof (patch_data_t));
    }
    else
    {
        /* we reference the parent's patch data in the skip_refined case */
        FCLAW_ASSERT (old_patch->level == old_patch_data->level + 1);
        FCLAW_ASSERT (new_domain->p.skip_refined);
        compute_child_patch_data (old_patch_data, new_patch_data,
                                  fclaw2d_patch_childid (new_patch));
    }
}

static void
unpack_patch_data (fclaw2d_domain_t * domain, fclaw2d_patch_t * patch,
                   int blockno, int patchno, void *unpack_data_from_here,
                   void *user)
{
    char *udfh = (char *) unpack_data_from_here;
    patch_data_t *patch_data = (patch_data_t *) patch->user;
    memcpy (&patch_data->xlower, udfh, sizeof (double));
    memcpy (&patch_data->xupper, udfh + sizeof (double), sizeof (double));
    memcpy (&patch_data->area, udfh + 2 * sizeof (double), sizeof (double));
    memcpy (&patch_data->level, udfh + 3 * sizeof (double), sizeof (int));
    memcpy (&patch_data->index, udfh + 3 * sizeof (double) + sizeof (int),
            sizeof (int));
    if (patch->level != patch_data->level)
    {
        /* we reference the parent's patch data in the skip_refined case */
        FCLAW_ASSERT (patch->level == patch_data->level + 1);
        FCLAW_ASSERT (domain->p.skip_refined);

        /* compute child patch data from unpacked parent patch data */
        patch_data_t *child_patch_data = FCLAW_ALLOC_ZERO (patch_data_t, 1);
        compute_child_patch_data (patch_data, child_patch_data,
                                  fclaw2d_patch_childid (patch));

        /* store child patch data in the patch's user data */
        memcpy (patch_data, child_patch_data, sizeof (patch_data_t));
        FCLAW_FREE (child_patch_data);
    }
}

static void
print_patch_data (fclaw2d_domain_t * domain, fclaw2d_patch_t * patch,
                  int blockno, int patchno, void *user)
{
    patch_data_t *patch_data = (patch_data_t *) patch->user;
    if (patch_data == NULL)
    {
        return;
    }
    fclaw_infof ("Patch %d has patch data index %d.\n", patchno,
                 patch_data->index);
};

static void
compute_checksum (fclaw2d_domain_t * domain, fclaw2d_patch_t * patch,
                  int blockno, int patchno, void *user)
{
    patch_data_t *patch_data = (patch_data_t *) patch->user;
    int *checksum = (int *) user;
    *checksum += patch_data->index;
};

static void
delete_patch_data (fclaw2d_domain_t * domain, fclaw2d_patch_t * patch,
                   int blockno, int patchno, void *user)
{
    FCLAW_FREE (patch->user);
};

static void
init_patch_data_skip_refined (fclaw2d_domain_t * old_domain,
                              fclaw2d_patch_t * old_patch,
                              fclaw2d_domain_t * new_domain,
                              fclaw2d_patch_t * new_patch,
                              fclaw2d_patch_relation_t newsize,
                              int blockno, int old_patchno, int new_patchno,
                              void *user)
{
    if (newsize == FCLAW2D_PATCH_HALFSIZE)
    {
        /* the old patch was recently refined; reference parents patch data */
        for (int i = 0; i < P4EST_CHILDREN; i++)
        {
            new_patch[i].user = old_patch[0].user;
        }
    }
    else
    {
        /* same size patch, as we do not coarsen in this demo */
        FCLAW_ASSERT (newsize == FCLAW2D_PATCH_SAMESIZE);
        /* copy patch data */
        new_patch[0].user = FCLAW_ALLOC (patch_data_t, 1);
        memcpy (new_patch[0].user, old_patch[0].user, sizeof (patch_data_t));
    }
}

static void
delete_patch_data_skip_refined (fclaw2d_domain_t * domain,
                                fclaw2d_patch_t * patch, int blockno,
                                int patchno, void *user)
{
    patch_data_t *patch_data = (patch_data_t *) patch->user;
    if (patch_data->level == patch->level)
    {
        FCLAW_FREE (patch->user);
    }
    else
    {
        /* we referenced the patch data of the parent patch */
        FCLAW_ASSERT (patch_data->level + 1 == patch->level);
    }
}

int
main (int argc, char **argv)
{
    /* This demo highlights the effects of the different partitioning options
     * on the patch data partitioning. In particular, they differ in the amount
     * of patch_pack callback calls that have to be performed. */

    /* Initialize application */
    fclaw_app_t *app = fclaw_app_new (&argc, &argv, NULL);
    sc_package_set_verbosity (fclaw_get_package_id (), FCLAW_VERBOSITY_INFO);

    /* Run the program */
    /* Create global structure which stores the domain, timers, etc */
    int size, rank;
    sc_MPI_Comm mpicomm = fclaw_app_get_mpi_size_rank (app, &size, &rank);
    fclaw_global_t *glob = fclaw_global_new_comm (mpicomm, size, rank);

    fclaw2d_domain_t *domain, *refined_domain, *partitioned_domain;
    p4est_wrap_t *wrap;

    /* iterate through the different partitioning strategies for patch data */
    int output_patch_data = 0;
    int checksums[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    for (int test_case = 0; test_case < 9; test_case++)
    {
        domain = fclaw2d_domain_new_brick (mpicomm, 2, 2, 0, 0, 1);
        wrap = (p4est_wrap_t *) domain->pp;

        /* set partitioning options */
        if (test_case < 8)
        {
            fclaw2d_domain_set_partitioning (domain, !(test_case / 4),
                                             (test_case % 2),
                                             (test_case % 4 / 2));

        }
        else
        {
            /* check case that skip_refined is enabled to late */
            FCLAW_ASSERT (test_case == 8);
            fclaw2d_domain_set_partitioning (domain, 0, 1, 0);
        }

        if (domain->mpisize != 1)
        {
            /* refine all patches uniformly, to ensure the skip_refined option can
             * distinguish the most recent refinement from other refinement */
            fclaw2d_domain_iterate_patches (domain, mark_refine_uniform,
                                            NULL);
            refined_domain = fclaw2d_domain_adapt (domain);
            fclaw2d_domain_destroy (domain);

            /* partition the resulting domain */
            partitioned_domain = fclaw2d_domain_partition (refined_domain, 0);
            if (partitioned_domain != NULL)
            {
                fclaw2d_domain_destroy (refined_domain);
                fclaw2d_domain_complete (partitioned_domain);
                domain = partitioned_domain;
            }
            else
            {
                domain = refined_domain;
            }
            refined_domain = partitioned_domain = NULL;

            /* set patch data for domain */
            fclaw2d_domain_iterate_patches (domain, alloc_patch_data, NULL);
            fclaw2d_domain_iterate_patches (domain, set_patch_data, NULL);

            /* refine only on a few processes to ensure repartitioning */
            fclaw2d_domain_iterate_patches (domain, mark_refine_rank, NULL);
            refined_domain = fclaw2d_domain_adapt (domain);

            /* set patch data for refined domain */
            if (!refined_domain->p.skip_refined)
            {
                fclaw2d_domain_iterate_patches (refined_domain,
                                                alloc_patch_data, NULL);
                fclaw2d_domain_iterate_patches (refined_domain,
                                                set_patch_data, domain);
            }
            else
            {
                /* delay computing patch data for recently refined patches */
                fclaw2d_domain_iterate_adapted (domain, refined_domain,
                                                init_patch_data_skip_refined,
                                                NULL);
            }

            if (output_patch_data)
            {
                fclaw2d_domain_iterate_patches (refined_domain,
                                                print_patch_data, NULL);
            }
            partitioned_domain = fclaw2d_domain_partition (refined_domain, 0);

            if (test_case < 8)
            {
                fclaw_global_productionf
                    ("Starting partitioning with skip_local = %d,"
                     " skip_refined = %d and partition_for_coarsening = %d.\n",
                     domain->p.skip_local, domain->p.skip_refined,
                     wrap->params.partition_for_coarsening);
            }
            else
            {
                fclaw_global_productionf
                    ("Starting partitioning with skip_local = %d,"
                     " partition_for_coarsening = %d and"
                     "skip_refined being enabled to late.\n",
                     domain->p.skip_local,
                     wrap->params.partition_for_coarsening);
                fclaw2d_domain_set_partitioning (domain, 0, 1, 1);
            }

            /* partitioned domain will have proper patch data everywhere
             * (unlike refined domain for skip_refined == 1) */
            fclaw2d_domain_iterate_patches (partitioned_domain,
                                            alloc_patch_data, NULL);

            num_patches_packed = 0;
            fclaw2d_domain_partition_t *p;
            p = fclaw2d_domain_iterate_pack (refined_domain,
                                             3 * sizeof (double) +
                                             2 * sizeof (int),
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

            /* output patch data if required */
            if (output_patch_data)
            {
                fclaw2d_domain_iterate_patches (partitioned_domain,
                                                print_patch_data, NULL);
            }

            /* compute and compare checksums */
            fclaw2d_domain_iterate_patches (partitioned_domain,
                                            compute_checksum,
                                            &checksums[test_case]);
            /* checksum should only be affected by partition_for_coarsening */
            FCLAW_ASSERT ((test_case % 4) == 0 ||
                          checksums[test_case - 1] == checksums[test_case]);

            fclaw2d_domain_complete (partitioned_domain);

            /* destroy refined_domain patch_data first, because it contains
             * references to the initial domain */
            fclaw2d_domain_iterate_patches (refined_domain,
                                            refined_domain->p.skip_refined ?
                                            delete_patch_data_skip_refined :
                                            delete_patch_data, NULL);
            fclaw2d_domain_iterate_patches (domain, delete_patch_data, NULL);
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
