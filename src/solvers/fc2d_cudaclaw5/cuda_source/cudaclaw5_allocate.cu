#include "cudaclaw5_allocate.h"

#include <fclaw2d_global.h>
#include <fclaw2d_patch.h>
#include <fclaw2d_clawpatch.h>
#include <fclaw_timer.h>



void cudaclaw5_allocate_fluxes(struct fclaw2d_global *glob,
                               struct fclaw2d_patch *patch)
{
    size_t size = fclaw2d_clawpatch_size(glob);

    cudaclaw5_fluxes_t *fluxes = FCLAW_ALLOC(cudaclaw5_fluxes,1);
    fluxes->num_bytes = size*sizeof(double);

    /* Assumption here is that cudaMalloc is a synchronous call */
    fclaw2d_timer_start (&glob->timers[FCLAW2D_TIMER_CUDA_ALLOCATE]);       
    cudaMalloc((void**)&fluxes->qold_dev, fluxes->num_bytes);
    cudaMalloc((void**)&fluxes->fm_dev,   fluxes->num_bytes);
    cudaMalloc((void**)&fluxes->fp_dev,   fluxes->num_bytes);
    cudaMalloc((void**)&fluxes->gm_dev,   fluxes->num_bytes);
    cudaMalloc((void**)&fluxes->gp_dev,   fluxes->num_bytes);
    fclaw2d_timer_stop (&glob->timers[FCLAW2D_TIMER_CUDA_ALLOCATE]);    

    fclaw2d_patch_set_user_data(glob,patch,fluxes);
}

void cudaclaw5_deallocate_fluxes(fclaw2d_global_t *glob,
                                 fclaw2d_patch_t *patch)
{
    cudaclaw5_fluxes_t *fluxes = (cudaclaw5_fluxes_t*) 
               fclaw2d_patch_get_user_data(glob,patch);

    FCLAW_ASSERT(fluxes != NULL);

    /* Assumption here is that cudaFree is a synchronous call */
    fclaw2d_timer_start (&glob->timers[FCLAW2D_TIMER_CUDA_ALLOCATE]);       
    cudaFree(fluxes->qold_dev);
    cudaFree(fluxes->fm_dev);
    cudaFree(fluxes->fp_dev);
    cudaFree(fluxes->gm_dev);
    cudaFree(fluxes->gp_dev);
    fclaw2d_timer_stop (&glob->timers[FCLAW2D_TIMER_CUDA_ALLOCATE]);    

    FCLAW_FREE((void*) fluxes);
}
