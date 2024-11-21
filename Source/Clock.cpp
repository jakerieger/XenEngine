// Author: Jake Rieger
// Created: 11/19/2024.
//

#include "Clock.hpp"

#if defined(WIN32) || defined(WIN64)
    #include <intrin.h>
#endif

namespace Xen {
    f32 Clock::GetDeltaTime() const {
        return mDeltaTime;
    }

    f32 Clock::GetFrameTime() const {
        return mFrameTime;
    }

    i64 Clock::GetTotalTime() const {
        if (mRunning) {
            const auto now     = std::chrono::high_resolution_clock::now();
            const auto elapsed = now - mStartTime;
            return elapsed.count();
        }

        return (mLastTime - mStartTime).count();
    }

    f32 Clock::GetFrameRate() const {
        return 1.f / GetDeltaTime();
    }

    void Clock::Start() {
        mRunning   = true;
        mStartTime = std::chrono::high_resolution_clock::now();
        mLastTime  = mStartTime;
    }

    void Clock::Stop() {
        mRunning = false;
    }

    void Clock::Tick() {
        const auto now                             = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<f32> deltaTime = now - mLastTime;
        mDeltaTime                                 = deltaTime.count();
        mLastTime                                  = now;
    }

    void Clock::Update() {
        const auto frameEnd                            = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<f32> frameDuration = frameEnd - mLastTime;
        mFrameTime                                     = frameDuration.count();
    }

    u64 Clock::CpuTimestamp() {
#ifdef _MSC_VER
        return __rdtsc();
#elif defined(__clang__) || defined(__GNUC__)
        u32 lo, hi;
        __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
        return ((u64)hi << 32) | lo;
#else
    #error "Unsupported compiler"
#endif
    }
}  // namespace Xen