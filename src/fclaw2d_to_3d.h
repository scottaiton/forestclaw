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

#ifndef FCLAW2D_TO_3D_H
#define FCLAW2D_TO_3D_H

#ifdef FORESTCLAW2D_H
#error "The include files forestclaw2d.h and fclaw2d_to_3d.h cannot be combined"
#endif

#include <p4est_to_p8est.h>

/* redefine macros */
#define FCLAW2D_SPACEDIM                FCLAW3D_SPACEDIM
#define FCLAW2D_NUMFACES                FCLAW3D_NUMFACES
#define FCLAW2D_NUMCORNERS              FCLAW3D_NUMCORNERS
#define FCLAW2D_NUMSIBLINGS             FCLAW3D_NUMSIBLINGS
#define FCLAW2D_NUMFACENEIGHBORS        FCLAW3D_NUMFACENEIGHBORS
/* not redefining REFINEFACTOR, which should be dimension-independent */

/* redefine typedefs */
#define fclaw2d_patch_flags_t           fclaw3d_patch_flags_t
#define fclaw2d_patch_t                 fclaw3d_patch_t
#define fclaw2d_patch_vtable_t          fclaw3d_patch_vtable_t
#define fclaw2d_patch_transform_data_t  fclaw3d_patch_transform_data_t
#define fclaw2d_patch_data_t            fclaw3d_patch_data_t
#define fclaw2d_block_t                 fclaw3d_block_t
#define fclaw2d_domain_t                fclaw3d_domain_t
#define fclaw2d_domain_data_t           fclaw3d_domain_data_t
#define fclaw2d_domain_persist_t        fclaw3d_domain_persist_t
#define fclaw2d_timer_names_t           fclaw3d_timer_names_t
#define fclaw2d_patch_callback_t        fclaw3d_patch_callback_t
#define fclaw2d_patch_relation_t        fclaw3d_patch_relation_t
#define fclaw2d_match_callback_t        fclaw3d_match_callback_t
#define fclaw2d_transfer_callback_t     fclaw3d_transfer_callback_t
#define fclaw2d_domain_exchange_t       fclaw3d_domain_exchange_t
#define fclaw2d_integrate_ray_t         fclaw3d_integrate_ray_t
#define fclaw2d_interpolate_point_t     fclaw3d_interpolate_point_t
#define fclaw2d_build_mode_t            fclaw3d_build_mode_t
#define fclaw2d_patch_new_t             fclaw3d_patch_new_t
#define fclaw2d_patch_delete_t          fclaw3d_patch_delete_t
#define fclaw2d_patch_build_t           fclaw3d_patch_build_t
#define fclaw2d_patch_build_from_fine_t fclaw3d_patch_build_from_fine_t
#define fclaw2d_patch_setup_t           fclaw3d_patch_setup_t
#define fclaw2d_patch_initialize_t      fclaw3d_patch_initialize_t
#define fclaw2d_patch_physical_bc_t     fclaw3d_patch_physical_bc_t
#define fclaw2d_patch_single_step_update_t fclaw3d_patch_single_step_update_t
#define fclaw2d_patch_rhs_t             fclaw3d_patch_rhs_t
#define fclaw2d_patch_setup_timeinterp_t fclaw3d_patch_setup_timeinterp_t
#define fclaw2d_patch_restore_step_t    fclaw3d_patch_restore_step_t
#define fclaw2d_patch_save_step_t       fclaw3d_patch_save_step_t
#define fclaw2d_patch_copy_face_t       fclaw3d_patch_copy_face_t
#define fclaw2d_patch_average_face_t    fclaw3d_patch_average_face_t
#define fclaw2d_patch_interpolate_face_t fclaw3d_patch_interpolate_face_t
#define fclaw2d_patch_copy_corner_t     fclaw3d_patch_copy_corner_t
#define fclaw2d_patch_average_corner_t  fclaw3d_patch_average_corner_t
#define fclaw2d_patch_interpolate_corner_t fclaw3d_patch_interpolate_corner_t
#define fclaw2d_patch_transform_init_data_t fclaw3d_patch_transform_init_data_t
#define fclaw2d_patch_transform_blockface_t fclaw3d_patch_transform_blockface_t
#define fclaw2d_patch_transform_blockface_intra_t fclaw3d_patch_transform_blockface_intra_t
#define fclaw2d_patch_tag4refinement_t  fclaw3d_patch_tag4refinement_t
#define fclaw2d_patch_tag4coarsening_t  fclaw3d_patch_tag4coarsening_t
#define fclaw2d_patch_interpolate2fine_t fclaw3d_patch_interpolate2fine_t
#define fclaw2d_patch_average2coarse_t  fclaw3d_patch_average2coarse_t
#define fclaw2d_patch_ghost_packsize_t  fclaw3d_patch_ghost_packsize_t
#define fclaw2d_patch_local_ghost_pack_t fclaw3d_patch_local_ghost_pack_t
#define fclaw2d_patch_local_ghost_alloc_t fclaw3d_patch_local_ghost_alloc_t
#define fclaw2d_patch_local_ghost_free_t fclaw3d_patch_local_ghost_free_t
#define fclaw2d_patch_remote_ghost_build_t fclaw3d_patch_remote_ghost_build_t
#define fclaw2d_patch_remote_ghost_setup_t fclaw3d_patch_remote_ghost_setup_t
#define fclaw2d_patch_remote_ghost_unpack_t fclaw3d_patch_remote_ghost_unpack_t
#define fclaw2d_patch_remote_ghost_delete_t fclaw3d_patch_remote_ghost_delete_t
#define fclaw2d_patch_partition_packsize_t fclaw3d_patch_partition_packsize_t
#define fclaw2d_patch_partition_pack_t  fclaw3d_patch_partition_pack_t
#define fclaw2d_patch_partition_unpack_t fclaw3d_patch_partition_unpack_t
#define fclaw2d_patch_time_sync_f2c_t   fclaw3d_patch_time_sync_f2c_t
#define fclaw2d_patch_time_sync_samesize_t fclaw3d_patch_time_sync_samesize_t
#define fclaw2d_patch_time_sync_reset_t fclaw3d_patch_time_sync_reset_t
#define fclaw2d_patch_create_user_data_t fclaw3d_patch_create_user_data_t
#define fclaw2d_patch_destroy_user_data_t fclaw3d_patch_destroy_user_data_t
#define fclaw2d_patch_metric_patch_t    fclaw3d_patch_metric_patch_t

