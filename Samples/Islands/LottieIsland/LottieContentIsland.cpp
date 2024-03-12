#include "pch.h"
#include "LottieContentIsland.h"
#include "LottieContentIsland.g.cpp"

namespace winrt::LottieIsland::implementation
{
    LottieContentIsland::LottieContentIsland(
        const winrt::Compositor& compositor)
        : m_compositor(compositor)
    {
        m_rootVisual = m_compositor.CreateContainerVisual();
        m_island = winrt::ContentIsland::Create(m_rootVisual);

        // TODO: set up a red tree
    }

    int32_t LottieContentIsland::MyProperty()
    {
        return m_myProperty;
    }

    void LottieContentIsland::MyProperty(int32_t value)
    {
        m_myProperty = value;
    }
}
