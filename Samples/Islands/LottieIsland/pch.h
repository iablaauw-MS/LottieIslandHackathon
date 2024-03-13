#pragma once

#include <chrono>

#include <unknwn.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/LottieVisualWinRT.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>

namespace winrt
{
    using namespace ::winrt::Microsoft::UI::Composition;
    using namespace ::winrt::Microsoft::UI::Content;
}

// Opt into time literals (i.e. 200ms, 1min, 15s)
using namespace std::chrono_literals;
