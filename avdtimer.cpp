//
//	SPDX-License-Identifier: MIT
//	SPDX-FileCopyrightText: Copyright (C) 2026 Steffen A. Mork
//

#include <cstdio>
#include <cstdint>

#include "avdtimer.h"

AvdTimer::AvdTimer()
{
	reset();
}

void AvdTimer::get(struct timespec & tv)
{
	clock_gettime(CLOCK_MONOTONIC, &tv);
}

void AvdTimer::reset()
{
	get(start);
}

void AvdTimer::print(const timespec & tv)
{
	printf("%lu:%06lu\n", tv.tv_sec, tv.tv_nsec / 1000);
}

double AvdTimer::diff() const
{
	struct timespec tv;

	get(tv);

	return getDouble(tv) - getDouble(start);
}

double AvdTimer::getDouble(const timespec & tv)
{
	const double seconds      = tv.tv_sec;
	const double microseconds = tv.tv_nsec / 1000;
	const double result       = seconds + microseconds / 1000000.0;

#if 0
	printf("%lu.%lu - %0.06lfs\n", tv.tv_sec, tv.tv_nsec / 1000, result);
#endif

	return result;
}
