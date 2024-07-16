/*
Copyright (c) 2012-2021 Carsten Burstedde, Donna Calhoun
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

#include <fclaw_timer.h>
#include <fclaw_global.h>
#include <fclaw_options.h>

#include <fclaw_partition.h>
#include <sc_statistics.h>
#include <fclaw_domain.h>

#define  PRIORITY_WALL          FCLAW_TIMER_PRIORITY_WALL
#define  PRIORITY_EXCLUSIVE1    FCLAW_TIMER_PRIORITY_SUMMARY
#define  PRIORITY_EXCLUSIVE2    FCLAW_TIMER_PRIORITY_EXCLUSIVE
#define  PRIORITY_COUNTERS1     FCLAW_TIMER_PRIORITY_COUNTERS
#define  PRIORITY_COUNTERS2     FCLAW_TIMER_PRIORITY_DETAILS
#define  PRIORITY_REGRID        FCLAW_TIMER_PRIORITY_DETAILS
#define  PRIORITY_PARTITION     FCLAW_TIMER_PRIORITY_DETAILS
#define  PRIORITY_ADVANCE       FCLAW_TIMER_PRIORITY_DETAILS
#define  PRIORITY_GHOST         FCLAW_TIMER_PRIORITY_DETAILS
#define  PRIORITY_SEARCH        FCLAW_TIMER_PRIORITY_DETAILS
#define  PRIORITY_COMM          FCLAW_TIMER_PRIORITY_DETAILS
#define  PRIORITY_CUDA          FCLAW_TIMER_PRIORITY_CUDA
#define  PRIORITY_ELLIPTIC      FCLAW_TIMER_PRIORITY_ELLIPTIC
#define  PRIORITY_EXTRA         FCLAW_TIMER_PRIORITY_EXTRA


#define FCLAW_STATS_SET(stats,glob,NAME) do {               \
    SC_CHECK_ABORT (!(glob)->timers[FCLAW_TIMER_ ## NAME].running,              \
                    "Timer " #NAME " still running in fclaw2d_domain_finalize");  \
    sc_stats_set1 ((stats) + FCLAW_TIMER_ ## NAME,                              \
                   (glob)->timers[FCLAW_TIMER_ ## NAME].cumulative, #NAME);     \
} while (0)

#define FCLAW_STATS_SET_GROUP(stats,NAME,GROUP) do {                  \
    sc_stats_set_group_prio ((stats) + FCLAW_TIMER_ ## NAME,          \
                   GROUP_ ## GROUP, PRIORITY_ ## GROUP);    \
} while (0)



/* -----------------------------------------------------------------
   Work with timers
   ----------------------------------------------------------------- */

double
fclaw_timer_wtime (void)
{
    return sc_MPI_Wtime ();
}

void
fclaw_timer_init (fclaw_timer_t *timer)
{
    memset (timer, 0, sizeof (fclaw_timer_t));
}

void
fclaw_timer_start (fclaw_timer_t *timer)
{
    if (!timer->running) 
    {
        timer->started = fclaw_timer_wtime ();
        timer->stopped = 0.;
        timer->running = 1;
    }
    else
    {
        SC_ABORT_NOT_REACHED ();
    }
}

void
fclaw_timer_stop (fclaw_timer_t *timer)
{
    if (timer->running) 
    {
        timer->stopped = fclaw_timer_wtime ();
        timer->cumulative += timer->stopped - timer->started;
        timer->running = 0;
    }
    else
    {
        SC_ABORT_NOT_REACHED ();
    }
}

/* Don't put timers inside of any step update functions when using OPENMP */
void fclaw_timer_start_threadsafe(fclaw_timer_t *timer)
{
#if !defined(_OPENMP)
    fclaw_timer_start(timer);
#endif    
}

void fclaw_timer_stop_threadsafe(fclaw_timer_t *timer)
{
#if !defined(_OPENMP)
    fclaw_timer_stop(timer);
#endif    
}