/* redefine enums */
#define FCLAW2D_PATCH_CHILDID           FCLAW3D_PATCH_CHILDID
#define FCLAW2D_PATCH_FIRST_SIBLING     FCLAW3D_PATCH_FIRST_SIBLING
#define FCLAW2D_PATCH_ON_PARALLEL_BOUNDARY  FCLAW3D_PATCH_ON_PARALLEL_BOUNDARY
#define FCLAW2D_PATCH_IS_GHOST          FCLAW3D_PATCH_IS_GHOST
#define FCLAW2D_PATCH_ON_BLOCK_FACE_0   FCLAW3D_PATCH_ON_BLOCK_FACE_0
#define FCLAW2D_PATCH_ON_BLOCK_FACE_1   FCLAW3D_PATCH_ON_BLOCK_FACE_1
#define FCLAW2D_PATCH_ON_BLOCK_FACE_2   FCLAW3D_PATCH_ON_BLOCK_FACE_2
#define FCLAW2D_PATCH_ON_BLOCK_FACE_3   FCLAW3D_PATCH_ON_BLOCK_FACE_3
#define FCLAW2D_PATCH_BOUNDARY          FCLAW3D_PATCH_BOUNDARY
#define FCLAW2D_PATCH_HALFSIZE          FCLAW3D_PATCH_HALFSIZE
#define FCLAW2D_PATCH_SAMESIZE          FCLAW3D_PATCH_SAMESIZE
#define FCLAW2D_PATCH_DOUBLESIZE        FCLAW3D_PATCH_DOUBLESIZE
#define FCLAW2D_BUILD_FOR_GHOST_AREA_COMPUTED FCLAW3D_BUILD_FOR_GHOST_AREA_COMPUTED
#define FCLAW2D_BUILD_FOR_GHOST_AREA_PACKED FCLAW3D_BUILD_FOR_GHOST_AREA_PACKED
#define FCLAW2D_BUILD_FOR_UPDATE        FCLAW3D_BUILD_FOR_UPDATE
#define FCLAW2D_BUILD_CUSTOM            FCLAW3D_BUILD_CUSTOM

