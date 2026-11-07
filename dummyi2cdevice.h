//
//	SPDX-License-Identifier: MIT
//	SPDX-FileCopyrightText: Copyright (C) 2026 Steffen A. Mork
//

#pragma once

#ifndef AVD_DUMMYI2CDEVICE_H
#define AVD_DUMMYI2CDEVICE_H

#include "i2cdevice.h"

class DummyI2cDevice : public I2CDevice
{
	std::uint8_t state = 0;
	std::uint8_t byte  = 0;
	mutable std::uint8_t bit   = 0;

	static constexpr std::uint8_t SDA = 1 << 0;
	static constexpr std::uint8_t SCL = 1 << 1;

public:
	DummyI2cDevice() = default;

	void set_dummy(const std::uint8_t dummy);

protected:
	virtual void         clock(const std::uint8_t scl) override;
	virtual void         set_data(const std::uint8_t sda) override;
	virtual std::uint8_t get_ack() const override;
	virtual std::uint8_t get_data() const override;
	virtual void         print(const char * text) const override;

	void print() const;
};

#endif // AVD_DUMMYI2CDEVICE_H
