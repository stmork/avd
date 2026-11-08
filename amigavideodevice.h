//
//	SPDX-License-Identifier: MIT
//	SPDX-FileCopyrightText: Copyright (C) 2026 Steffen A. Mork
//

#pragma once

#ifndef AVD_AMIGAVIDEODEVICE_H
#define AVD_AMIGAVIDEODEVICE_H

#include "i2cdevice.h"
#include "avddeviceinfo.h"

class AmigaVideoDevice : protected I2CDevice
{
	static constexpr std::uint8_t SDA    = 1 << 0;
	static constexpr std::uint8_t SCL    = 1 << 1;

	static constexpr std::uint8_t VSYNC  = 1 << 0;
	static constexpr std::uint8_t HBLANK = 1 << 1;

	volatile std::uint8_t  * io_address = nullptr;
	const    std::uint16_t * io_offsets = nullptr;

	std::uint8_t             state = 0;

	static const std::uint8_t SAA_SLAVE_ADDRESS = 0x8a;

public:
	static const     unsigned     xSize  = 720;
	static const     unsigned     ySize  = 576;

	AmigaVideoDevice() = delete;
	AmigaVideoDevice(
		volatile std::uint8_t * io_address_param,
		const std::uint16_t  *  io_offsets_param) :
		io_address(io_address_param), io_offsets(io_offsets_param)
	{
	}

	void init(const bool pal = true, const bool short_delay = true);
	std::uint8_t get_status_byte();
	bool sync();
	bool wait(const std::uint32_t mask, const bool set);

	void fill();
	void read_yuv(
		std::vector<std::uint8_t> & yData,
		std::vector<std::uint8_t> & uvData);

protected:
	virtual void clock(const std::uint8_t scl) override;
	virtual void set_data(const std::uint8_t sda) override;
	virtual std::uint8_t get_ack() const override;
	virtual std::uint8_t get_data() const override;

private:
	template<typename T>
	inline constexpr T read(const AvdDeviceInfo::IoIndex index) const
	{
		auto * ptr = reinterpret_cast<volatile const T *>(
				io_address + io_offsets[(AvdDeviceInfo::IoIndexType)index]);

		return *ptr;
	}

	template<typename T>
	inline constexpr void write(const AvdDeviceInfo::IoIndex index, const T value)
	{
		auto * ptr = reinterpret_cast<volatile T *>(
				io_address + io_offsets[(AvdDeviceInfo::IoIndexType)index]);

		*ptr = value;
	}

	static const std::vector<std::uint8_t> SAA_short_delay;
	static const std::vector<std::uint8_t> SAA_long_delay;

	static const std::vector<std::uint8_t> SAA_PAL;
	static const std::vector<std::uint8_t> SAA_NTSC;

	void set_write_yuv();
	void clr_write_yuv();
	void reset_read_yuv();
	void enable_write(const bool enable);
	std::uint8_t get_sync() const;
};

#endif // AVD_AMIGAVIDEODEVICE_H
