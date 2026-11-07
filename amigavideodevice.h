//
//	SPDX-License-Identifier: MIT
//	SPDX-FileCopyrightText: Copyright (C) 2026 Steffen A. Mork
//

#pragma once

#ifndef AVD_AMIGAVIDEODEVICE_H
#define AVD_AMIGAVIDEODEVICE_H

#include "i2cdevice.h"
#include "avddeviceinfo.h"

class AmigaVideoDevice : public I2CDevice
{
	static constexpr std::uint8_t SDA    = 1 << 0;
	static constexpr std::uint8_t SCL    = 1 << 1;

	static constexpr std::uint8_t VSYNC  = 1 << 0;
	static constexpr std::uint8_t HBLANK = 1 << 1;

	volatile std::uint8_t * io_address = nullptr;
	std::uint16_t     *     io_offsets = nullptr;

	std::uint8_t            state = 0;

public:
	AmigaVideoDevice() = delete;
	AmigaVideoDevice(
		volatile std::uint8_t * io_address_param,
		std::uint16_t     *     io_offsets_param) :
		io_address(io_address_param), io_offsets(io_offsets_param)
	{
	}

protected:
	virtual void clock(const std::uint8_t scl) override;
	virtual void set_data(const std::uint8_t sda) override;
	virtual std::uint8_t get_ack() const override;
	virtual std::uint8_t get_data() const override;

private:
	template<typename T>
	inline T read(const AvdDeviceInfo::IoIndex index) const
	{
		auto * ptr = reinterpret_cast<volatile const T *>(
				io_address + io_offsets[(AvdDeviceInfo::IoIndexType)index]);

		return *ptr;
	}

	template<typename T>
	inline void write(const AvdDeviceInfo::IoIndex index, const T value)
	{
		auto * ptr = reinterpret_cast<volatile T *>(
				io_address + io_offsets[(AvdDeviceInfo::IoIndexType)index]);

		*ptr = value;
	}

	void set_write_yuv();
	void clr_write_yuv();
	void reset_read_yuv();
	void enable_write(const bool enable);
	std::uint8_t get_sync() const;
};

#endif // AVD_AMIGAVIDEODEVICE_H