/* redefine variables */
#define fclaw2d_patch_block_face_flags  fclaw3d_patch_block_face_flags
#define fclaw2d_smallest_h              fclaw3d_smallest_h

/* redefine functions */
#define fclaw2d_domain_global_maximum   fclaw3d_domain_global_maximum
#define fclaw2d_domain_global_sum       fclaw3d_domain_global_sum
#define fclaw2d_domain_barrier          fclaw3d_domain_barrier
#define fclaw2d_domain_dimension        fclaw3d_domain_dimension
#define fclaw2d_check_initial_level     fclaw3d_check_initial_level
#define fclaw2d_domain_new_unitsquare   fclaw3d_domain_new_unitcube
#define fclaw2d_domain_new_brick        fclaw3d_domain_new_brick
#define fclaw2d_domain_new_conn         fclaw3d_domain_new_conn
#define fclaw2d_domain_num_faces        fclaw3d_domain_num_faces
#define fclaw2d_domain_num_corners      fclaw3d_domain_num_corners
#define fclaw2d_domain_num_face_corners     fclaw3d_domain_num_face_corners
#define fclaw2d_domain_num_orientations fclaw3d_num_orientations
#define fclaw2d_domain_corner_faces     fclaw3d_domain_corner_faces
#define fclaw2d_patch_corner_dimension  fclaw3d_patch_corner_dimension
#define fclaw2d_patch_childid           fclaw3d_patch_childid
#define fclaw2d_patch_is_first_sibling  fclaw3d_patch_is_first_sibling
#define fclaw2d_patch_is_ghost          fclaw3d_patch_is_ghost
#define fclaw2d_patch_reset_data        fclaw3d_patch_reset_data
#define fclaw2d_patch_data_delete       fclaw3d_patch_data_delete
#define fclaw2d_patch_build             fclaw3d_patch_build
#define fclaw2d_patch_build_from_fine   fclaw3d_patch_build_from_fine
#define fclaw2d_patch_initialize        fclaw3d_patch_initialize
#define fclaw2d_patch_physical_bc       fclaw3d_patch_physical_bc
#define fclaw2d_patch_single_step_update fclaw3d_patch_single_step_update
#define fclaw2d_patch_set_rhs           fclaw3d_patch_set_rhs
#define fclaw2d_patch_restore_step      fclaw3d_patch_restore_step
#define fclaw2d_patch_save_step         fclaw3d_patch_save_step
#define fclaw2d_patch_setup_timeinterp  fclaw3d_patch_setup_timeinterp
#define fclaw2d_patch_copy_face         fclaw3d_patch_copy_face
#define fclaw2d_patch_average_face      fclaw3d_patch_average_face
#define fclaw2d_patch_interpolate_face  fclaw3d_patch_interpolate_face
#define fclaw2d_patch_copy_corner       fclaw3d_patch_copy_corner
#define fclaw2d_patch_average_corner    fclaw3d_patch_average_corner
#define fclaw2d_patch_interpolate_corner fclaw3d_patch_interpolate_corner
#define fclaw2d_patch_create_user_data  fclaw3d_patch_create_user_data
#define fclaw2d_patch_destroy_user_data fclaw3d_patch_destroy_user_data
#define fclaw2d_patch_transform_init_data fclaw3d_patch_transform_init_data
#define fclaw2d_patch_transform_blockface fclaw3d_patch_transform_blockface
#define fclaw2d_patch_transform_blockface_intra fclaw3d_patch_transform_blockface_intra
#define fclaw2d_patch_tag4refinement    fclaw3d_patch_tag4refinement
#define fclaw2d_patch_tag4coarsening    fclaw3d_patch_tag4coarsening
#define fclaw2d_patch_interpolate2fine  fclaw3d_patch_interpolate2fine
#define fclaw2d_patch_average2coarse    fclaw3d_patch_average2coarse
#define fclaw2d_patch_ghost_packsize    fclaw3d_patch_ghost_packsize
#define fclaw2d_patch_local_ghost_alloc fclaw3d_patch_local_ghost_alloc
#define fclaw2d_patch_local_ghost_free  fclaw3d_patch_local_ghost_free
#define fclaw2d_patch_local_ghost_pack  fclaw3d_patch_local_ghost_pack
#define fclaw2d_patch_remote_ghost_build fclaw3d_patch_remote_ghost_build
#define fclaw2d_patch_remote_ghost_unpack fclaw3d_patch_remote_ghost_unpack
#define fclaw2d_patch_remote_ghost_delete fclaw3d_patch_remote_ghost_delete
#define fclaw2d_patch_partition_pack    fclaw3d_patch_partition_pack
#define fclaw2d_patch_partition_unpack  fclaw3d_patch_partition_unpack
#define fclaw2d_patch_partition_packsize fclaw3d_patch_partition_packsize
#define fclaw2d_patch_time_sync_f2c     fclaw3d_patch_time_sync_f2c
#define fclaw2d_patch_time_sync_samesize fclaw3d_patch_time_sync_samesize
#define fclaw2d_patch_time_sync_reset   fclaw3d_patch_time_sync_reset
#define fclaw2d_patch_vt                fclaw3d_patch_vt
#define fclaw2d_patch_vtable_initialize fclaw3d_patch_vtable_initialize
#define fclaw2d_patch_get_info          fclaw3d_patch_get_info
#define fclaw2d_patch_get_info2         fclaw3d_patch_get_info2
#define fclaw2d_patch_get_user_patch    fclaw3d_patch_get_user_patch
#define fclaw2d_patch_get_patch_data    fclaw3d_patch_get_patch_data
#define fclaw2d_patch_get_user_data     fclaw3d_patch_get_user_data
#define fclaw2d_patch_metric_patch      fclaw3d_patch_metric_patch
#define fclaw2d_patch_get_blockno       fclaw3d_patch_get_blockno
#define fclaw2d_patch_get_patchno       fclaw3d_patch_get_patchno
#define fclaw2d_patch_user_data         fclaw3d_patch_user_data
#define fclaw2d_patch_set_user_data     fclaw3d_patch_set_user_data
#define fclaw2d_patch_on_parallel_boundary fclaw3d_patch_on_parallel_boundary
#define fclaw2d_patch_set_face_type     fclaw3d_patch_set_face_type
#define fclaw2d_patch_set_corner_type   fclaw3d_patch_set_corner_type
#define fclaw2d_patch_set_missing_corner fclaw3d_patch_set_missing_corner
#define fclaw2d_patch_get_face_type     fclaw3d_patch_get_face_type
#define fclaw2d_patch_get_corner_type   fclaw3d_patch_get_corner_type
#define fclaw2d_patch_corner_is_missing fclaw3d_patch_corner_is_missing
#define fclaw2d_patch_neighbors_set   fclaw3d_patch_neighbors_set
#define fclaw2d_patch_neighbors_reset   fclaw3d_patch_neighbors_reset
#define fclaw2d_patch_neighbor_type_set fclaw3d_patch_neighbor_type_set
#define fclaw2d_patch_has_finegrid_neighbors fclaw3d_patch_has_finegrid_neighbors
#define fclaw2d_patch_on_coarsefine_interface fclaw3d_patch_on_coarsefine_interface
#define fclaw2d_patch_block_corner_count fclaw3d_patch_block_corner_count
#define fclaw2d_patch_set_block_corner_count fclaw3d_patch_set_block_corner_count
#define fclaw2d_domain_attribute_add    fclaw3d_domain_attribute_add
#define fclaw2d_domain_attribute_access fclaw3d_domain_attribute_access
#define fclaw2d_domain_attribute_remove fclaw3d_domain_attribute_remove
#define fclaw2d_domain_iterate_level    fclaw3d_domain_iterate_level
#define fclaw2d_domain_iterate_patches  fclaw3d_domain_iterate_patches
#define fclaw2d_domain_iterate_families fclaw3d_domain_iterate_families
#define fclaw2d_patch_boundary_type     fclaw3d_patch_boundary_type
#define fclaw2d_patch_normal_match      fclaw3d_patch_normal_match
#define fclaw2d_patch_face_neighbors    fclaw3d_patch_face_neighbors
#define fclaw2d_patch_face_swap         fclaw3d_patch_face_swap
#define fclaw2d_patch_face_transformation   fclaw3d_patch_face_transformation
#define fclaw2d_patch_face_transformation_block fclaw3d_patch_face_transformation_block
#define fclaw2d_patch_face_transformation_intra fclaw3d_patch_face_transformation_intra
#define fclaw2d_patch_face_transformation_valid fclaw3d_patch_face_transformation_valid
#define fclaw2d_patch_transform_face    fclaw3d_patch_transform_face
#define fclaw2d_patch_transform_face2   fclaw3d_patch_transform_face2
#define fclaw2d_patch_corner_neighbors  fclaw3d_patch_corner_neighbors
#define fclaw2d_patch_corner_swap       fclaw3d_patch_corner_swap
#define fclaw2d_patch_transform_corner  fclaw3d_patch_transform_corner
#define fclaw2d_patch_transform_corner2 fclaw3d_patch_transform_corner2
#define fclaw2d_domain_set_refinement   fclaw3d_domain_set_refinement
#define fclaw2d_patch_mark_refine       fclaw3d_patch_mark_refine
#define fclaw2d_patch_mark_coarsen      fclaw3d_patch_mark_coarsen
#define fclaw2d_domain_data_new         fclaw3d_domain_data_new
#define fclaw2d_domain_data_delete      fclaw3d_domain_data_delete
#define fclaw2d_domain_get_data         fclaw3d_domain_get_data
#define fclaw2d_domain_setup            fclaw3d_domain_setup
#define fclaw2d_domain_reset            fclaw3d_domain_reset
#define fclaw2d_domain_iterate_adapted  fclaw3d_domain_iterate_adapted
#define fclaw2d_domain_assign_for_partition fclaw3d_domain_assign_for_partition
#define fclaw2d_domain_allocate_before_partition    fclaw3d_domain_allocate_before_partition
#define fclaw2d_domain_retrieve_after_partition     fclaw3d_domain_retrieve_after_partition
#define fclaw2d_domain_iterate_partitioned  fclaw3d_domain_iterate_partitioned
#define fclaw2d_domain_free_after_partition fclaw3d_domain_free_after_partition
#define fclaw2d_domain_allocate_before_exchange fclaw3d_domain_allocate_before_exchange
#define fclaw2d_domain_free_after_exchange  fclaw3d_domain_free_after_exchange
#define fclaw2d_domain_ghost_exchange   fclaw3d_domain_ghost_exchange
#define fclaw2d_domain_ghost_exchange_begin fclaw3d_domain_ghost_exchange_begin
#define fclaw2d_domain_ghost_exchange_end   fclaw3d_domain_ghost_exchange_end
#define fclaw2d_domain_free_after_exchange  fclaw3d_domain_free_after_exchange
#define fclaw2d_output_frame            fclaw3d_output_frame

