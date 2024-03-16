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

#include <fclaw_file.h>
#include <fclaw2d_wrap.h>
#include <fclaw3d_wrap.h>

struct fclaw_file_context
{
    int refine_dim;
    fclaw2d_file_context_t *d2;
    fclaw3d_file_context_t *d3;
};

static fclaw_file_context_t *wrap_file_2d(fclaw2d_file_context_t *d2)
{
    fclaw_file_context_t *fc = FCLAW_ALLOC (fclaw_file_context_t, 1);
    fc->refine_dim = 2;
    fc->d2 = d2;
    fc->d3 = NULL;
    return fc;
}

static fclaw_file_context_t *wrap_file_3d(fclaw3d_file_context_t *d3)
{
    fclaw_file_context_t *fc = FCLAW_ALLOC (fclaw_file_context_t, 1);
    fc->refine_dim = 3;
    fc->d2 = NULL;
    fc->d3 = d3;
    return fc;
}

fclaw_file_context_t *fclaw_file_open_write (const char *filename,
                                             const char *user_string,
                                             fclaw_domain_t * domain,
                                             int *errcode)
{
    if(domain->refine_dim == 2)
    {
        fclaw2d_file_context_t *d2 = fclaw2d_file_open_write (filename, user_string,
                                                              domain->d2, errcode);
        return wrap_file_2d(d2);
    }
    else if(domain->refine_dim == 3)
    {
        fclaw3d_file_context_t *d3 = fclaw3d_file_open_write (filename, user_string,
                                                              domain->d3, errcode);
        return wrap_file_3d(d3);
    }
    else
    {
        SC_ABORT_NOT_REACHED ();
    }
}

int fclaw_file_write_partition (const char *filename,
                                const char *user_string,
                                fclaw_domain_t * domain, int *errcode)
{
    if(domain->refine_dim == 2)
    {
        return fclaw2d_file_write_partition (filename, user_string, domain->d2, errcode);
    }
    else if(domain->refine_dim == 3)
    {
        return fclaw3d_file_write_partition (filename, user_string, domain->d3, errcode);
    }
    else
    {
        SC_ABORT_NOT_REACHED ();
    }
}

fclaw_file_context_t *fclaw_file_write_block (fclaw_file_context_t *
                                              fc, const char *user_string,
                                              size_t block_size,
                                              sc_array_t * block_data,
                                              int *errcode)
{
    if(fc->refine_dim == 2)
    {
        return wrap_file_2d(fclaw2d_file_write_block (fc->d2, user_string,
                                                      block_size, block_data, errcode));
    }
    else if(fc->refine_dim == 3)
    {
        return wrap_file_3d(fclaw3d_file_write_block (fc->d3, user_string,
                                                      block_size, block_data, errcode));
    }
    else
    {
        SC_ABORT_NOT_REACHED ();
    }
}

fclaw_file_context_t *fclaw_file_write_array (fclaw_file_context_t *
                                              fc, const char *user_string,
                                              size_t patch_size,
                                              sc_array_t * patch_data,
                                              int *errcode)
{
    if(fc->refine_dim == 2)
    {
        return wrap_file_2d(fclaw2d_file_write_array (fc->d2, user_string,
                                                      patch_size, patch_data, errcode));
    }
    else if(fc->refine_dim == 3)
    {
        return wrap_file_3d(fclaw3d_file_write_array (fc->d3, user_string,
                                                      patch_size, patch_data, errcode));
    }
    else
    {
        SC_ABORT_NOT_REACHED ();
    }
}

int fclaw_file_read_partition (int refine_dim,
                               const char *filename, char *user_string,
                               sc_MPI_Comm mpicomm, sc_array_t * partition,
                               int *errcode)
{
    if(refine_dim == 2)
    {
        return fclaw2d_file_read_partition (filename, user_string, mpicomm, partition, errcode);
    }
    else if(refine_dim == 3)
    {
        return fclaw3d_file_read_partition (filename, user_string, mpicomm, partition, errcode);
    }
    else
    {
        fclaw_abortf("Invalid refine_dim %d\n", refine_dim);
    }
}

