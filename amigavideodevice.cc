//
//	SPDX-License-Identifier: MIT
//	SPDX-FileCopyrightText: Copyright (C) 2026 Steffen A. Mork
//

#include <unistd.h>

#include "amigavideodevice.h"

/*************************************************************************
**                                                                      **
**                        SAA 9051 configuration data                   **
**                                                                      **
*************************************************************************/

const std::vector<std::uint8_t> AmigaVideoDevice::SAA_short_delay
{
	0x5e, 0x37, 0x07, 0xf6, 0xc7, 0xff
};

const std::vector<std::uint8_t> AmigaVideoDevice::SAA_long_delay
{
	0x7e, 0x73, 0x43, 0x32, 0x03, 0xff
};

const std::vector<std::uint8_t> AmigaVideoDevice::SAA_PAL
{
	0x02, 0x00, 0x38, 0xe3, 0x58, 0x00
};

const std::vector<std::uint8_t> AmigaVideoDevice::SAA_NTSC
{
	0x62, 0x00, 0x77, 0xe3, 0x58, 0x00
};

/*************************************************************************
**                                                                      **
**                        SAA 9051 control and access                   **
**                                                                      **
*************************************************************************/

void AmigaVideoDevice::init()
{
	std::vector<std::uint8_t> saa_slave_receiver
	{
		0x8a, 0
	};

	saa_slave_receiver.insert(saa_slave_receiver.end(), SAA_short_delay.begin(), SAA_short_delay.end());
	saa_slave_receiver.insert(saa_slave_receiver.end(), SAA_PAL.begin(), SAA_PAL.end());

	send(saa_slave_receiver);
}

std::uint8_t AmigaVideoDevice::get_status_byte()
{
	static const std::vector<std::uint8_t> saa_slave_transmitter = { 0x8b };
	std::vector<std::uint8_t> rx_data   = { 0 };

	receive(saa_slave_transmitter, rx_data);

	return rx_data[0];
}

bool AmigaVideoDevice::sync()
{
	wait(VSYNC,  false);
	wait(HBLANK, false);

	set_write_yuv();
	clr_write_yuv();

	wait(VSYNC,  true);
	wait(HBLANK, true);

	enable_write(true);

	return true;
}

bool AmigaVideoDevice::wait(const std::uint32_t mask, const bool set)
{
	const std::uint8_t cmp = set ? mask : 0;

	for (unsigned i = 0; i < 65000; i++)
	{
		if ((get_sync() & mask) == cmp)
		{
			return true;
		}
	}
	return false;
}

void AmigaVideoDevice::fill()
{
	enable_write(true);
	usleep(1000000 / 50 + 5);
	enable_write(false);
}

void AmigaVideoDevice::read_yuv(
	std::vector<std::uint8_t> & yData,
	std::vector<std::uint8_t> & uvData)
{
	reset_read_yuv();
	yData.clear();
	yData.reserve(xSize * ySize);
	uvData.clear();
	uvData.reserve(xSize * ySize / 2);

	for (unsigned y = 0; y < ySize; y++)
	{
		for (unsigned x = 0; x < xSize; x += 4)
		{
			const std::uint32_t yyyy = read<std::uint32_t>(AvdDeviceInfo::IoIndex::VRAM_READINCY) << 1;
			const std::uint8_t  uv_h = read<std::uint8_t>(AvdDeviceInfo::IoIndex::VRAM_READINCUV);
			const std::uint8_t  uv_l = read<std::uint8_t>(AvdDeviceInfo::IoIndex::VRAM_READINCUV);

			yData.push_back((yyyy >> 24) & 0xff);
			yData.push_back((yyyy >> 16) & 0xff);
			yData.push_back((yyyy >>  8) & 0xff);
			yData.push_back(yyyy         & 0xff);

			uvData.push_back((uv_h & 0xf0) | (uv_l >> 4));
			uvData.push_back((uv_h << 4)   | (uv_l & 0x0f));
		}
	}
}

/*************************************************************************
**                                                                      **
**                        i²c                                           **
**                                                                      **
*************************************************************************/

void AmigaVideoDevice::clock(const std::uint8_t scl)
{
	state = (state & SDA) | (scl ? SCL : 0);

	write<std::uint8_t>(AvdDeviceInfo::IoIndex::I2C_BUSSEND, state);
}

void AmigaVideoDevice::set_data(const std::uint8_t sda)
{
	state = (state & SCL) | (sda ? SDA : 0);

	write<std::uint8_t>(AvdDeviceInfo::IoIndex::I2C_BUSSEND, state);
}

std::uint8_t AmigaVideoDevice::get_ack() const
{
	return !get_data();
}

std::uint8_t AmigaVideoDevice::get_data() const
{
	return read<std::uint8_t>(AvdDeviceInfo::IoIndex::I2C_BUSRECEIVE);
}

/*************************************************************************
**                                                                      **
**                        VRAM control                                  **
**                                                                      **
*************************************************************************/

void AmigaVideoDevice::set_write_yuv()
{
	read<std::uint16_t>(AvdDeviceInfo::IoIndex::VRAM_SETRSTW);
}

void AmigaVideoDevice::clr_write_yuv()
{
	read<std::uint16_t>(AvdDeviceInfo::IoIndex::VRAM_CLRRSTW);
}

void AmigaVideoDevice::reset_read_yuv()
{
	read<std::uint16_t>(AvdDeviceInfo::IoIndex::VRAM_RESETREADYUV);
}

void AmigaVideoDevice::enable_write(const bool enable)
{
	write<std::uint8_t>(AvdDeviceInfo::IoIndex::VRAM_WRITE_ENABLE, enable);
}

std::uint8_t AmigaVideoDevice::get_sync() const
{
	return read<std::uint8_t>(AvdDeviceInfo::IoIndex::VIDEO_SYNC);
}
