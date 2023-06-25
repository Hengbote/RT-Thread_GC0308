/**
 ******************************************************************************
 * @file   matd.h
 * @author SRA AI Application Team
 * @brief  STM32 Image Processing Library - matd header file
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.
 *
 * Portions of this file are part of the OpenMV project.
 *
 * Copyright (c) 2013-2019 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2019 Kwabena W. Agyeman <kwagyeman@openmv.io>
 * Copyright (c) 2013-2016 The Regents of The University of Michigan
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* This file has been created for STM32IPL and contains code moved here from
 * apriltag.c file.
 */


#ifndef __MATD_H_
#define __MATD_H_

#include <stdarg.h>
#include <stdio.h>
#include "arm_math.h"
#include "fmath.h"
#undef assert
#define assert(expression)
#define floor(x) fast_floorf(x)
#define fabs(x) fast_fabsf(x)

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "math_util.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef M_TWOPI
// STM32IPL		# define M_TWOPI       6.2831853071795862319959  /* 2*pi */
# define M_TWOPI	6.2831853f  /* 2*pi */		// STM32IPL
#endif

 #ifndef M_PI
 // STM32IPL		# define M_PI 3.141592653589793238462643383279502884196
 # define M_PI 		3.14159265f		// STM32IPL	
 #endif

#define to_radians(x) ( (x) * (M_PI / 180.0 ))
#define to_degrees(x) ( (x) * (180.0 / M_PI ))

#define max(A, B) (A < B ? B : A)
#define min(A, B) (A < B ? A : B)

/* DEPRECATE, threshold meaningless without context.
 static inline int dequals(float a, float b)
 {
 float thresh = 1e-9;
 return (fabs(a-b) < thresh);
 }
 */

static inline int dequals_mag(float a, float b, float thresh)
{
	return (fabs(a-b) < thresh);
}

static inline int isq(int v)
{
	return v * v;
}

static inline float fsq(float v)
{
	return v * v;
}

static inline float sq(float v)
{
	return v * v;
}

static inline float sgn(float v)
{
	return (v >= 0) ? 1 : -1;
}

// random number between [0, 1)
static inline float randf(void)
{
	return ((float)rand()) / (RAND_MAX + 1.0);
}

static inline float signed_randf(void)
{
	return randf() * 2 - 1;
}

// return a random integer between [0, bound)
static inline int irand(int bound)
{
	int v = (int)(randf() * bound);
	if (v == bound)
		return (bound - 1);
	//assert(v >= 0);
	//assert(v < bound);
	return v;
}

/** Map vin to [0, 2*PI) **/
static inline float mod2pi_positive(float vin)
{
	return vin - M_TWOPI * floor(vin / M_TWOPI);
}

/** Map vin to [-PI, PI) **/
static inline float mod2pi(float vin)
{
	return mod2pi_positive(vin + M_PI) - M_PI;
}

/** Return vin such that it is within PI degrees of ref **/
static inline float mod2pi_ref(float ref, float vin)
{
	return ref + mod2pi(vin - ref);
}

/** Map vin to [0, 360) **/
static inline float mod360_positive(float vin)
{
	return vin - 360 * floor(vin / 360);
}

/** Map vin to [-180, 180) **/
static inline float mod360(float vin)
{
	return mod360_positive(vin + 180) - 180;
}

static inline int theta_to_int(float theta, int max)
{
	theta = mod2pi_ref(M_PI, theta);
	int v = (int)(theta / M_TWOPI * max);

	if (v == max)
		v = 0;

	assert (v >= 0 && v < max);

	return v;
}

static inline int imin(int a, int b)
{
	return (a < b) ? a : b;
}

static inline int imax(int a, int b)
{
	return (a > b) ? a : b;
}

static inline int64_t imin64(int64_t a, int64_t b)
{
	return (a < b) ? a : b;
}

static inline int64_t imax64(int64_t a, int64_t b)
{
	return (a > b) ? a : b;
}

static inline int iclamp(int v, int minv, int maxv)
{
	return imax(minv, imin(v, maxv));
}