void
fclaw2d_timer_report(fclaw_global_t *glob)
{
    sc_statinfo_t stats[FCLAW_TIMER_COUNT];


    fclaw_options_t *fclaw_opt = fclaw_get_options(glob);

    fclaw_timer_stop (&glob->timers[FCLAW_TIMER_WALLTIME]);

    FCLAW_STATS_SET (stats, glob, INIT);
    FCLAW_STATS_SET (stats, glob, OUTPUT);
    FCLAW_STATS_SET (stats, glob, DIAGNOSTICS);
    FCLAW_STATS_SET (stats, glob, REGRID);
    FCLAW_STATS_SET (stats, glob, ADVANCE);
    FCLAW_STATS_SET (stats, glob, GHOSTFILL);
    FCLAW_STATS_SET (stats, glob, ADAPT_COMM);
    FCLAW_STATS_SET (stats, glob, PARTITION_COMM);
    FCLAW_STATS_SET (stats, glob, GHOSTPATCH_COMM);
    FCLAW_STATS_SET (stats, glob, DIAGNOSTICS_COMM);
    FCLAW_STATS_SET (stats, glob, CFL_COMM);
    FCLAW_STATS_SET (stats, glob, WALLTIME);
    FCLAW_STATS_SET (stats, glob, REGRID_BUILD);
    FCLAW_STATS_SET (stats, glob, REGRID_TAGGING);
    FCLAW_STATS_SET (stats, glob, TIMESYNC);
    FCLAW_STATS_SET (stats, glob, PARTITION);
    FCLAW_STATS_SET (stats, glob, PARTITION_BUILD);
    FCLAW_STATS_SET (stats, glob, ADVANCE_STEP2);
    FCLAW_STATS_SET (stats, glob, ADVANCE_B4STEP2);
    FCLAW_STATS_SET (stats, glob, GHOSTPATCH_BUILD);
    FCLAW_STATS_SET (stats, glob, GHOSTFILL_COPY);
    FCLAW_STATS_SET (stats, glob, GHOSTFILL_AVERAGE);
    FCLAW_STATS_SET (stats, glob, GHOSTFILL_INTERP);
    FCLAW_STATS_SET (stats, glob, GHOSTFILL_PHYSBC);
    FCLAW_STATS_SET (stats, glob, GHOSTFILL_STEP1);
    FCLAW_STATS_SET (stats, glob, GHOSTFILL_STEP2);
    FCLAW_STATS_SET (stats, glob, GHOSTFILL_STEP3);
    FCLAW_STATS_SET (stats, glob, NEIGHBOR_SEARCH);
    FCLAW_STATS_SET (stats, glob, CUDA_ALLOCATE);
    FCLAW_STATS_SET (stats, glob, CUDA_MEMCOPY_H2H);
    FCLAW_STATS_SET (stats, glob, CUDA_MEMCOPY_H2D);
    FCLAW_STATS_SET (stats, glob, CUDA_MEMCOPY_D2H);
    FCLAW_STATS_SET (stats, glob, ELLIPTIC_SOLVE);
    FCLAW_STATS_SET (stats, glob, EXTRA1);
    FCLAW_STATS_SET (stats, glob, EXTRA2);
    FCLAW_STATS_SET (stats, glob, EXTRA3);
    FCLAW_STATS_SET (stats, glob, EXTRA4);

    int d = glob->count_grids_per_proc;
    int count_grids_per_proc = (d > 0) ? d : 1;   /* To avoid division by zero */
    d = glob->count_amr_advance;
    int count_amr_advance = (d > 0) ? d : 1;   /* To avoid division by zero */

    double gpp = count_grids_per_proc/             count_amr_advance;
    double glb = glob->count_grids_local_boundary/ count_amr_advance;
    double grb = glob->count_grids_remote_boundary/count_amr_advance;
    double gint = gpp - glb;

    /* compute arithmetic mean of total advance steps per processor */
    sc_stats_set1 (&stats[FCLAW_TIMER_ADVANCE_STEPS_COUNTER],
                   glob->count_single_step,"ADVANCE_STEPS_COUNTER");

    sc_stats_set1 (&stats[FCLAW_TIMER_ELLIPTIC_GRIDS_COUNTER],
                   glob->count_elliptic_grids,"ELLIPTIC_GRIDS_COUNTER");

    /* Compute the arithmetic mean of grids per processor */
    sc_stats_set1 (&stats[FCLAW_TIMER_GRIDS_PER_PROC],gpp,"GRIDS_PER_PROC");

    /* Compute the arithmetic mean of grids in the interior */
    sc_stats_set1 (&stats[FCLAW_TIMER_GRIDS_INTERIOR],gint,
                   "GRIDS_INTERIOR");

    /* Compute the arithmetic mean of local grids on the boundary */
    sc_stats_set1 (&stats[FCLAW_TIMER_GRIDS_LOCAL_BOUNDARY],glb,
                   "GRIDS_LOCAL_BOUNDARY");

    /* Compute the arithmetic mean of remote grids on the boundary */
    sc_stats_set1 (&stats[FCLAW_TIMER_GRIDS_REMOTE_BOUNDARY],grb,
                   "GRIDS_REMOTE_BOUNDARY");

    int time_ex1 = glob->timers[FCLAW_TIMER_REGRID].cumulative +
                   glob->timers[FCLAW_TIMER_ADVANCE].cumulative +
                   glob->timers[FCLAW_TIMER_GHOSTFILL].cumulative +
                   glob->timers[FCLAW_TIMER_ADAPT_COMM].cumulative +
                   glob->timers[FCLAW_TIMER_ELLIPTIC_SOLVE].cumulative +
                   glob->timers[FCLAW_TIMER_GHOSTPATCH_COMM].cumulative;


    int time_ex2 = glob->timers[FCLAW_TIMER_INIT].cumulative +
                   glob->timers[FCLAW_TIMER_OUTPUT].cumulative +
                   glob->timers[FCLAW_TIMER_DIAGNOSTICS].cumulative +
                   glob->timers[FCLAW_TIMER_PARTITION_COMM].cumulative +
                   glob->timers[FCLAW_TIMER_DIAGNOSTICS_COMM].cumulative +
                   glob->timers[FCLAW_TIMER_CFL_COMM].cumulative;

    /* Get a partial sum of timers not accounted for in reported summary */
    int priority = fclaw_opt->report_timing_verbosity;  /* Make this an option */


    if (priority == FCLAW_TIMER_PRIORITY_SUMMARY)
    {
        sc_stats_set1 (&stats[FCLAW_TIMER_UNACCOUNTED],
                       glob->timers[FCLAW_TIMER_WALLTIME].cumulative - time_ex1,
                       "UNACCOUNTED");
    }
    else
    {
        sc_stats_set1 (&stats[FCLAW_TIMER_UNACCOUNTED],
                       glob->timers[FCLAW_TIMER_WALLTIME].cumulative - time_ex1 - time_ex2,
                       "UNACCOUNTED");
    }

    sc_stats_set1 (&stats[FCLAW_TIMER_GLOBAL_COMM],
                   glob->timers[FCLAW_TIMER_ADAPT_COMM].cumulative +
                   glob->timers[FCLAW_TIMER_GHOSTPATCH_COMM].cumulative +
                   glob->timers[FCLAW_TIMER_PARTITION_COMM].cumulative +
                   glob->timers[FCLAW_TIMER_DIAGNOSTICS_COMM].cumulative +
                   glob->timers[FCLAW_TIMER_CFL_COMM].cumulative,
                   "FCLAW_TIMER_GLOBAL_COMM");

    /* Just subtracting FCLAW_TIMER_GLOBAL here doesn't work ... */
    sc_stats_set1 (&stats[FCLAW_TIMER_LOCAL_COMM],
                   glob->timers[FCLAW_TIMER_WALLTIME].cumulative -
                   (glob->timers[FCLAW_TIMER_ADAPT_COMM].cumulative +
                   glob->timers[FCLAW_TIMER_GHOSTPATCH_COMM].cumulative +
                   glob->timers[FCLAW_TIMER_PARTITION_COMM].cumulative +
                   glob->timers[FCLAW_TIMER_DIAGNOSTICS_COMM].cumulative +
                    glob->timers[FCLAW_TIMER_CFL_COMM].cumulative),
                   "FCLAW_TIMER_LOCAL_COMM");


    /* --------------------------------- Set stats groups ------------------------------*/

    /* Names for timer groups */
    enum 
    {
        GROUP_NONE = -1,
        GROUP_EXCLUSIVE1,
        GROUP_EXCLUSIVE2,
        GROUP_WALL,
        GROUP_COUNTERS1,
        GROUP_COUNTERS2,        
        GROUP_COMM,
        GROUP_REGRID,
        GROUP_PARTITION,
        GROUP_ADVANCE,
        GROUP_GHOST,
        GROUP_SEARCH,
        GROUP_CUDA,
        GROUP_EXTRA,
        GROUP_COUNT
    };

    FCLAW_STATS_SET_GROUP(stats,WALLTIME,              WALL);

    FCLAW_STATS_SET_GROUP(stats,ADVANCE,               EXCLUSIVE1);
    FCLAW_STATS_SET_GROUP(stats,ELLIPTIC_SOLVE,        EXCLUSIVE1);
    FCLAW_STATS_SET_GROUP(stats,GHOSTFILL,             EXCLUSIVE1);
    FCLAW_STATS_SET_GROUP(stats,GHOSTPATCH_COMM,       EXCLUSIVE1);
    FCLAW_STATS_SET_GROUP(stats,REGRID,                EXCLUSIVE1);
    FCLAW_STATS_SET_GROUP(stats,ADAPT_COMM,            EXCLUSIVE1);


    if (priority == FCLAW_TIMER_PRIORITY_SUMMARY)
        FCLAW_STATS_SET_GROUP(stats,UNACCOUNTED,           EXCLUSIVE1);
    else
        FCLAW_STATS_SET_GROUP(stats,UNACCOUNTED,           EXCLUSIVE2);    

    FCLAW_STATS_SET_GROUP(stats,INIT,                  EXCLUSIVE2);
    FCLAW_STATS_SET_GROUP(stats,OUTPUT,                EXCLUSIVE2);
    FCLAW_STATS_SET_GROUP(stats,DIAGNOSTICS,           EXCLUSIVE2);
    FCLAW_STATS_SET_GROUP(stats,PARTITION_COMM,        EXCLUSIVE2);
    FCLAW_STATS_SET_GROUP(stats,DIAGNOSTICS_COMM,      EXCLUSIVE2);
    FCLAW_STATS_SET_GROUP(stats,CFL_COMM,              EXCLUSIVE2);

    FCLAW_STATS_SET_GROUP(stats,LOCAL_COMM,            COMM);
    FCLAW_STATS_SET_GROUP(stats,GLOBAL_COMM,           COMM);

    FCLAW_STATS_SET_GROUP(stats,ADVANCE_STEPS_COUNTER, COUNTERS1);
    FCLAW_STATS_SET_GROUP(stats,ELLIPTIC_GRIDS_COUNTER, COUNTERS1);
    FCLAW_STATS_SET_GROUP(stats,GRIDS_PER_PROC,        COUNTERS1);

    FCLAW_STATS_SET_GROUP(stats,GRIDS_INTERIOR,        COUNTERS2);
    FCLAW_STATS_SET_GROUP(stats,GRIDS_LOCAL_BOUNDARY,  COUNTERS2);
    FCLAW_STATS_SET_GROUP(stats,GRIDS_REMOTE_BOUNDARY, COUNTERS2);

    FCLAW_STATS_SET_GROUP(stats,REGRID_BUILD,          REGRID);
    FCLAW_STATS_SET_GROUP(stats,REGRID_TAGGING,        REGRID);

    FCLAW_STATS_SET_GROUP(stats,PARTITION,             PARTITION);  
    FCLAW_STATS_SET_GROUP(stats,PARTITION_BUILD,       PARTITION);

    FCLAW_STATS_SET_GROUP(stats,ADVANCE_STEP2,         ADVANCE);
    FCLAW_STATS_SET_GROUP(stats,ADVANCE_B4STEP2,       ADVANCE);

    FCLAW_STATS_SET_GROUP(stats,GHOSTPATCH_BUILD,      GHOST);
    FCLAW_STATS_SET_GROUP(stats,TIMESYNC,              GHOST);
    FCLAW_STATS_SET_GROUP(stats,GHOSTFILL_COPY,        GHOST);
    FCLAW_STATS_SET_GROUP(stats,GHOSTFILL_AVERAGE,     GHOST);
    FCLAW_STATS_SET_GROUP(stats,GHOSTFILL_INTERP,      GHOST);
    FCLAW_STATS_SET_GROUP(stats,GHOSTFILL_PHYSBC,      GHOST);
    FCLAW_STATS_SET_GROUP(stats,GHOSTFILL_STEP1,       GHOST);
    FCLAW_STATS_SET_GROUP(stats,GHOSTFILL_STEP2,       GHOST);
    FCLAW_STATS_SET_GROUP(stats,GHOSTFILL_STEP3,       GHOST);

    FCLAW_STATS_SET_GROUP(stats,NEIGHBOR_SEARCH,       SEARCH);

    FCLAW_STATS_SET_GROUP(stats,CUDA_ALLOCATE,         CUDA);
    FCLAW_STATS_SET_GROUP(stats,CUDA_MEMCOPY_H2H,      CUDA);
    FCLAW_STATS_SET_GROUP(stats,CUDA_MEMCOPY_H2D,      CUDA);
    FCLAW_STATS_SET_GROUP(stats,CUDA_MEMCOPY_D2H,      CUDA);

    FCLAW_STATS_SET_GROUP(stats,EXTRA1,                EXTRA);
    FCLAW_STATS_SET_GROUP(stats,EXTRA1,                EXTRA);

    FCLAW_STATS_SET_GROUP(stats,EXTRA1,                EXTRA);
    FCLAW_STATS_SET_GROUP(stats,EXTRA2,                EXTRA);
    FCLAW_STATS_SET_GROUP(stats,EXTRA3,                EXTRA);
    FCLAW_STATS_SET_GROUP(stats,EXTRA4,                EXTRA);

    /* ----------------------------------- Compute timers ------------------------------*/

    /* This does all-reduce, etc to set stats */
    sc_stats_compute (glob->mpicomm, FCLAW_TIMER_COUNT, stats);

    /* ------------------------------------ Print timers ------------------------------*/


    sc_stats_print_ext(sc_package_id, SC_LP_ESSENTIAL, FCLAW_TIMER_COUNT,
                       stats,sc_stats_group_all,priority,1,0);


    SC_GLOBAL_ESSENTIALF ("Procs %d advance %d %g exchange %d %g "
                          "regrid %d %d %g\n", glob->mpisize,
                          glob->count_amr_advance,
                          stats[FCLAW_TIMER_ADVANCE].average,
                          glob->count_ghost_exchange,
                          stats[FCLAW_TIMER_GHOSTFILL].average,
                          glob->count_amr_regrid,
                          glob->count_amr_new_domain,
                          stats[FCLAW_TIMER_REGRID].average);

    SC_GLOBAL_ESSENTIALF ("Max/P %d advance %d %g exchange %d %g "
                          "regrid %d %d %g\n", glob->mpisize,
                          glob->count_amr_advance,
                          stats[FCLAW_TIMER_ADVANCE].max,
                          glob->count_ghost_exchange,
                          stats[FCLAW_TIMER_GHOSTFILL].max,
                          glob->count_amr_regrid,
                          glob->count_amr_new_domain,
                          stats[FCLAW_TIMER_REGRID].max);

#if 0
    /* Find out process rank */
    /* TODO : Fix this so that it doesn't interfere with output printed above. */
    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Barrier(MPI_COMM_WORLD);

    /* Write out individual processor timers */
    printf("%12s time on proc %d : %12.4f\n","ADVANCE",
           domain->mpirank,glob->timers[FCLAW_TIMER_ADVANCE].cumulative);
    printf("%12s time on proc %d : %12.4f\n","GHOSTCOMM",
           domain->mpirank,glob->timers[FCLAW_TIMER_GHOSTCOMM].cumulative);
    printf("%12s time on proc %d : %12.4f\n","EXCHANGE",
           domain->mpirank,glob->timers[FCLAW_TIMER_EXCHANGE].cumulative);
    printf("%12s time on proc %d : %12.4f\n","REGRID",
           domain->mpirank,glob->timers[FCLAW_TIMER_REGRID].cumulative);
    printf("\n");
#endif


}
