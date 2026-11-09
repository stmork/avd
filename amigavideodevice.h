//
//	SPDX-License-Identifier: MIT
//	SPDX-FileCopyrightText: Copyright (C) 2026 Steffen A. Mork
//

#pragma once

#ifndef AVD_AMIGAVIDEODEVICE_H
#define AVD_AMIGAVIDEODEVICE_H

#include "i2cdevice.h"
#include "avddeviceinfo.h"

union Saa9051Status
{
	struct
	{
		std::uint8_t PONRES : 1;
		std::uint8_t HLOCK  : 1;
		std::uint8_t ONE5   : 1;
		std::uint8_t FD     : 1;
		std::uint8_t ZERO3  : 1;
		std::uint8_t CD     : 1;
		std::uint8_t CS     : 1;
		std::uint8_t ZERO0  : 1;
	};
	std::uint8_t value;
};

union Saa9051Config
{
	struct
	{
		union
		{
			struct
			{
				std::uint8_t BY  : 1;
				std::uint8_t PF  : 1;
				std::uint8_t BP  : 2;
				std::uint8_t COR : 2;
				std::uint8_t AP  : 2;
			};
			std::uint8_t value;
		} horizontal_peaking;

		std::uint8_t hue;

		union
		{
			struct
			{
				std::uint8_t HPLL : 1;
				std::uint8_t FS   : 1;
				std::uint8_t VTR  : 1;
				std::uint8_t CO   : 1;
				std::uint8_t ALT  : 1;
				std::uint8_t YPN  : 1;
				std::uint8_t CCFR : 2;
			};
			std::uint8_t value;
		} control1;

		union
		{
			struct
			{
				std::uint8_t VNL  : 1;
				std::uint8_t OEY  : 1;
				std::uint8_t OEC  : 1;
				std::uint8_t X    : 1;
				std::uint8_t CI   : 1;
				std::uint8_t AFCC : 1;
				std::uint8_t SS   : 2;
			};
			std::uint8_t value;
		} control2;

		union
		{
			struct
			{
				std::uint8_t SYC  : 1;
				std::uint8_t CT   : 1;
				std::uint8_t YC   : 1;
				std::uint8_t SS   : 2;
				std::uint8_t YDL  : 3;
			};
			std::uint8_t value;
		} control3;

		std::uint8_t secam_delay_compensation;
	};
	std::uint8_t values[6];
};

struct Saa9051Param
{
	enum Bandpass : std::uint8_t
	{
		BP_4_1,
		BP_3_8,
		BP_2_6,
		BP_2_9
	};

	enum Coring : std::uint8_t
	{
		COR_OFF,
		COR_4,
		COR_5,
		COR_6
	};

	enum Weighting : std::uint8_t
	{
		WEIGHT_0,
		WEIGHT_0_25,
		WEIGHT_0_5,
		WEIGHT_1
	};

	Bandpass    bandpass  = BP_4_1;
	Coring      coring    = COR_OFF;
	Weighting   weighting = WEIGHT_0;
	std::int8_t hue       = 0;
	bool        prefilter = false;
	bool        vtr       = true;
	bool        vnl       = true;
};

class AmigaVideoDevice : protected I2CDevice
{
	static constexpr std::uint8_t SDA    = 1 << 0;
	static constexpr std::uint8_t SCL    = 1 << 1;

	static constexpr std::uint8_t VSYNC  = 1 << 0;
	static constexpr std::uint8_t HBLANK = 1 << 1;

	static constexpr std::uint8_t SAA_SLAVE_ADDRESS = 0x8a;

	volatile std::uint8_t    *    io_address = nullptr;
	const    std::uint16_t    *   io_offsets = nullptr;

	std::uint8_t                  state = 0;
	std::uint8_t                  input = 0;

public:
	static constexpr unsigned     xSize     = 720;
	static constexpr unsigned     ySize     = 625;

	static constexpr unsigned     MAX_INPUT = 2;

	AmigaVideoDevice() = delete;
	AmigaVideoDevice(
		volatile std::uint8_t * io_address_param,
		const std::uint16_t  *  io_offsets_param) :
		io_address(io_address_param), io_offsets(io_offsets_param)
	{
	}

	void init(const bool short_delay = true);
	void init_norm(
		const bool color,
		const bool ntsc = false);

	Saa9051Status get_status_byte();
	void scan_sync(
		std::vector<std::uint8_t> & data,
		const unsigned              max = 3000);

	bool sync();
	bool wait(const std::uint32_t mask, const bool set);

	void fill();
	void read_yuv(
		std::vector<std::uint8_t> & yData,
		std::vector<std::uint8_t> & uvData);

	bool select_input(const unsigned new_input);
	void select_param(const Saa9051Param & new_param);

protected:
	virtual void clock(const std::uint8_t scl) override;
	virtual void set_data(const std::uint8_t sda) override;
	virtual std::uint8_t get_ack() const override;
	virtual std::uint8_t get_data() const override;

private:

	template<typename T>
	inline constexpr volatile T * address(const AvdDeviceInfo::IoIndex index) const
	{
		return reinterpret_cast<volatile T *>(
				io_address + io_offsets[(AvdDeviceInfo::IoIndexType)index]);
	}

	template<typename T>
	inline constexpr T read(const AvdDeviceInfo::IoIndex index) const
	{
		volatile const T * ptr = address<T>(index);

		return *ptr;
	}

	template<typename T>
	inline constexpr void write(const AvdDeviceInfo::IoIndex index, const T value)
	{
		volatile T * ptr = address<T>(index);

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

	Saa9051Param              params[MAX_INPUT];
};

#endif // AVD_AMIGAVIDEODEVICE_H
