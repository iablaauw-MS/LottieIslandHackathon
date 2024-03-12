#pragma once

#include "LottieContentIsland.g.h"

namespace winrt::LottieIsland::implementation
{
    struct LottieContentIsland : LottieContentIslandT<LottieContentIsland>
    {
        LottieContentIsland() = default;

        int32_t MyProperty();
        void MyProperty(int32_t value);

    private:
        int32_t m_myProperty = 42;
    };
}

namespace winrt::LottieIsland::factory_implementation
{
    struct LottieContentIsland : LottieContentIslandT<LottieContentIsland, implementation::LottieContentIsland>
    {
    };
}
