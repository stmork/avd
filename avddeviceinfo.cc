//
//	SPDX-License-Identifier: MIT
//	SPDX-FileCopyrightText: Copyright (C) 2026 Steffen A. Mork
//

#include "avddeviceinfo.h"

namespace
{
	const AvdDeviceInfo AVD_MVD819_INFO
	{
		"MVD819",
		{
			0x4f01,
			0x4d01,

			0x0080,
			0x0181,

			0x4000,
			0x4100,
			0x4300,
			0x4701,

			0x4501
		},
		2157, 7
	};

	const AvdDeviceInfo AVD_VLAB_INFO
	{
		"VLab 1.0",
		{
			0x2901,
			0x2b01,

			0x0000,
			0x0401,

			0x2200,
			0x2080,
			0x2280,
			0x2101,

			0x2301
		},
		18260, 4
	};
}

const std::unordered_map<AutoConfigKey, const AvdDeviceInfo *, AutoConfigKey> avd_map
{
	{
		AutoConfigKey(AVD_MVD819_INFO), &AVD_MVD819_INFO
	},
	{
		AutoConfigKey(AVD_VLAB_INFO), &AVD_VLAB_INFO
	}
};
