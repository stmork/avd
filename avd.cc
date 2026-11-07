//
//	SPDX-License-Identifier: MIT
//	SPDX-FileCopyrightText: Copyright (C) 2026 Steffen A. Mork
//

#include <cstdlib>
#include <cstdio>

#include "dummyi2cdevice.h"
#include "videodevicefactory.h"

std::vector<std::uint8_t> tx_data   = { 0x8a, 0x00, 0x01, 0x02 };
std::vector<std::uint8_t> rx_header = { 0x8b };
std::vector<std::uint8_t> rx_data   = { 0 };

#define _DEBUG

#ifdef _DEBUG
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

int main()
{
	VideoDeviceFactory        factory;
	std::vector<std::uint8_t> y;
	std::vector<std::uint8_t> uv;

#ifdef _DEBUG
	TestDummy();
#endif
	auto * device = factory.get()[0];

	device->init();
	const std::uint8_t status = device->get_status_byte();

	printf("Status: 0x%02x\n", status);
	if (device->sync())
	{
		device->fill();
		device->read_yuv(y, uv);
	}

	return EXIT_SUCCESS;
}
