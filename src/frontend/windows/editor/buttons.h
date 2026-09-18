// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray

#include "../../../backend/app.h"

namespace koji::frontend::windows::editor
{
void buttonUp(koji::backend::app::EditorContext &context, float button_size);
void buttonDown(koji::backend::app::EditorContext &context, float button_size);
void buttonLeft(koji::backend::app::EditorContext &context, float button_size);
void buttonRight(koji::backend::app::EditorContext &context, float button_size);
void buttonSave(koji::backend::app::AppState &state, float button_size);
void buttonCancel(koji::backend::app::AppState &state, float button_size);
} // namespace koji::frontend::windows::editor