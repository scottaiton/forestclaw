"""
Module to set up run time parameters for Clawpack.

The values set in the function setrun are then written out to data files
that will be read in by the Fortran code.

"""

from __future__ import absolute_import
import os
import numpy as np

#------------------------------
def setrun(claw_pkg='Classic'):
#------------------------------

    """
    Define the parameters used for running Clawpack.

    INPUT:
        claw_pkg expected to be "classic" for this setrun.

    OUTPUT:
        rundata - object of class ClawRunData

    """

    from clawpack.clawutil import data 


    assert claw_pkg.lower() == 'classic',  "Expected claw_pkg = 'classic'"

    num_dim = 1
    rundata = data.ClawRunData(claw_pkg, num_dim)

    #------------------------------------------------------------------
    # Parameters set below
    #------------------------------------------------------------------

    # Physical parameters
    L = 4096.    # L = 4000
    ax = 0
    bx = L

    # Time stepping parameters
    tfinal = 2000                  # If outstyle = 1

    outstyle = 1
    if outstyle == 1:
        tfinal = 2000
        tfinal = 90
        nout = 18
    elif outstyle == 2:
        tfinal = 2000                  # If outstyle = 1
        tsteps = [ 35.0, 90.0, 700.0, 2000.0]   # If outstyle = 2
    else:
        nout = 400
        nsteps = 50    # If outstyle = 3

    dt_initial = 0.1047505286036417
    dt_initial = 0.8
    dt_variable = True

    limiter = 'minmod'      # 7 = generalized minmod


    # Spatial parameters
    mx_grid = 32
    mi = 128

    mx = 2**15     # 2**15 = 32768, 2**7 = 128
    # mx = mx_grid*mi  # 2**5*2**7 = 2^12

    maux = 3      # Store bathymetry


    #------------------------------------------------------------------
    # Problem-specific parameters to be written to setprob.data:
    #------------------------------------------------------------------

    probdata = rundata.new_UserData(name='probdata',fname='setprob.data')
    probdata.add_param('grav',          1.0,   'Gravity')
    probdata.add_param('dry_tolerance', 1e-4, 'Dry_tolerance')
    probdata.add_param('sea_level',     0.0,   'Sea Level')

    # Set to -1 to turn off dispersion
    probdata.add_param('breaking',      1.0,  'Breaking slope') 

    # From S. Popinet's paper/website
    probdata.add_param('alpha',         1.153,  'Alpha (for dispersion)')

    # Used for initial conditions
    probdata.add_param('a',             0.1,  'a (from Madsen/Popinet)')
    probdata.add_param('b',             12.2, 'b (from Madsen/Popinet)')
    probdata.add_param('h0',            1.0,  'Initial height')

    #------------------------------------------------------------------
    # Standard Clawpack parameters to be written to claw.data:
    #------------------------------------------------------------------

    clawdata = rundata.clawdata  # initialized when rundata instantiated

    # ---------------
    # Spatial domain:
    # ---------------

    # Number of space dimensions:
    clawdata.num_dim = 1

    # Lower and upper edge of computational domain:
    clawdata.lower[0] = ax
    clawdata.upper[0] = bx

    # Number of grid cells:    
    clawdata.num_cells[0] = mx

    # ---------------
    # Size of system:
    # ---------------

    # Number of equations in the system:
    clawdata.num_eqn = 3

    # Number of auxiliary variables in the aux array (initialized in setaux)
    clawdata.num_aux = maux

    # Index of aux array corresponding to capacity function, if there is one:
    clawdata.capa_index = 0

    # -------------
    # Initial time:
    # -------------

    clawdata.t0 = 0.0

    # Restart from checkpoint file of a previous run?
    # If restarting, t0 above should be from original run, and the
    # restart_file 'fort.qNNNN' specified below should be in 
    # the OUTDIR indicated in Makefile.

    clawdata.restart = False               # True to restart from prior results
    clawdata.restart_file = 'fort.q0006'   # File to use for restart data


    # -------------
    # Output times:
    #--------------

    # Specify at what times the results should be written to fort.q files.
    # Note that the time integration stops after the final output time.
    # The solution at initial time t0 is always written in addition.

    clawdata.output_style = outstyle

    if clawdata.output_style==1:
        # Output nout frames at equally spaced times up to tfinal:
        clawdata.num_output_times = nout
        clawdata.tfinal = tfinal
        clawdata.output_t0 = True  # output at initial (or restart) time?

    elif clawdata.output_style == 2:
        # Specify a list of output times.
        clawdata.output_t0 = True  # output at initial (or restart) time?
        clawdata.output_times =  tsteps   # used if outstyle == 2

    elif clawdata.output_style == 3:
        # Output every iout timesteps with a total of ntot time steps:
        clawdata.total_steps = nout
        clawdata.output_step_interval = nsteps
        clawdata.output_t0 = True  # output at initial (or restart) time?


    clawdata.output_format = 'ascii'      # 'ascii', 'binary', 'netcdf'

    clawdata.output_q_components = 'all'   # could be list such as [True,True]
    clawdata.output_aux_components = 'none'  # could be list
    clawdata.output_aux_onlyonce = True    # output aux arrays only at t0


    # ---------------------------------------------------
    # Verbosity of messages to screen during integration:
    # ---------------------------------------------------

    # The current t, dt, and cfl will be printed every time step
    # at AMR levels <= verbosity.  Set verbosity = 0 for no printing.
    #   (E.g. verbosity == 2 means print only on levels 1 and 2.)
    clawdata.verbosity = 1


    # --------------
    # Time stepping:
    # --------------

    # if dt_variable==1: variable time steps used based on cfl_desired,
    # if dt_variable==0: fixed time steps dt = dt_initial will always be used.
    clawdata.dt_variable = dt_variable

    # Initial time step for variable dt.
    # If dt_variable==0 then dt=dt_initial for all steps:
    clawdata.dt_initial = dt_initial

    # Max time step to be allowed if variable dt used:
    clawdata.dt_max = 1e+99

    # Desired Courant number if variable dt used, and max to allow without
    # retaking step with a smaller dt:
    clawdata.cfl_desired = 0.9
    clawdata.cfl_max = 1.0

    # Maximum number of time steps to allow between output times:
    clawdata.steps_max = 1000000


    # ------------------
    # Method to be used:
    # ------------------

    # Order of accuracy:  1 => Godunov,  2 => Lax-Wendroff plus limiters
    clawdata.order = 2

    # Number of waves in the Riemann solution:
    clawdata.num_waves = 3

    # List of limiters to use for each wave family:  
    # Required:  len(limiter) == num_waves
    # Some options:
    #   0 or 'none'     ==> no limiter (Lax-Wendroff)
    #   1 or 'minmod'   ==> minmod
    #   2 or 'superbee' ==> superbee
    #   3 or 'vanleer'  ==> van Leer
    #   4 or 'mc'       ==> MC limiter
    #clawdata.limiter = ['minmod', 'minmod']
    clawdata.limiter = [limiter]*3

    clawdata.use_fwaves = True    # True ==> use f-wave version of algorithms
    
    # Source terms splitting:
    #   src_split == 0 or 'none'    ==> no source term (src routine never called)
    #   src_split == 1 or 'godunov' ==> Godunov (1st order) splitting used, 
    #   src_split == 2 or 'strang'  ==> Strang (2nd order) splitting used,  not recommended.
    clawdata.source_split = 'godunov'

    # --------------------
    # Boundary conditions:
    # --------------------

    # Number of ghost cells (usually 2)
    clawdata.num_ghost = 2

    # Choice of BCs at xlower and xupper:
    #   0 => user specified (must modify bcN.f to use this option)
    #   1 => extrapolation (non-reflecting outflow)
    #   2 => periodic (must specify this at both boundaries)
    #   3 => solid wall for systems where q(2) is normal velocity

    clawdata.bc_lower[0] = 'wall'   # at xlower
    clawdata.bc_upper[0] = 'extrap'   # at xupper

    return rundata
    # end of function setrun
    # ----------------------

if __name__ == '__main__':
    # Set up run-time parameters and write all data files.
    import sys
    rundata = setrun(*sys.argv[1:])
    rundata.write()
