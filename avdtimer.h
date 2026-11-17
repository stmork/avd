//
//	SPDX-License-Identifier: MIT
//	SPDX-FileCopyrightText: Copyright (C) 2026 Steffen A. Mork
//

#pragma once

#ifndef AVD_TIMER_H
#define AVD_TIMER_H

#include <time.h>

class AvdTimer
{
	struct timespec       start;

public:

	AvdTimer();

	void          reset();
	double        diff() const;

	static void   get(struct timespec & tv);
	static void   print(const struct timespec & tv);
	static double getDouble(const struct timespec & tv);
};

#endif // AVD_TIMER_H