#define fclaw2d_exchange_setup          fclaw3d_exchange_setup
#define fclaw2d_exchange_delete         fclaw3d_exchange_delete
#define fclaw2d_exchange_ghost_patches_begin fclaw3d_exchange_ghost_patches_begin
#define fclaw2d_exchange_ghost_patches_end fclaw3d_exchange_ghost_patches_end
#define fclaw2d_domain_serialization_enter  fclaw3d_domain_serialization_enter
#define fclaw2d_domain_serialization_leave  fclaw3d_domain_serialization_leave
#define fclaw2d_domain_is_meta          fclaw3d_domain_is_meta
#define fclaw2d_domain_init_meta        fclaw3d_domain_init_meta
#define fclaw2d_domain_destroy          fclaw3d_domain_destroy
#define fclaw2d_domain_adapt            fclaw3d_domain_adapt
#define fclaw2d_domain_partition        fclaw3d_domain_partition
#define fclaw2d_domain_partition_unchanged  fclaw3d_domain_partition_unchanged
#define fclaw2d_domain_complete         fclaw3d_domain_complete
#define fclaw2d_domain_write_vtk        fclaw3d_domain_write_vtk
#define fclaw2d_domain_list_levels      fclaw3d_domain_list_levels
#define fclaw2d_domain_list_neighbors   fclaw3d_domain_list_neighbors
#define fclaw2d_domain_list_adapted     fclaw3d_domain_list_adapted
#define fclaw2d_domain_search_points    fclaw3d_domain_search_points
#define fclaw2d_domain_iterate_cb       fclaw3d_domain_iterate_cb
#define fclaw2d_domain_iterate_level_mthread fclaw3d_domain_iterate_level_mthread
#define fclaw_domain_new2d              fclaw_domain_new3d
#define fclaw_domain_destroy2d          fclaw_domain_destroy3d

