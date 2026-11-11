//
//	SPDX-License-Identifier: MIT
//	SPDX-FileCopyrightText: Copyright (C) 2026 Steffen A. Mork
//

#include <cstdlib>
#include <cstdio>

#include <unistd.h>

#include "dummyi2cdevice.h"
#include "videodevicefactory.h"

#ifdef _DEBUG
#undef _DEBUG
#endif

#ifdef _DEBUG
static std::vector<std::uint8_t> tx_data   = { 0x8a, 0x00, 0x01, 0x02 };
static std::vector<std::uint8_t> rx_header = { 0x8b };
static std::vector<std::uint8_t> rx_data   = { 0 };

static void TestDummy()
{
	DummyI2cDevice     device;

	device.send(tx_data);
	device.set_dummy(0xa5);
	device.receive(rx_header, rx_data);

	for (const auto byte : rx_data)
	{
		printf("%02x ", byte);
	}
	printf("\n");
}
#endif

static void write(
	const std::vector<std::uint8_t> & vector,
	const char            *           filename)
{
	FILE * file = fopen(filename, "wb");

	if (file != nullptr)
	{
		fwrite(vector.data(), 1, vector.size(), file);
		fclose(file);
	}
}

int main()
{
	VideoDeviceFactory        factory;

#ifdef _DEBUG
	TestDummy();
#endif

	if (!factory.get().empty())
	{
		auto           *          device = *factory.get().begin();
		Saa9051Status             status;

		status = device->get_status_byte();
		printf("Status: 0x%02x\n", status.value);
		device->init();
		device->select_input(0);

		// Wait for power on and hlock
		usleep(250000);

		device->scan_sync(625 * AmigaVideoDevice::HLINE_TIMEOUT);

		status = device->get_status_byte();
		printf("Status: 0x%02x\n", status.value);

		if (status.PONRES)
		{
			printf("Ongoing Reset\n");
		}
		if (status.HLOCK)
		{
			printf("Warning: No horizontal sync detected!\n");
		}
		else
		{
			printf("Field frequency: %d Hz, %s\n",
				status.FD ? 60 : 50,
				status.CD ? "color" : "b/w");
			if (status.CS)
			{
				printf("SECAM detected\n");
			}
			device->init_norm(status.CD, status.FD);

			printf("Waiting for sync...\n");
			if (device->sync())
			{
				device->fill();
				printf("Filled VRAM.\n");

				std::vector<std::uint8_t> y;
				std::vector<std::uint8_t> uv;

				printf("Reading VRAM...\n");
				if (status.CD)
				{
					device->read_yuv(y, uv);
				}
				else
				{
					device->read_y(y);
				}

				printf("Writing data to files...\n");
				write(y,  "frame_y.dat");
				if (status.CD)
				{
					write(uv, "frame_uv.dat");
				}
			}
			else
			{
				printf("No sync found!\n");
			}
			write(device->sync_data, "frame_sync.dat");
			write(device->int_data,  "frame_int.dat");
		}

		printf("Int count: %u %u %u\n",
			device->counter,
			device->vsync_counter,
			device->hblank_counter);
	}

	return EXIT_SUCCESS;
}
