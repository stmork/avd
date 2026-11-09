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
		auto * device = *factory.get().begin();
		std::vector<std::uint8_t> sync_data;

		device->init();
		device->select_input(0);

		usleep(100000);
		const Saa9051Status status = device->get_status_byte();

		printf("Status: 0x%02x\n", status.value);

		device->scan_sync(sync_data, 50 * 60 * 2);
		write(sync_data, "frame_sync.dat");

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
				std::vector<std::uint8_t> y;
				std::vector<std::uint8_t> uv;

				printf("Filling VRAM...\n");
				device->fill();

				printf("Reading VRAM...\n");
				device->read_yuv(y, uv);

				printf("Writing data to files...\n");
				write(y, "frame_y.dat");
				write(uv, "frame_uv.dat");
			}
		}
	}

	return EXIT_SUCCESS;
}
