#pragma once

#include <stddef.h>

void dgemm_naive(size_t size,
	const double a[restrict static size*size],
	const double b[restrict static size*size],
	double c[restrict static size*size]);

void dgemm_blocked(size_t size,
	const double a[restrict static size*size],
	const double b[restrict static size*size],
	double c[restrict static size*size]);

void chase_pointers(void** data);
