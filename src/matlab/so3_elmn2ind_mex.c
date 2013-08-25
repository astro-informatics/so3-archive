// SO3 package to perform Wigner transforms
// Copyright (C) 2013 Martin Buettner and Jason McEwen
// See LICENSE.txt for license details


#include <so3.h>
#include "so3_mex.h"
#include <string.h>
#include <stdlib.h>
#include "mex.h"


/**
 * Compute array index from harmonic indices.
 *
 * Usage:
 *   [ind] = so3_elmn2ind(el, m, n, L, N, order, storage)
 *
 * \author Martin Buettner
 * \author Jason McEwen
 **/
void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
    int el, m, n, L, N, ind;
    int len, iin, iout = 0;
    char order[SO3_STRING_LEN], storage[SO3_STRING_LEN];
    so3_storage_t method;

    /* Check number of arguments */
    if (nrhs != 7)
        mexErrMsgIdAndTxt("so3_elmn2ind_mex:InvalidInput:nrhs",
                          "Require seven inputs.");
    if (nlhs != 1)
        mexErrMsgIdAndTxt("so3_elmn2ind_mex:InvalidInput:nlhs",
                          "Require one output.");

    /* Parse harmonic index el */
    iin = 0;
    if (!mxIsDouble(prhs[iin]) ||
        mxIsComplex(prhs[iin]) ||
        mxGetNumberOfElements(prhs[iin]) != 1)
    {
        mexErrMsgIdAndTxt("so3_elmn2ind_mex:InvalidInput:harmonicIndex",
                          "Harmonic index must be an integer.");
    }
    el = (int)mxGetScalar(prhs[iin]);
    if (mxGetScalar(prhs[iin]) > (double)el ||
        el < 0)
    {
        mexErrMsgIdAndTxt("so3_elmn2ind_mex:InvalidInput:harmonicIndexNonInt",
                          "Harmonic index must be a non-negative integer.");
    }

    /* Parse azimuthal index m */
    iin = 1;
    if (!mxIsDouble(prhs[iin]) ||
        mxIsComplex(prhs[iin]) ||
        mxGetNumberOfElements(prhs[iin]) != 1)
    {
        mexErrMsgIdAndTxt("so3_elmn2ind_mex:InvalidInput:azimuthalIndex",
                          "Azimuthal index must be an integer.");
    }
    m = (int)mxGetScalar(prhs[iin]);
    if (mxGetScalar(prhs[iin]) > (double)m)
    {
        mexErrMsgIdAndTxt("so3_elmn2ind_mex:InvalidInput:azimuthalIndexNonInt",
                          "Azimuthal index must be an integer.");
    }

    /* Parse orientational index n */
    iin = 2;
    if (!mxIsDouble(prhs[iin]) ||
        mxIsComplex(prhs[iin]) ||
        mxGetNumberOfElements(prhs[iin]) != 1)
    {
        mexErrMsgIdAndTxt("so3_elmn2ind_mex:InvalidInput:orientationalIndex",
                          "Orientational index must be an integer.");
    }
    n = (int)mxGetScalar(prhs[iin]);
    if (mxGetScalar(prhs[iin]) > (double)n)
    {
        mexErrMsgIdAndTxt("so3_elmn2ind_mex:InvalidInput:orientationalIndexNonInt",
                          "Orientational index must be an integer.");
    }

    /* Parse harmonic band-limit L */
    iin = 3;
    if (!mxIsDouble(prhs[iin]) ||
        mxIsComplex(prhs[iin]) ||
        mxGetNumberOfElements(prhs[iin]) != 1)
    {
        mexErrMsgIdAndTxt("so3_elmn2ind_mex:InvalidInput:harmonicBandLimit",
                          "Harmonic band-limit must be an integer.");
    }
    L = (int)mxGetScalar(prhs[iin]);
    if (mxGetScalar(prhs[iin]) > (double)L ||
        L <= 0)
    {
        mexErrMsgIdAndTxt("so3_elmn2ind_mex:InvalidInput:harmonicBandLimitNonInt",
                          "Harmonic band-limit must be a positive integer.");
    }

    /* Parse orientational band-limit N */
    iin = 4;
    if (!mxIsDouble(prhs[iin]) ||
        mxIsComplex(prhs[iin]) ||
        mxGetNumberOfElements(prhs[iin]) != 1)
    {
        mexErrMsgIdAndTxt("so3_elmn2ind_mex:InvalidInput:orientationalBandLimit",
                          "Orientational band-limit must be an integer.");
    }
    N = (int)mxGetScalar(prhs[iin]);
    if (mxGetScalar(prhs[iin]) > (double)N ||
        N <= 0)
    {
        mexErrMsgIdAndTxt("so3_elmn2ind_mex:InvalidInput:orientationalBandLimitNonInt",
                          "Orientational band-limit must be a positive integer.");
    }

    /* Additional input checks */
    if (N > L)
        mexErrMsgIdAndTxt("so3_elmn2ind_mex:InvalidInput:NgreaterL",
                          "Orientational band-limit must not exceed harmonic band-limit.");

    if (n >= N)
        mexErrMsgIdAndTxt("so3_elmn2ind_mex:InvalidInput:invalidOrientationalIndex",
                          "Orientational index must not exceed band-limit.");

    if (el >= L)
        mexErrMsgIdAndTxt("so3_elmn2ind_mex:InvalidInput:invalidHarmonicIndex",
                          "Harmonic index must not exceed band-limit.");

    if (abs(m) > el)
        mexErrMsgIdAndTxt("so3_elmn2ind_mex:InvalidInput:invalidAzimuthalIndex",
                          "Modulus of azimuthal index must not exceed harmonic index.");

    /* Parse storage order */
    iin = 5;
    if (!mxIsChar(prhs[iin]))
        mexErrMsgIdAndTxt("so3_elmn2ind_mex:InvalidInput:orderChar",
                          "Storage order must be a string.");

    len = mxGetM(prhs[iin]) * mxGetN(prhs[iin]) + 1;
    if (len >= SO3_STRING_LEN)
        mexErrMsgIdAndTxt("so3_elmn2ind_mex:InvalidInput:orderTooLong",
                          "Storage order exceeds maximum string length.");
    mxGetString(prhs[iin], order, len);

    /* Parse storage type */
    iin = 6;
    if (!mxIsChar(prhs[iin]))
        mexErrMsgIdAndTxt("so3_elmn2ind_mex:InvalidInput:storageChar",
                          "Storage type must be a string.");

    len = mxGetM(prhs[iin]) * mxGetN(prhs[iin]) + 1;
    if (len >= SO3_STRING_LEN)
        mexErrMsgIdAndTxt("so3_elmn2ind_mex:InvalidInput:storageTooLong",
                          "Storage type exceeds maximum string length.");
    mxGetString(prhs[iin], storage, len);

    if (strcmp(storage, SO3_STORAGE_PADDED) == 0)
    {
        if (strcmp(order, SO3_ORDER_ZEROFIRST) == 0)
            method = SO3_STORE_ZERO_FIRST_PAD;
        else if (strcmp(order, SO3_ORDER_NEGFIRST) == 0)
            method = SO3_STORE_NEG_FIRST_PAD;
        else
            mexErrMsgIdAndTxt("so3_elmn2ind_mex:InvalidInput:order",
                              "Invalid storage order.");
    }
    else if (strcmp(storage, SO3_STORAGE_COMPACT) == 0)
    {
        if (el < abs(n))
            mexErrMsgIdAndTxt("so3_elmn2ind_mex:InvalidInput:omittedIndex",
                              "The requested index is not available in compact storage.");

        if (strcmp(order, SO3_ORDER_ZEROFIRST) == 0)
            method = SO3_STORE_ZERO_FIRST_COMPACT;
        else if (strcmp(order, SO3_ORDER_NEGFIRST) == 0)
            method = SO3_STORE_NEG_FIRST_COMPACT;
        else
            mexErrMsgIdAndTxt("so3_elmn2ind_mex:InvalidInput:order",
                              "Invalid storage order.");
    }
    else
        mexErrMsgIdAndTxt("so3_elmn2ind_mex:InvalidInput:storage",
                          "Invalid storage type.");

    so3_sampling_elmn2ind(&ind, el, m, n, L, N, method);
    ++ind; // Adjust index from C-style 0-based to MATLAB-style 1-based
    plhs[iout] = mxCreateDoubleScalar(ind);
}