/* translations for maps */
#define fclaw2d_map_context_t           fclaw3d_map_context_t
#define fclaw2d_map_destroy_t           fclaw3d_map_destroy_t
#define fclaw2d_map_destroy             fclaw3d_map_destroy
#define fclaw2d_map_new_nomap           fclaw3d_map_new_nomap

/* translations for the global data structure */
#define fclaw2d_iterate_patch_cb        fclaw3d_iterate_patch_cb
#define fclaw2d_iterate_family_cb       fclaw3d_iterate_family_cb
#define fclaw2d_domain_integrate_rays   fclaw3d_domain_integrate_rays
#define fclaw2d_overlap_exchange        fclaw3d_overlap_exchange
#define fclaw2d_global_t                fclaw3d_global_t
#define fclaw2d_global_iterate_t        fclaw3d_global_iterate_t
#define fclaw2d_global_new              fclaw3d_global_new
#define fclaw2d_global_new_comm         fclaw3d_global_new_comm
#define fclaw2d_global_destroy          fclaw3d_global_destroy
#define fclaw2d_global_store_domain     fclaw3d_global_store_domain
#define fclaw2d_global_store_map        fclaw3d_global_store_map
#define fclaw2d_global_iterate_level    fclaw3d_global_iterate_level
#define fclaw2d_global_iterate_patches  fclaw3d_global_iterate_patches
#define fclaw2d_global_iterate_families fclaw3d_global_iterate_families
#define fclaw2d_global_iterate_adapted  fclaw3d_global_iterate_adapted
#define fclaw2d_global_iterate_level_mthread fclaw3d_global_iterate_level_mthread
#define fclaw2d_global_iterate_partitioned fclaw3d_global_iterate_partitioned
#define fclaw2d_global_options_store    fclaw3d_global_options_store
#define fclaw2d_global_get_options      fclaw3d_global_get_options
#define fclaw2d_global_set_global       fclaw3d_global_set_global
#define fclaw2d_global_unset_global     fclaw3d_global_unset_global
#define fclaw2d_global_get_global       fclaw3d_global_get_global
#define fclaw2d_set_global_context      fclaw3d_set_global_context
#define fclaw2d_clear_global_context    fclaw3d_clear_global_context

