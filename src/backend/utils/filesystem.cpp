// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#include "filesystem.h"

using namespace std;

namespace koji::backend::utils
{
filesystem::path xdgConfigDir()
{
    filesystem::path config_path;

    const char *xdg_config = getenv("XDG_CONFIG_HOME");
    if (xdg_config && xdg_config[0] != '\0')
        config_path = filesystem::path(xdg_config);

    const char *home = std::getenv("HOME");
    if (home && home[0] != '\0')
        config_path = std::filesystem::path(home) / ".config";

    return config_path;
}
} // namespace koji::backend::utils
