//
//	SPDX-License-Identifier: MIT
//	SPDX-FileCopyrightText: Copyright (C) 2026 Steffen A. Mork
//

#pragma once

#ifndef AVD_TIMER_H
#define AVD_TIMER_H

#include <sys/time.h>

#ifdef __amiga__
#include <devices/timer.h>
#endif

class AvdTimer
{
#ifdef __amiga__
	static const unsigned ECLOCK_HZ = 709379;

	struct IORequest      timer_request;
#endif

	struct timeval        start;

public:
	AvdTimer();
	~AvdTimer();

	void          reset();
	double        diff() const;

	static void   get(struct timeval & tv);
	static void   print(const struct timeval & tv);
	static double getDouble(const struct timeval & tv);
};

#endif // AVD_TIMER_H
