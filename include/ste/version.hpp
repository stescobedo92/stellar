// SPDX-License-Identifier: MIT
#pragma once

#define STELLAR_VERSION_MAJOR 0
#define STELLAR_VERSION_MINOR 1
#define STELLAR_VERSION_PATCH 0
#define STELLAR_VERSION_STRING "0.1.0"

namespace ste {
inline constexpr int  kVersionMajor = STELLAR_VERSION_MAJOR;
inline constexpr int  kVersionMinor = STELLAR_VERSION_MINOR;
inline constexpr int  kVersionPatch = STELLAR_VERSION_PATCH;
inline constexpr auto kVersion      = STELLAR_VERSION_STRING;
}  // namespace ste
