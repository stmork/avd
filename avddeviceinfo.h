//
//	SPDX-License-Identifier: MIT
//	SPDX-FileCopyrightText: Copyright (C) 2026 Steffen A. Mork
//

#pragma once

#ifndef AVD_AVDDEVICEINFO_H
#define AVD_AVDDEVICEINFO_H

#include <cstdint>
#include <type_traits>
#include <unordered_map>

struct AvdDeviceInfo
{
	enum class IoIndex : unsigned
	{
		I2C_BUSSEND,
		I2C_BUSRECEIVE,

		VRAM_READINCY,
		VRAM_READINCUV,

		VRAM_SETRSTW,
		VRAM_CLRRSTW,
		VRAM_RESETREADYUV,
		VRAM_WRITE_ENABLE,

		VIDEO_SYNC,

		IO_MAX_INDEX
	};

	using IoIndexType = std::underlying_type_t<IoIndex>;

	const char     *    name;
	const std::uint16_t io_offsets[(unsigned)IoIndex::IO_MAX_INDEX];
	const std::uint16_t vendor_id;
	const std::uint8_t  product_id;
};

// This is a shortcut to of the key map.
typedef std::pair<decltype(AvdDeviceInfo::vendor_id), decltype(AvdDeviceInfo::product_id)> AutoConfigId;

// This is a container to handle the hash value
class AutoConfigKey : public AutoConfigId
{
public:
	explicit constexpr AutoConfigKey() : AutoConfigId(0, 0)
	{
	}

	explicit constexpr AutoConfigKey(
		decltype(AvdDeviceInfo::vendor_id)  vendor_id,
		decltype(AvdDeviceInfo::product_id) product_id) :
		AutoConfigId(vendor_id, product_id)
	{
	}

	explicit constexpr AutoConfigKey(const AvdDeviceInfo & info) :
		AutoConfigId(info.vendor_id, info.product_id)
	{
	}

	std::size_t operator()(const AutoConfigKey & p) const
	{
		return (p.second << 16) | p.first;
	}
};

// Maps from AutoConfigId to AvdDeviceInfo using the AutoConfigKey hasher.
extern const std::unordered_map<AutoConfigKey, const AvdDeviceInfo *, AutoConfigKey> avd_map;

#endif // AVD_AVDDEVICEINFO_H
