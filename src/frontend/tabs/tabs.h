// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#pragma once

#include "../../backend/app.h"

namespace koji::frontend::tabs
{
void queueTab(koji::backend::app::AppState &state);
void albumTab(koji::backend::app::AppState &state);
void playlistTab(koji::backend::app::AppState &state);
} // namespace koji::frontend::tabs

namespace koji::frontend::tabs::internal
{
bool beginTab(const char *label);
void endTab();
} // namespace koji::frontend::tabs::internal