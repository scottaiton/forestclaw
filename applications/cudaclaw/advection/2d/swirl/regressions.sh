#!/bin/sh
# absolute path to application we are testing
application=$FCLAW_APPLICATIONS_BUILD_DIR/cudaclaw/advection/2d/swirl/swirl_cuda

# change to source dir for working directory
cd $FCLAW_APPLICATIONS_SRC_DIR/cudaclaw/advection/2d/swirl/

# run programs, exit script with nonzero on failure (or else script will exit with value of last program run)
$FCLAW_MPIRUN $FCLAW_MPI_TEST_FLAGS $application -F regression.ini  --regression-check=regression_values_0.csv || exit 1