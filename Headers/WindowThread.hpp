#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>

void windowThread(const bool&, const std::mutex&, const std::condition_variable&);