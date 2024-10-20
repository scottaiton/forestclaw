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
#define FCLAW2D_FILE_USER_STRING_BYTES  FCLAW3D_FILE_USER_STRING_BYTES
#define FCLAW2D_FILE_MAX_BLOCK_SIZE     FCLAW3D_FILE_MAX_BLOCK_SIZE
#define FCLAW2D_FILE_MAX_FIELD_ENTRY_SIZE FCLAW3D_FILE_MAX_FIELD_ENTRY_SIZE
/* not redefining REFINEFACTOR, which should be dimension-independent */

/* redefine typedefs */
#define fclaw2d_patch_flags_t           fclaw3d_patch_flags_t
#define fclaw2d_patch_t                 fclaw3d_patch_t
#define fclaw2d_block_t                 fclaw3d_block_t
#define fclaw2d_domain_t                fclaw3d_domain_t
#define fclaw2d_file_context            fclaw3d_file_context
#define fclaw2d_file_context_t          fclaw3d_file_context_t
#define fclaw2d_domain_persist_t        fclaw3d_domain_persist_t
#define fclaw2d_timer_names_t           fclaw3d_timer_names_t
#define fclaw2d_patch_callback_t        fclaw3d_patch_callback_t
#define fclaw2d_patch_relation_t        fclaw3d_patch_relation_t
#define fclaw2d_match_callback_t        fclaw3d_match_callback_t
#define fclaw2d_transfer_callback_t     fclaw3d_transfer_callback_t
#define fclaw2d_pack_callback_t         fclaw3d_pack_callback_t
#define fclaw2d_unpack_callback_t       fclaw3d_unpack_callback_t
#define fclaw2d_domain_partition_t      fclaw3d_domain_partition_t
#define fclaw2d_domain_exchange_t       fclaw3d_domain_exchange_t
#define fclaw2d_domain_indirect         fclaw3d_domain_indirect
#define fclaw2d_domain_indirect_t       fclaw3d_domain_indirect_t
#define fclaw2d_integrate_ray_t         fclaw3d_integrate_ray_t
#define fclaw2d_interpolate_point_t     fclaw3d_interpolate_point_t
#define fclaw2d_file_context_p4est_v1_t fclaw2d_file_context_p4est_v1_t
#define fclaw2d_file_error_t            fclaw3d_file_error_t

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
#define FCLAW2D_FILE_ERR_SUCCESS     FCLAW3D_FILE_ERR_SUCCESS
#define FCLAW2D_FILE_ERR_FILE        FCLAW3D_FILE_ERR_FILE
#define FCLAW2D_FILE_ERR_NOT_SAME    FCLAW3D_FILE_ERR_NOT_SAME
#define FCLAW2D_FILE_ERR_AMODE       FCLAW3D_FILE_ERR_AMODE
#define FCLAW2D_FILE_ERR_NO_SUCH_FILE FCLAW3D_FILE_ERR_NO_SUCH_FILE
#define FCLAW2D_FILE_ERR_FILE_EXIST  FCLAW3D_FILE_ERR_FILE_EXIST
#define FCLAW2D_FILE_ERR_BAD_FILE    FCLAW3D_FILE_ERR_BAD_FILE
#define FCLAW2D_FILE_ERR_ACCESS      FCLAW3D_FILE_ERR_ACCESS
#define FCLAW2D_FILE_ERR_NO_SPACE    FCLAW3D_FILE_ERR_NO_SPACE
#define FCLAW2D_FILE_ERR_QUOTA       FCLAW3D_FILE_ERR_QUOTA
#define FCLAW2D_FILE_ERR_READ_ONLY   FCLAW3D_FILE_ERR_READ_ONLY
#define FCLAW2D_FILE_ERR_IN_USE      FCLAW3D_FILE_ERR_IN_USE
#define FCLAW2D_FILE_ERR_IO          FCLAW3D_FILE_ERR_IO
#define FCLAW2D_FILE_ERR_FORMAT      FCLAW3D_FILE_ERR_FORMAT
#define FCLAW2D_FILE_ERR_SECTION_TYPE FCLAW3D_FILE_ERR_SECTION_TYPE
#define FCLAW2D_FILE_ERR_CONN        FCLAW3D_FILE_ERR_CONN
#define FCLAW2D_FILE_ERR_P4EST       FCLAW3D_FILE_ERR_P4EST
#define FCLAW2D_FILE_ERR_IN_DATA     FCLAW3D_FILE_ERR_IN_DATA
#define FCLAW2D_FILE_ERR_COUNT       FCLAW3D_FILE_ERR_COUNT
#define FCLAW2D_FILE_ERR_DIM         FCLAW3D_FILE_ERR_DIM
#define FCLAW2D_FILE_ERR_PART        FCLAW3D_FILE_ERR_PART
#define FCLAW2D_FILE_ERR_NOT_IMPLEMENTED FCLAW3D_FILE_ERR_NOT_IMPLEMENTED
#define FCLAW2D_FILE_ERR_UNKNOWN     FCLAW3D_FILE_ERR_UNKNOWN
#define FCLAW2D_FILE_ERR_LASTCODE    FCLAW3D_FILE_ERR_LASTCODE

/* redefine variables */
#define fclaw2d_patch_block_face_flags  fclaw3d_patch_block_face_flags
#define fclaw2d_smallest_h              fclaw3d_smallest_h

