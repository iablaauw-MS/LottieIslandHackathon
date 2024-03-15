﻿#pragma once

#include "LottieContentIsland.g.h"

namespace winrt::Microsoft::UI::Xaml::Controls
{
    interface IAnimatedVisualSource;
}

namespace winrt::LottieIsland::implementation
{
    struct LottieContentIsland : LottieContentIslandT<LottieContentIsland>
    {
        LottieContentIsland(const winrt::Compositor& compositor);

        int32_t MyProperty();
        void MyProperty(int32_t value);

        winrt::ContentIsland Island() const
        {
            return m_island;
        }

        winrt::Microsoft::UI::Xaml::Controls::IAnimatedVisualSource AnimatedVisualSource() const;
        void AnimatedVisualSource(const winrt::Microsoft::UI::Xaml::Controls::IAnimatedVisualSource& source);

        winrt::Windows::Foundation::TimeSpan Duration() const;

        bool IsAnimationLoaded() const;

        bool IsPlaying() const;

        void Pause();

        winrt::Windows::Foundation::IAsyncAction PlayAsync(double fromProgress, double toProgress, bool looped);

        void Resume();

        void Stop();

    private:
        void StartAnimation(double fromProgress, double toProgress, bool loop);
        void StopAnimation();

        void OnIslandStateChanged(const winrt::ContentIsland& island, const winrt::ContentIslandStateChangedEventArgs& args);

        void Resize(const float2& size);

        int32_t m_myProperty = 42;
        winrt::Compositor m_compositor{ nullptr };
        winrt::ContainerVisual m_rootVisual{ nullptr };
        winrt::ContentIsland m_island{ nullptr };
        winrt::IAnimatedVisualSource m_animatedVisualSource{ nullptr };
        winrt::IAnimatedVisual m_animatedVisual{ nullptr };
        winrt::CompositionPropertySet m_progressPropertySet{ nullptr };
        winrt::AnimationController m_animationController{ nullptr };
        double m_previousFromProgress = 0.0;
    };
}

namespace winrt::LottieIsland::factory_implementation
{
    struct LottieContentIsland : LottieContentIslandT<LottieContentIsland, implementation::LottieContentIsland>
    {
    };
}
