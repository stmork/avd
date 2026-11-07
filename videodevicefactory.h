//
//	SPDX-License-Identifier: MIT
//	SPDX-FileCopyrightText: Copyright (C) 2026 Steffen A. Mork
//

#pragma once

#ifndef AVD_VIDEODEVICEFACTORY_H
#define AVD_VIDEODEVICEFACTORY_H

#include <vector>

#include "amigavideodevice.h"

class VideoDeviceFactory
{
	mutable std::vector<AmigaVideoDevice *> devices;

public:
	VideoDeviceFactory();
	~VideoDeviceFactory();

	std::vector<AmigaVideoDevice *> & get() const;
private:
	void find_device(
		const AutoConfigKey  &  key,
		volatile std::uint8_t * io);
};

#endif // AVD_VIDEODEVICEFACTORY_H
