// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#include <format>
#include "time.h"

namespace koji::backend::utils
{
std::string formatTime(const float seconds)
{
    if (seconds < 0.0f)
        return "--:--";

    int total   = static_cast<int>(seconds);
    int hours   = total / 3600;
    int minutes = (total % 3600) / 60;
    int secs    = total % 60;

    if (hours > 0)
        return std::format("{:02}:{:02}:{:02}", hours, minutes, secs);
    return std::format("{:02}:{:02}", minutes, secs);
}
} // namespace koji::backend::utils