fclaw_file_context_t *fclaw_file_open_read (int refine_dim,
                                            const char *filename,
                                            char *user_string,
                                            sc_MPI_Comm mpicomm,
                                            sc_array_t * partition,
                                            fclaw_domain_t ** domain,
                                            int *errcode)
{
    if(refine_dim == 2)
    {
        fclaw2d_domain_t *new_domain;
        fclaw2d_file_context_t *d2 = fclaw2d_file_open_read (filename, user_string,
                                                             mpicomm, partition, &new_domain, errcode);
        
        *domain = fclaw_domain_wrap_2d(new_domain);
        return wrap_file_2d(d2);
    }
    else if(refine_dim == 3)
    {
        fclaw3d_domain_t *new_domain;
        fclaw3d_file_context_t *d3 = fclaw3d_file_open_read (filename, user_string,
                                                             mpicomm, partition, &new_domain, errcode);
        *domain = fclaw_domain_wrap_3d(new_domain);
        return wrap_file_3d(d3);
    }
    else
    {
        fclaw_abortf("Invalid refine_dim %d\n", refine_dim);
    }
}

fclaw_file_context_t *fclaw_file_read_block (fclaw_file_context_t *
                                             fc, char *user_string,
                                             size_t block_size,
                                             sc_array_t * block_data,
                                             int *errcode)
{
    if(fc->refine_dim == 2)
    {
        fclaw2d_file_context_t* retval;
        retval = fclaw2d_file_read_block (fc->d2, user_string,
                                          block_size, block_data, errcode);
        if(retval == NULL)
        {
            return NULL;
        }
        else
        {
            FCLAW_ASSERT(fc->d2 == retval);
            return fc;
        }
    }
    else if(fc->refine_dim == 3)
    {
        fclaw3d_file_context_t* retval;
        retval = fclaw3d_file_read_block (fc->d3, user_string,
                                          block_size, block_data, errcode);
        if(retval == NULL)
        {
            return NULL;
        }
        else
        {
            FCLAW_ASSERT(fc->d3 == retval);
            return fc;
        }
    }
    else
    {
        SC_ABORT_NOT_REACHED ();
    }

}

fclaw_file_context_t *fclaw_file_read_array (fclaw_file_context_t *
                                             fc, char *user_string,
                                             size_t patch_size,
                                             sc_array_t * patch_data,
                                             int *errcode)
{
    if(fc->refine_dim == 2)
    {
        fclaw2d_file_context_t* retval;
        retval = fclaw2d_file_read_array (fc->d2, user_string,
                                          patch_size, patch_data, errcode);
        if(retval == NULL)
        {
            return NULL;
        }
        else
        {
            FCLAW_ASSERT(fc->d2 == retval);
            return fc;
        }
    }
    else if(fc->refine_dim == 3)
    {
        fclaw3d_file_context_t* retval;
        retval = fclaw3d_file_read_array (fc->d3, user_string,
                                          patch_size, patch_data, errcode);
        if(retval == NULL)
        {
            return NULL;
        }
        else
        {
            FCLAW_ASSERT(fc->d3 == retval);
            return fc;
        }
    }
    else
    {
        SC_ABORT_NOT_REACHED ();
    }
}

int fclaw_file_close (fclaw_file_context_t * fc, int *errcode)
{
    int retval;
    if(fc->refine_dim == 2)
    {
        retval = fclaw2d_file_close (fc->d2, errcode);
    }
    else if(fc->refine_dim == 3)
    {
        retval = fclaw3d_file_close (fc->d3, errcode);
    }
    else
    {
        SC_ABORT_NOT_REACHED ();
    }
    FCLAW_FREE(fc);
    return retval;
}

int fclaw_file_error_string (int refine_dim, int errcode, char *string, int *resultlen)
{
    if(refine_dim == 2)
    {
        return fclaw2d_file_error_string (errcode, string, resultlen);
    }
    else if(refine_dim == 3)
    {
        return fclaw3d_file_error_string (errcode, string, resultlen);
    }
    else
    {
        fclaw_abortf("Invalid refine_dim %d\n", refine_dim);
    }
}