/* translations for the options */
#define fclaw2d_options_store           fclaw3d_options_store
#define fclaw2d_get_options             fclaw3d_get_options

/* translations for the global vtable */
#define fclaw2d_problem_setup_t         fclaw3d_problem_setup_t
#define fclaw2d_output_frame_t          fclaw3d_output_frame_t
#define fclaw2d_after_regrid_t          fclaw3d_after_regrid_t
#define fclaw2d_vtable                  fclaw3d_vtable
#define fclaw2d_vtable_t                fclaw3d_vtable_t
#define fclaw2d_vt                      fclaw3d_vt
#define fclaw2d_vtable_initialize       fclaw3d_vtable_initialize
#define fclaw2d_after_regrid            fclaw3d_after_regrid

/* translations for timeinterp */
#define fclaw2d_timeinterp              fclaw3d_timeinterp
#define FCLAW2D_TIMEINTERP_FORT         FCLAW3D_TIMEINTERP_FORT

/* translations for time_sync */
#define fclaw2d_time_sync_type           fclaw3d_time_sync_type
#define FCLAW2D_TIME_SYNC_RESET_F2C      FCLAW3D_TIME_SYNC_RESET_F2C
#define FCLAW2D_TIME_SYNC_RESET_SAMESIZE FCLAW3D_TIME_SYNC_RESET_SAMESIZE
#define FCLAW2D_TIME_SYNC_RESET_PHYS     FCLAW3D_TIME_SYNC_RESET_PHYS
#define fclaw2d_time_sync_type_t         fclaw3d_time_sync_type_t
#define fclaw2d_time_sync                fclaw3d_time_sync

