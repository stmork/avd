//
//	SPDX-License-Identifier: MIT
//	SPDX-FileCopyrightText: Copyright (C) 2026 Steffen A. Mork
//

#pragma once

#ifndef AVD_I2CDEVICE_H
#define AVD_I2CDEVICE_H

#include <cstdint>
#include <vector>

class I2CDevice
{
	unsigned timeout = 1000;

public:
	void send(const std::vector<std::uint8_t> & tx_data);
	void receive(
		const std::vector<std::uint8_t> & tx_data,
		std::vector<std::uint8_t> & rx_data);

protected:
	virtual ~I2CDevice() = default;

	virtual void         clock(const std::uint8_t state) = 0;
	virtual void         set_data(const std::uint8_t bit) = 0;
	virtual std::uint8_t get_ack() const = 0;
	virtual std::uint8_t get_data() const = 0;

private:
	std::uint8_t send(const std::uint8_t byte);
	std::uint8_t receive(const bool ack);
	void         start();
	void         stop();
};

#endif // AVD_I2CDEVICE_H
