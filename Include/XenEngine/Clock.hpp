// Author: Jake Rieger
// Created: 11/19/2024.
//

#pragma once

#define TYPES_ALL
#include <Types/Types.h>

namespace Xen {
    class Clock {
    public:
        Clock() = default;

        /// @brief Returns frame delta time in seconds.
        [[nodiscard]] f32 GetDeltaTime() const;

        /// @brief Returns frame time in seconds.
        [[nodiscard]] f32 GetFrameTime() const;

        /// @brief Returns the elapsed time between now and the clock starting.
        [[nodiscard]] i64 GetTotalTime() const;

        /// @brief Returns the frame rate in frames per second.
        [[nodiscard]] f32 GetFrameRate() const;

        /// @brief Starts the clock.
        void Start();

        /// @brief Stops the clock.
        void Stop();

        /// @brief Tick frame counter one. Updates delta time and capture the start time of the
        /// frame.
        /// @note This should be called at the beginning of your frame.
        void Tick();

        /// @brief Captures the end time of the frame and updates frame time.
        /// @note This should be called at the end of your frame.
        void Update();

        /// @brief Reads the value of the CPU's time-stamp counter.
        static u64 CpuTimestamp();

    private:
        using ClockTime = std::chrono::time_point<std::chrono::steady_clock>;

        bool mRunning  = false;
        f32 mDeltaTime = 0;
        f32 mFrameTime = 0;
        ClockTime mLastTime;
        ClockTime mStartTime;
    };
}  // namespace Xen