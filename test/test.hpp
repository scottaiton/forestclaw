#ifndef FCLAW_TEST_HPP
#define FCLAW_TEST_HPP
#include <doctest.h>
#include <csetjmp>
#include <initializer_list>

bool test_output_vtk();

void fclaw_test_expect_abort();
void fclaw_test_clear_expect_abort();

void fclaw_test_set_dirty_memory();

std::jmp_buf& fclaw_test_get_jump_buffer();

#define CHECK_SC_ABORTED(expr) \
{ \
    bool aborted = false; \
	fclaw_test_expect_abort(); \
	if(!setjmp(fclaw_test_get_jump_buffer())){ \
	    expr; \
	}else{ \
		fclaw_test_set_dirty_memory(); \
		aborted=true; \
	} \
	CHECK_UNARY(aborted); \
	fclaw_test_clear_expect_abort(); \
}

#endif