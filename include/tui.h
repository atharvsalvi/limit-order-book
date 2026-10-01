#pragma once

#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

#include <atomic>
#include <sstream>
#include <thread>

#include "dashboard.h"

void startTUI();
void refreshBook();
void stopTUI();