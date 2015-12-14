/*
Copyright (c) 2012 Carsten Burstedde, Donna Calhoun
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

#ifndef SUBCYCLE_MANAGER_H
#define SUBCYCLE_MANAGER_H

/* this header file must come first */
#include <fclaw2d_defs.h>

#include <fclaw2d_convenience.h>
#include <fclaw2d_forestclaw.h>
#include <fclaw_options.h>

#include <iostream>
#include <cstdlib>
#include <vector>


class level_data
{
public:

    level_data();
    ~level_data();

    void define(const int a_level,
                const int a_refratio,
                const int a_maxlevel,
                const double a_time,
                const bool a_subcycle);

    void set_dt(const double a_dt);

    void increment_step_counter();
    void increment_time();
    double current_time();
    void set_time(double t);
    double dt();

    int m_level;
    int m_last_step;
    int m_step_inc;

    double m_time;
    double m_dt;
};



class subcycle_manager
{
public:
    subcycle_manager();
    ~subcycle_manager();
    void define(fclaw2d_domain_t *domain,
                const amr_options_t *gparms,
                const double a_time,
                const double dt_minlevel);

    int last_step(const int a_level);
    int step_inc(const int a_level);
    void increment_step_counter(const int a_level);

    bool nosubcycle();
    bool subcycle();
    int verbosity();
    double sync_time();


    // These deal with real-valued 'time' and 'dt' value.  Most others only deal with integers,
    // i.e. powers of ref_ratio.
    double level_time(const int a_level);   /// time() ?
    double initial_time();
    double dt(int level);
    void increment_time(const int a_level);
    void set_dt_minlevel(const double a_dt);
    void set_dt_maxlevel(const double a_dt);

    int minlevel_factor();
    int maxlevel_factor();

    bool is_coarsest(const int a_level);
    int minlevel();
    int maxlevel();

    int local_minlevel();
    int local_maxlevel();

    int user_minlevel();
    int user_maxlevel();

private :
    std::vector<level_data> m_levels;
    int m_local_maxlevel;   /* Local to this proc */
    int m_local_minlevel;
    int m_user_minlevel;    /* Set by the user and stored in parms */
    int m_user_maxlevel;
    int m_refratio;
    double m_dt_minlevel;
    double m_initial_time;
    bool m_nosubcycle;
    int m_verbosity;
};


#endif
