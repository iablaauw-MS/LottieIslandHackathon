#pragma once

#include "LottieContentIsland.g.h"

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

    private:
        void InitializeTree();

        int32_t m_myProperty = 42;
        winrt::Compositor m_compositor{ nullptr };
        winrt::ContainerVisual m_rootVisual{ nullptr };
        winrt::ContentIsland m_island{ nullptr };
    };
}

namespace winrt::LottieIsland::factory_implementation
{
    struct LottieContentIsland : LottieContentIslandT<LottieContentIsland, implementation::LottieContentIsland>
    {
    };
}