static inline float dclamp(float a, float min, float max)
{
	if (a < min)
		return min;
	if (a > max)
		return max;
	return a;
}

static inline int fltcmp(float f1, float f2)
{
	float epsilon = f1 - f2;
	if (epsilon < 0.0f)		// STM32IPL: added f to the constant.
		return -1;
	else
		if (epsilon > 0.0f)	// STM32IPL: added f to the constant.
			return 1;
		else
			return 0;
}

static inline int dblcmp(float d1, float d2)
{
	float epsilon = d1 - d2;
	if (epsilon < 0.0f)	// STM32IPL: added f to the constant.
		return -1;
	else
		if (epsilon > 0.0f)	// STM32IPL: added f to the constant.
			return 1;
		else
			return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "svd22.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

void svd22(const float A[4], float U[4], float S[2], float V[4]);

// for the matrix [a b; b d]
void svd_sym_singular_values(float A00, float A01, float A11, float *Lmin, float *Lmax);

////////////////////////////////////////////////////////////////////////////////////////////////////
//////// "matd.h"
////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Defines a matrix structure for holding float-precision values with
 * data in row-major order (i.e. index = row*ncols + col).
 *
 * nrows and ncols are 1-based counts with the exception that a scalar (non-matrix)
 *   is represented with nrows=0 and/or ncols=0.
 */
typedef struct
{
	unsigned int nrows, ncols;
	float data[];
//    float *data;
} matd_t;

#define MATD_ALLOC(name, nrows, ncols) float name ## _storage [nrows*ncols]; matd_t name = { .nrows = nrows, .ncols = ncols, .data = &name ## _storage };

/**
 * Defines a small value which can be used in place of zero for approximating
 * calculations which are singular at zero values (i.e. inverting a matrix with
 * a zero or near-zero determinant).
 */
#define MATD_EPS 1e-8f		// STM32IPL: f added to the constant.

/**
 * A macro to reference a specific matd_t data element given it's zero-based
 * row and column indexes. Suitable for both retrieval and assignment.
 */
#define MATD_EL(m, row, col) (m)->data[((row)*(m)->ncols + (col))]

/**
 * Creates a float matrix with the given number of rows and columns (or a scalar
 * in the case where rows=0 and/or cols=0). All data elements will be initialized
 * to zero. It is the caller's responsibility to call matd_destroy() on the
 * returned matrix.
 */
matd_t* matd_create(int rows, int cols);

/**
 * Creates a float matrix with the given number of rows and columns (or a scalar
 * in the case where rows=0 and/or cols=0). All data elements will be initialized
 * using the supplied array of data, which must contain at least rows*cols elements,
 * arranged in row-major order (i.e. index = row*ncols + col). It is the caller's
 * responsibility to call matd_destroy() on the returned matrix.
 */
matd_t* matd_create_data(int rows, int cols, const float *data);

/**
 * Creates a float matrix with the given number of rows and columns (or a scalar
 * in the case where rows=0 and/or cols=0). All data elements will be initialized
 * using the supplied array of float data, which must contain at least rows*cols elements,
 * arranged in row-major order (i.e. index = row*ncols + col). It is the caller's
 * responsibility to call matd_destroy() on the returned matrix.
 */
matd_t* matd_create_dataf(int rows, int cols, const float *data);

/**
 * Creates a square identity matrix with the given number of rows (and
 * therefore columns), or a scalar with value 1 in the case where dim=0.
 * It is the caller's responsibility to call matd_destroy() on the
 * returned matrix.
 */
matd_t* matd_identity(int dim);

/**
 * Creates a scalar with the supplied value 'v'. It is the caller's responsibility
 * to call matd_destroy() on the returned matrix.
 *
 * NOTE: Scalars are different than 1x1 matrices (implementation note:
 * they are encoded as 0x0 matrices). For example: for matrices A*B, A
 * and B must both have specific dimensions. However, if A is a
 * scalar, there are no restrictions on the size of B.
 */
matd_t* matd_create_scalar(float v);

/**
 * Retrieves the cell value for matrix 'm' at the given zero-based row and column index.
 * Performs more thorough validation checking than MATD_EL().
 */
float matd_get(const matd_t *m, int row, int col);

/**
 * Assigns the given value to the matrix cell at the given zero-based row and
 * column index. Performs more thorough validation checking than MATD_EL().
 */
void matd_put(matd_t *m, int row, int col, float value);

/**
 * Retrieves the scalar value of the given element ('m' must be a scalar).
 * Performs more thorough validation checking than MATD_EL().
 */
float matd_get_scalar(const matd_t *m);

/**
 * Assigns the given value to the supplied scalar element ('m' must be a scalar).
 * Performs more thorough validation checking than MATD_EL().
 */
void matd_put_scalar(matd_t *m, float value);

/**
 * Creates an exact copy of the supplied matrix 'm'. It is the caller's
 * responsibility to call matd_destroy() on the returned matrix.
 */
matd_t* matd_copy(const matd_t *m);

/**
 * Creates a copy of a subset of the supplied matrix 'a'. The subset will include
 * rows 'r0' through 'r1', inclusive ('r1' >= 'r0'), and columns 'c0' through 'c1',
 * inclusive ('c1' >= 'c0'). All parameters are zero-based (i.e. matd_select(a, 0, 0, 0, 0)
 * will return only the first cell). Cannot be used on scalars or to extend
 * beyond the number of rows/columns of 'a'. It is the caller's  responsibility to
 * call matd_destroy() on the returned matrix.
 */
matd_t* matd_select(const matd_t *a, int r0, int r1, int c0, int c1);

/**
 * Prints the supplied matrix 'm' to standard output by applying the supplied
 * printf format specifier 'fmt' for each individual element. Each row will
 * be printed on a separate newline.
 */
void matd_print(const matd_t *m, const char *fmt);

/**
 * Prints the transpose of the supplied matrix 'm' to standard output by applying
 * the supplied printf format specifier 'fmt' for each individual element. Each
 * row will be printed on a separate newline.
 */
void matd_print_transpose(const matd_t *m, const char *fmt);

/**
 * Adds the two supplied matrices together, cell-by-cell, and returns the results
 * as a new matrix of the same dimensions. The supplied matrices must have
 * identical dimensions.  It is the caller's responsibility to call matd_destroy()
 * on the returned matrix.
 */
matd_t* matd_add(const matd_t *a, const matd_t *b);

/**
 * Adds the values of 'b' to matrix 'a', cell-by-cell, and overwrites the
 * contents of 'a' with the results. The supplied matrices must have
 * identical dimensions.
 */
void matd_add_inplace(matd_t *a, const matd_t *b);

/**
 * Subtracts matrix 'b' from matrix 'a', cell-by-cell, and returns the results
 * as a new matrix of the same dimensions. The supplied matrices must have
 * identical dimensions.  It is the caller's responsibility to call matd_destroy()
 * on the returned matrix.
 */
matd_t* matd_subtract(const matd_t *a, const matd_t *b);

/**
 * Subtracts the values of 'b' from matrix 'a', cell-by-cell, and overwrites the
 * contents of 'a' with the results. The supplied matrices must have
 * identical dimensions.
 */
void matd_subtract_inplace(matd_t *a, const matd_t *b);

/**
 * Scales all cell values of matrix 'a' by the given scale factor 's' and
 * returns the result as a new matrix of the same dimensions. It is the caller's
 * responsibility to call matd_destroy() on the returned matrix.
 */
matd_t* matd_scale(const matd_t *a, float s);

/**
 * Scales all cell values of matrix 'a' by the given scale factor 's' and
 * overwrites the contents of 'a' with the results.
 */
void matd_scale_inplace(matd_t *a, float s);

/**
 * Multiplies the two supplied matrices together (matrix product), and returns the
 * results as a new matrix. The supplied matrices must have dimensions such that
 * columns(a) = rows(b). The returned matrix will have a row count of rows(a)
 * and a column count of columns(b). It is the caller's responsibility to call
 * matd_destroy() on the returned matrix.
 */
matd_t* matd_multiply(const matd_t *a, const matd_t *b);

/**
 * Creates a matrix which is the transpose of the supplied matrix 'a'. It is the
 * caller's responsibility to call matd_destroy() on the returned matrix.
 */
matd_t* matd_transpose(const matd_t *a);

/**
 * Calculates the determinant of the supplied matrix 'a'.
 */
float matd_det(const matd_t *a);

/**
 * Attempts to compute an inverse of the supplied matrix 'a' and return it as
 * a new matrix. This is strictly only possible if the determinant of 'a' is
 * non-zero (matd_det(a) != 0).
 *
 * If the determinant is zero, NULL is returned. It is otherwise the
 * caller's responsibility to cope with the results caused by poorly
 * conditioned matrices. (E.g.., if such a situation is likely to arise, compute
 * the pseudo-inverse from the SVD.)
 **/
matd_t* matd_inverse(const matd_t *a);

static inline void matd_set_data(matd_t *m, const float *data)
{
	memcpy(m->data, data, m->nrows * m->ncols * sizeof(float));
}

/**
 * Determines whether the supplied matrix 'a' is a scalar (positive return) or
 * not (zero return, indicating a matrix of dimensions at least 1x1).
 */
static inline int matd_is_scalar(const matd_t *a)
{
	assert(a != NULL);
	return a->ncols == 0 || a->nrows == 0;
}

/**
 * Determines whether the supplied matrix 'a' is a row or column vector
 * (positive return) or not (zero return, indicating either 'a' is a scalar or a
 * matrix with at least one dimension > 1).
 */
static inline int matd_is_vector(const matd_t *a)
{
	assert(a != NULL);
	return a->ncols == 1 || a->nrows == 1;
}

/**
 * Determines whether the supplied matrix 'a' is a row or column vector
 * with a dimension of 'len' (positive return) or not (zero return).
 */
static inline int matd_is_vector_len(const matd_t *a, int len)
{
	assert(a != NULL);
	return (a->ncols == 1 && a->nrows == len) || (a->ncols == len && a->nrows == 1);
}

/**
 * Calculates the magnitude of the supplied matrix 'a'.
 */
float matd_vec_mag(const matd_t *a);

/**
 * Calculates the magnitude of the distance between the points represented by
 * matrices 'a' and 'b'. Both 'a' and 'b' must be vectors and have the same
 * dimension (although one may be a row vector and one may be a column vector).
 */
float matd_vec_dist(const matd_t *a, const matd_t *b);

/**
 * Same as matd_vec_dist, but only uses the first 'n' terms to compute distance
 */
float matd_vec_dist_n(const matd_t *a, const matd_t *b, int n);

/**
 * Calculates the dot product of two vectors. Both 'a' and 'b' must be vectors
 * and have the same dimension (although one may be a row vector and one may be
 * a column vector).
 */
float matd_vec_dot_product(const matd_t *a, const matd_t *b);

/**
 * Calculates the normalization of the supplied vector 'a' (i.e. a unit vector
 * of the same dimension and orientation as 'a' with a magnitude of 1) and returns
 * it as a new vector. 'a' must be a vector of any dimension and must have a
 * non-zero magnitude. It is the caller's responsibility to call matd_destroy()
 * on the returned matrix.
 */
matd_t* matd_vec_normalize(const matd_t *a);

/**
 * Calculates the cross product of supplied matrices 'a' and 'b' (i.e. a x b)
 * and returns it as a new matrix. Both 'a' and 'b' must be vectors of dimension
 * 3, but can be either row or column vectors. It is the caller's responsibility
 * to call matd_destroy() on the returned matrix.
 */
matd_t* matd_crossproduct(const matd_t *a, const matd_t *b);

float matd_err_inf(const matd_t *a, const matd_t *b);

/**
 * Creates a new matrix by applying a series of matrix operations, as expressed
 * in 'expr', to the supplied list of matrices. Each matrix to be operated upon
 * must be represented in the expression by a separate matrix placeholder, 'M',
 * and there must be one matrix supplied as an argument for each matrix
 * placeholder in the expression. All rules and caveats of the corresponding
 * matrix operations apply to the operated-on matrices. It is the caller's
 * responsibility to call matd_destroy() on the returned matrix.
 *
 * Available operators (in order of increasing precedence):
 *   M+M   add two matrices together
 *   M-M   subtract one matrix from another
 *   M*M   multiply to matrices together (matrix product)
 *   MM    multiply to matrices together (matrix product)
 *   -M    negate a matrix
 *   M^-1  take the inverse of a matrix
 *   M'    take the transpose of a matrix
 *
 * Expressions can be combined together and grouped by enclosing them in
 * parenthesis, i.e.:
 *   -M(M+M+M)-(M*M)^-1
 *
 * Scalar values can be generated on-the-fly, i.e.:
 *   M*2.2  scales M by 2.2
 *   -2+M   adds -2 to all elements of M
 *
 * All whitespace in the expression is ignored.
 */
matd_t* matd_op(const char *expr, ...);

/**
 * Frees the memory associated with matrix 'm', being the result of an earlier
 * call to a matd_*() function, after which 'm' will no longer be usable.
 */
void matd_destroy(matd_t *m);

typedef struct
{
	matd_t *U;
	matd_t *S;
	matd_t *V;
} matd_svd_t;

/** Compute a complete SVD of a matrix. The SVD exists for all
 * matrices. For a matrix MxN, we will have:
 *
 * A = U*S*V'
 *
 * where A is MxN, U is MxM (and is an orthonormal basis), S is MxN
 * (and is diagonal up to machine precision), and V is NxN (and is an
 * orthonormal basis).
 *
 * The caller is responsible for destroying U, S, and V.
 **/
matd_svd_t matd_svd(matd_t *A);

#define MATD_SVD_NO_WARNINGS 1
matd_svd_t matd_svd_flags(matd_t *A, int flags);

////////////////////////////////
// PLU Decomposition

// All square matrices (even singular ones) have a partially-pivoted
// LU decomposition such that A = PLU, where P is a permutation
// matrix, L is a lower triangular matrix, and U is an upper
// triangular matrix.
//
typedef struct
{
	// was the input matrix singular? When a zero pivot is found, this
	// flag is set to indicate that this has happened.
	int singular;

	unsigned int *piv; // permutation indices
	int pivsign; // either +1 or -1

	// The matd_plu_t object returned "owns" the enclosed LU matrix. It
	// is not expected that the returned object is itself useful to
	// users: it contains the L and U information all smushed
	// together.
	matd_t *lu; // combined L and U matrices, permuted so they can be triangular.
} matd_plu_t;

matd_plu_t* matd_plu(const matd_t *a);
void matd_plu_destroy(matd_plu_t *mlu);
float matd_plu_det(const matd_plu_t *lu);
matd_t* matd_plu_p(const matd_plu_t *lu);
matd_t* matd_plu_l(const matd_plu_t *lu);
matd_t* matd_plu_u(const matd_plu_t *lu);
matd_t* matd_plu_solve(const matd_plu_t *mlu, const matd_t *b);

// uses LU decomposition internally.
matd_t* matd_solve(matd_t *A, matd_t *b);

////////////////////////////////
// Cholesky Factorization

/**
 * Creates a float matrix with the Cholesky lower triangular matrix
 * of A. A must be symmetric, positive definite. It is the caller's
 * responsibility to call matd_destroy() on the returned matrix.
 */
//matd_t *matd_cholesky(const matd_t *A);
typedef struct
{
	int is_spd;
	matd_t *u;
} matd_chol_t;

matd_chol_t* matd_chol(matd_t *A);
matd_t* matd_chol_solve(const matd_chol_t *chol, const matd_t *b);
void matd_chol_destroy(matd_chol_t *chol);
// only sensible on PSD matrices
matd_t* matd_chol_inverse(matd_t *a);

void matd_ltransposetriangle_solve(matd_t *u, const float *b, float *x);
void matd_ltriangle_solve(matd_t *u, const float *b, float *x);
void matd_utriangle_solve(matd_t *u, const float *b, float *x);

float matd_max(matd_t *m);

#endif /* __MATD_H_ */
