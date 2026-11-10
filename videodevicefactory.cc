//
//	SPDX-License-Identifier: MIT
//	SPDX-FileCopyrightText: Copyright (C) 2026 Steffen A. Mork
//

#include <cstdio>
#include <cstring>

#ifdef __amiga__
#include <proto/exec.h>
#include <proto/expansion.h>

#include <libraries/expansion.h>
#endif

#include "videodevicefactory.h"
#include "avddeviceinfo.h"

namespace
{
	static std::uint8_t io_addresses[65536];
}

VideoDeviceFactory::VideoDeviceFactory()
{
	memset(io_addresses, 0x7f, sizeof(io_addresses));

#ifdef __amiga__
	ExpansionBase = reinterpret_cast<struct ExpansionBase *>(OpenLibrary("expansion.library", 0));

	if (ExpansionBase != nullptr)
	{
		ConfigDev * config_dev = nullptr;

		while ((config_dev = FindConfigDev(config_dev, -1, -1)) != nullptr)
		{
			const UWORD vendor_id     = config_dev->cd_Rom.er_Manufacturer;
			const UBYTE product_id    = config_dev->cd_Rom.er_Product;
			APTR        board_address = config_dev->cd_BoardAddr;

			find_device(AutoConfigKey(vendor_id, product_id), static_cast<volatile std::uint8_t *>(board_address));
		}

		CloseLibrary(ExpansionBase);
	}
#endif

#ifdef _DEBUG
	if (devices.empty())
	{
		find_device(AutoConfigKey(2157, 7), io_addresses);
	}
#endif
}

VideoDeviceFactory::~VideoDeviceFactory()
{
	for (auto device : devices)
	{
		delete device;
	}
	devices.clear();
}

std::vector<AmigaVideoDevice *> & VideoDeviceFactory::get() const
{
	return devices;
}

void VideoDeviceFactory::find_device(
	const AutoConfigKey  &  key,
	volatile std::uint8_t * io)
{
	auto it = avd_map.find(key);

	if (it != avd_map.end())
	{
		const AvdDeviceInfo * info = it->second;

		printf("Found: %s at %8p\n", info->name, io);
		AmigaVideoDevice * device = new AmigaVideoDevice(
			io, info->io_offsets, info->y_shift);

		devices.push_back(device);
	}
}
