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

#include <fclaw2d_global.h>
#include <fclaw2d_elliptic_solver.h>
#include <test.hpp>

TEST_CASE("fclaw2d_elliptic_vtable_initialize stores two seperate vtables in two seperate globs")
{
	fclaw2d_global_t* glob1 = fclaw2d_global_new();
	fclaw2d_global_t* glob2 = fclaw2d_global_new();

	fclaw2d_elliptic_vtable_initialize(glob1);
	fclaw2d_elliptic_vtable_initialize(glob2);

	CHECK_NE(fclaw2d_elliptic_vt(glob1), fclaw2d_elliptic_vt(glob2));

	fclaw2d_global_destroy(glob1);
	fclaw2d_global_destroy(glob2);
}

TEST_CASE("fclaw2d_elliptic_vtable_initialize sets is_set flag")
{
	fclaw2d_global_t* glob = fclaw2d_global_new();

	fclaw2d_elliptic_vtable_initialize(glob);

	CHECK_UNARY(fclaw2d_elliptic_vt(glob)->is_set);

	fclaw2d_global_destroy(glob);
}

#ifdef FCLAW_ENABLE_DEBUG

TEST_CASE("fclaw2d_elliptic_vtable_initialize fails if called twice on a glob")
{
	fclaw2d_global_t* glob1 = fclaw2d_global_new();
	fclaw2d_global_t* glob2 = fclaw2d_global_new();

	fclaw2d_elliptic_vtable_initialize(glob1);
	CHECK_SC_ABORTED(fclaw2d_elliptic_vtable_initialize(glob1));
	fclaw2d_elliptic_vtable_initialize(glob2);
	CHECK_SC_ABORTED(fclaw2d_elliptic_vtable_initialize(glob2));

	fclaw2d_global_destroy(glob1);
	fclaw2d_global_destroy(glob2);
}

#endif