/* translations for diagnostics */
#define fclaw2d_diagnostics_vtable_t          fclaw3d_diagnostics_vtable_t
#define fclaw2d_diagnostics_accumulator_t     fclaw3d_diagnostics_accumulator_t
#define fclaw2d_diagnostics_accumulator       fclaw3d_diagnostics_accumulator
#define fclaw2d_diagnostics_initialize_t      fclaw3d_diagnostics_initialize_t
#define fclaw2d_diagnostics_compute_t         fclaw3d_diagnostics_compute_t
#define fclaw2d_diagnostics_gather_t          fclaw3d_diagnostics_gather_t
#define fclaw2d_diagnostics_reset_t           fclaw3d_diagnostics_reset_t
#define fclaw2d_diagnostics_finalize_t        fclaw3d_diagnostics_finalize_t
#define fclaw2d_diagnostics_vtable            fclaw3d_diagnostics_vtable
#define fclaw2d_diagnostics_vt                fclaw3d_diagnostics_vt
#define fclaw2d_diagnostics_vtable_initialize fclaw3d_diagnostics_vtable_initialize
#define fclaw2d_domain_global_minimum         fclaw3d_domain_global_minimum
#define fclaw2d_diagnostics_initialize        fclaw3d_diagnostics_initialize
#define fclaw2d_diagnostics_gather            fclaw3d_diagnostics_gather
#define fclaw2d_diagnostics_reset             fclaw3d_diagnostics_reset
#define fclaw2d_diagnostics_finalize          fclaw3d_diagnostics_finalize

/* translations not found in p4est */
#ifndef p4est_wrap_new_unitsquare
#define p4est_wrap_new_unitsquare       p8est_wrap_new_unitcube
#endif

#endif /* !FCLAW2D_TO_3D_H */
