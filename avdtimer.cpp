//
//	SPDX-License-Identifier: MIT
//	SPDX-FileCopyrightText: Copyright (C) 2026 Steffen A. Mork
//

#include <cstdio>
#include <cstdint>

#include "avdtimer.h"

#ifdef __amiga__
#include <proto/exec.h>
#include <proto/timer.h>

struct Device * TimerBase;

#endif

AvdTimer::AvdTimer()
{
#ifdef __amiga__
	OpenDevice(TIMERNAME, UNIT_ECLOCK, &timer_request, 0);
	TimerBase = timer_request.io_Device;
#endif

	reset();
}

AvdTimer::~AvdTimer()
{
#ifdef __amiga__
#endif
}

void AvdTimer::get(timeval & tv)
{
#ifdef __amiga__
	struct EClockVal eclock_val;

	ReadEClock(&eclock_val);

	const std::uint64_t eclock = (std::uint64_t(eclock_val.ev_hi) << 32) + eclock_val.ev_lo;
	const unsigned f = eclock % ECLOCK_HZ;

	tv.tv_sec  = eclock / ECLOCK_HZ;
	tv.tv_usec = 1000000llu * f / ECLOCK_HZ;

#else
	gettimeofday(&tv, nullptr);
#endif
}

void AvdTimer::reset()
{
	get(start);
}

void AvdTimer::print(const timeval & tv)
{
	printf("%lu:%06lu\n", tv.tv_sec, tv.tv_usec);
}

double AvdTimer::diff() const
{
	struct timeval tv;

	get(tv);

	return getDouble(tv) - getDouble(start);
}

double AvdTimer::getDouble(const timeval & tv)
{
	const double seconds      = tv.tv_sec;
	const double microseconds = tv.tv_usec;
	const double result       = seconds + microseconds / 1000000.0;

#if 0
	printf("%lu.%lu - %0.06lfs\n", tv.tv_sec, tv.tv_usec, result);
#endif

	return result;
}
