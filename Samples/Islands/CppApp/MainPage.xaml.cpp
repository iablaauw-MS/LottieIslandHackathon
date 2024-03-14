// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "MainPage.xaml.h"
#if __has_include("MainPage.g.cpp")
#include "MainPage.g.cpp"
#endif

#include <winrt/Microsoft.UI.Xaml.Hosting.h>

namespace winrt
{
    using namespace Microsoft::UI::Composition;
    using namespace Microsoft::UI::Xaml;
    using namespace Microsoft::UI::Xaml::Controls;
    using namespace Microsoft::UI::Xaml::Hosting;
    using namespace Microsoft::UI::Xaml::Media;
    using namespace Microsoft::UI::Xaml::Media::Animation;
    using namespace Microsoft::UI::Xaml::Navigation;
    using namespace Windows::UI::Xaml::Interop;
}

namespace winrt::CppApp::implementation
{
    CppApp::MainPage MainPage::current{ nullptr };

    MainPage::MainPage()
    {
        InitializeComponent();
        MainPage::current = *this;
        LottieVisualWinRT::Class1::SayHello();

        m_class1 = LottieVisualWinRT::Class1();
        if (m_class1 != nullptr)
        {
            OutputDebugString(L"Meep\n");
        }

        m_compositor = ElementCompositionPreview::GetElementVisual(MyGrid()).Compositor();
        m_rootVisual = m_compositor.CreateContainerVisual();
        ElementCompositionPreview::SetElementChildVisual(MyGrid(), m_rootVisual);
        m_class1.SetUpLottie(m_compositor, m_rootVisual, L"ms-appx:///LottieLogo1.json");
    }
}
