//
//	SPDX-License-Identifier: MIT
//	SPDX-FileCopyrightText: Copyright (C) 2026 Steffen A. Mork
//

#include "amigavideodevice.h"

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
