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

/*************************************************************************
**                                                                      **
**                        SAA 9051 control and access                   **
**                                                                      **
*************************************************************************/

void AmigaVideoDevice::init(const bool short_delay)
{
	std::vector<std::uint8_t> saa_slave_receiver
	{
		SAA_SLAVE_ADDRESS, 0
	};

	const std::vector<std::uint8_t> & saa_delay = short_delay ? SAA_short_delay : SAA_long_delay;

	saa_slave_receiver.insert(saa_slave_receiver.end(), saa_delay.begin(), saa_delay.end());

	send(saa_slave_receiver);
}

void AmigaVideoDevice::init_norm(
	const bool color,
	const bool ntsc)
{
	std::vector<std::uint8_t> saa_slave_receiver
	{
		SAA_SLAVE_ADDRESS, 6
	};

	Saa9051Config config{};

	config.horizontal_peaking.PF  = params[input].prefilter;
	config.horizontal_peaking.BY  = 0;
	config.horizontal_peaking.BP  = params[input].bandpass;
	config.horizontal_peaking.COR = params[input].coring;
	config.horizontal_peaking.AP  = params[input].weighting;

	config.hue           = params[input].hue;

	config.control1.FS   = ntsc;
	config.control1.VTR  = params[input].vtr;
	config.control1.CO   = color;
	config.control1.ALT  = !ntsc;
	config.control1.YPN  = 0;
	config.control1.CCFR = 0;

	config.control2.VNL = params[input].vnl;
	config.control2.OEY = 1;
	config.control2.OEC = color;
	config.control2.CI  = 0;
	config.control2.SS  = input;

	config.control3.value = 0;
	config.secam_delay_compensation = 0;

	saa_slave_receiver.insert(saa_slave_receiver.end(),
		config.values, config.values + sizeof(config.values));

	send(saa_slave_receiver);
}

Saa9051Status AmigaVideoDevice::get_status_byte()
{
	static const std::vector<std::uint8_t> saa_slave_transmitter { SAA_SLAVE_ADDRESS | 1 };
	std::vector<std::uint8_t> rx_data   = { 0 };

	receive(saa_slave_transmitter, rx_data);
	Saa9051Status status;

	status.value = rx_data[0];
	return status;
}

void AmigaVideoDevice::scan_sync(
	std::vector<std::uint8_t> & sync_vector,
	const unsigned              max)
{
	for (unsigned i = 0; i < max; i++)
	{
		sync_vector.push_back(get_sync() & (VSYNC | HBLANK));
	}
}

#define DBEQ  false
#define DBNE  true

bool AmigaVideoDevice::sync()
{
#if 1
	wait(VSYNC,  DBNE);
	set_write_yuv();
	wait(HBLANK, DBNE);
	clr_write_yuv();
	wait(VSYNC,  DBEQ);
	enable_write(true);

	wait(VSYNC,  DBNE);
	wait(VSYNC,  DBEQ);
	wait(VSYNC,  DBNE);
	wait(VSYNC,  DBEQ);
	// wait(VSYNC,  DBNE);

	enable_write(false);
#else
	// Wait end of VSync/HBlank
	wait(VSYNC,  DBNE);
	wait(HBLANK, DBNE);

	set_write_yuv();
	clr_write_yuv();

	// Wait start of VSync/HBlank
	wait(VSYNC,  DBEQ);
	wait(HBLANK, DBEQ);

	enable_write(true);

	wait(VSYNC,  DBNE);
	wait(VSYNC,  DBEQ);
	wait(VSYNC,  DBNE);

	enable_write(false);
#endif

	return true;
}

bool AmigaVideoDevice::wait(const std::uint32_t mask, const bool set)
{
	volatile const std::uint8_t * ptr = address<std::uint8_t>(AvdDeviceInfo::IoIndex::VIDEO_SYNC);
	const std::uint8_t            cmp = set ? mask : 0;

	for (unsigned i = 0; i < 65000; i++)
	{
		if ((*ptr & mask) == cmp)
		{
			return true;
		}
	}
	return false;
}

void AmigaVideoDevice::fill()
{
	// usleep(1200000 / 50);
	// enable_write(false);
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

	volatile const std::uint32_t * yPtr = address<std::uint32_t>(AvdDeviceInfo::IoIndex::VRAM_READINCY);
	volatile const std::uint8_t  * uvPtr = address<std::uint8_t>(AvdDeviceInfo::IoIndex::VRAM_READINCUV);

	for (unsigned y = 0; y < ySize; y++)
	{
		for (unsigned x = 0; x < xSize; x += 4)
		{
			const std::uint32_t yyyy = *yPtr << 1;
			const std::uint8_t  uv_h = *uvPtr;
			const std::uint8_t  uv_l = *uvPtr;

			yData.push_back((yyyy >> 24) & 0xff);
			yData.push_back((yyyy >> 16) & 0xff);
			yData.push_back((yyyy >>  8) & 0xff);
			yData.push_back(yyyy         & 0xff);

			uvData.push_back((uv_h & 0xf0) | (uv_l >> 4));
			uvData.push_back((uv_h << 4)   | (uv_l & 0x0f));
		}
	}
}

bool AmigaVideoDevice::select_input(const unsigned int new_input)
{
	if (new_input >= MAX_INPUT)
	{
		return false;
	}
	input = new_input;

	Saa9051Config config{};

	config.control2.VNL = params[input].vnl;
	config.control2.OEY = 1;
	config.control2.OEC = 1;
	config.control2.CI  = 0;
	config.control2.SS  = input;

	const std::vector<__uint8_t> saa_slave_receiver
	{
		SAA_SLAVE_ADDRESS, 0x09, config.control2.value
	};

	return send(saa_slave_receiver);
}

void AmigaVideoDevice::select_param(const Saa9051Param & new_param)
{
	params[input] = new_param;
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
	return read<std::uint8_t>(AvdDeviceInfo::IoIndex::I2C_BUSRECEIVE) & SDA;
}

/*************************************************************************
**                                                                      **
**                        VRAM control                                  **
**                                                                      **
*************************************************************************/

void AmigaVideoDevice::set_write_yuv()
{
	read<std::uint16_t>(AvdDeviceInfo::IoIndex::VRAM_SET_RSTW);
}

void AmigaVideoDevice::clr_write_yuv()
{
	read<std::uint16_t>(AvdDeviceInfo::IoIndex::VRAM_CLR_RSTW);
}

void AmigaVideoDevice::reset_read_yuv()
{
	read<std::uint16_t>(AvdDeviceInfo::IoIndex::VRAM_RESET_READYUV);
}

void AmigaVideoDevice::enable_write(const bool enable)
{
	write<std::uint8_t>(AvdDeviceInfo::IoIndex::VRAM_WRITE_ENABLE, enable);
}

std::uint8_t AmigaVideoDevice::get_sync() const
{
	return read<std::uint8_t>(AvdDeviceInfo::IoIndex::VIDEO_SYNC);
}