/* redefine functions */
#define fclaw2d_domain_global_maximum   fclaw3d_domain_global_maximum
#define fclaw2d_domain_global_sum       fclaw3d_domain_global_sum
#define fclaw2d_domain_barrier          fclaw3d_domain_barrier
#define fclaw2d_domain_dimension        fclaw3d_domain_dimension
#define fclaw2d_check_initial_level     fclaw3d_check_initial_level
#define fclaw2d_file_open_write         fclaw3d_file_open_write
#define fclaw2d_file_write_partition    fclaw3d_file_write_partition
#define fclaw2d_file_write_block        fclaw3d_file_write_block
#define fclaw2d_file_write_array        fclaw3d_file_write_array
#define fclaw2d_file_read_partition     fclaw3d_file_read_partition
#define fclaw2d_file_open_read          fclaw3d_file_open_read
#define fclaw2d_file_read_block         fclaw3d_file_read_block
#define fclaw2d_file_read_array         fclaw3d_file_read_array
#define fclaw2d_file_error_string       fclaw3d_file_error_string
#define fclaw2d_file_close              fclaw3d_file_close
#define fclaw2d_domain_new_brick        fclaw3d_domain_new_brick
#define fclaw2d_domain_new_conn         fclaw3d_domain_new_conn
#define fclaw2d_domain_new_p4est        fclaw3d_domain_new_p8est
#define fclaw2d_domain_num_faces        fclaw3d_domain_num_faces
#define fclaw2d_domain_num_corners      fclaw3d_domain_num_corners
#define fclaw2d_domain_num_face_corners     fclaw3d_domain_num_face_corners
#define fclaw2d_domain_num_orientations fclaw3d_num_orientations
#define fclaw2d_domain_corner_faces     fclaw3d_domain_corner_faces
#define fclaw2d_domain_indirect_begin   fclaw3d_domain_indirect_begin
#define fclaw2d_domain_indirect_face_neighbors fclaw3d_domain_indirect_face_neighbors
#define fclaw2d_domain_indirect_corner_neighbor fclaw3d_domain_indirect_corner_neighbor
#define fclaw2d_domain_indirect_end     fclaw3d_domain_indirect_end
#define fclaw2d_domain_indirect_destroy fclaw3d_domain_indirect_destroy
#define fclaw2d_patch_corner_dimension  fclaw3d_patch_corner_dimension
#define fclaw2d_patch_childid           fclaw3d_patch_childid
#define fclaw2d_patch_is_first_sibling  fclaw3d_patch_is_first_sibling
#define fclaw2d_patch_is_ghost          fclaw3d_patch_is_ghost
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
#define fclaw2d_domain_set_partitioning fclaw3d_domain_set_partitioning
#define fclaw2d_patch_mark_refine       fclaw3d_patch_mark_refine
#define fclaw2d_patch_mark_coarsen      fclaw3d_patch_mark_coarsen
#define fclaw2d_domain_iterate_adapted  fclaw3d_domain_iterate_adapted
#define fclaw2d_domain_assign_for_partition fclaw3d_domain_assign_for_partition
#define fclaw2d_domain_allocate_before_partition    fclaw3d_domain_allocate_before_partition
#define fclaw2d_domain_retrieve_after_partition     fclaw3d_domain_retrieve_after_partition
#define fclaw2d_domain_iterate_partitioned  fclaw3d_domain_iterate_partitioned
#define fclaw2d_domain_iterate_pack     fclaw3d_domain_iterate_pack
#define fclaw2d_domain_iterate_transfer fclaw3d_domain_iterate_transfer
#define fclaw2d_domain_iterate_unpack   fclaw3d_domain_iterate_unpack
#define fclaw2d_domain_partition_free   fclaw3d_domain_partition_free
#define fclaw2d_domain_free_after_partition fclaw3d_domain_free_after_partition
#define fclaw2d_domain_allocate_before_exchange fclaw3d_domain_allocate_before_exchange
#define fclaw2d_domain_free_after_exchange  fclaw3d_domain_free_after_exchange
#define fclaw2d_domain_ghost_exchange   fclaw3d_domain_ghost_exchange
#define fclaw2d_domain_ghost_exchange_begin fclaw3d_domain_ghost_exchange_begin
#define fclaw2d_domain_ghost_exchange_end   fclaw3d_domain_ghost_exchange_end
#define fclaw2d_domain_free_after_exchange  fclaw3d_domain_free_after_exchange

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

/* translations for the global data structure */
#define fclaw2d_domain_integrate_rays   fclaw3d_domain_integrate_rays
#define fclaw2d_overlap_exchange        fclaw3d_overlap_exchange

/* translations not found in p4est */
#ifndef p4est_wrap_new_unitsquare
#define p4est_wrap_new_unitsquare       p8est_wrap_new_unitcube
#endif

/* translations for dimension independent wrapper functions/types */
#define fclaw_domain_wrap_2d               fclaw_domain_wrap_3d
#define fclaw2d_patch_wrap_cb              fclaw3d_patch_wrap_cb
#define fclaw2d_transfer_wrap_cb           fclaw3d_transfer_wrap_cb
#define fclaw2d_match_wrap_cb              fclaw3d_match_wrap_cb
#define fclaw2d_pack_wrap_cb               fclaw3d_pack_wrap_cb
#define fclaw2d_unpack_wrap_cb             fclaw3d_unpack_wrap_cb
#define fclaw2d_intersect_wrap_cb          fclaw3d_intersect_wrap_cb
#define fclaw2d_interpolate_point_wrap_cb  fclaw3d_interpolate_point_wrap_cb
#define fclaw2d_file_error_wrap            fclaw3d_file_error_wrap

#endif /* !FCLAW2D_TO_3D_H */
