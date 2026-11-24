//
//	SPDX-License-Identifier: MIT
//	SPDX-FileCopyrightText: Copyright (C) 2026 Steffen A. Mork
//

#include "avddeviceinfo.h"

namespace
{
	static constexpr AvdDeviceInfo::UvLUT MVD819_UV_LUT = []() constexpr
	{
		AvdDeviceInfo::UvLUT lut;

		for (AvdDeviceInfo::UvLUT::value_type uv = 0; uv < lut.size(); uv++)
		{
			lut[uv] = ((uv & 0xf0) << 4) | (uv & 0xf); // Split UV into low nibbles
		}
		return lut;
	}();

	static constexpr AvdDeviceInfo AVD_MVD819_INFO
	{
		"MVD 819",
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
		MVD819_UV_LUT,
		2157, 7,
		1,
		false
	};

	static constexpr AvdDeviceInfo::UvLUT VLab_UV_LUT = []()constexpr
	{
		AvdDeviceInfo::UvLUT lut;

		for (AvdDeviceInfo::UvLUT::value_type uv = 0; uv < lut.size(); uv++)
		{
			lut[uv] =
			((uv & 0xc0) << 4) | ((uv & 0x0c) << 6) | // U
			((uv & 0x30) >> 2) |  (uv & 0x03); // V
		}
		return lut;
	} ();

	static constexpr AvdDeviceInfo AVD_VLAB_INFO
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
		VLab_UV_LUT,
		18260, 4,
		0,
		false
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
