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

        m_compositor = ElementCompositionPreview::GetElementVisual(MyGrid()).Compositor();
        m_rootVisual = m_compositor.CreateContainerVisual();
        ElementCompositionPreview::SetElementChildVisual(MyGrid(), m_rootVisual);

        winrt::LottieVisualWinRT::LottieAnimatedVisualWinRT lottieAnimatedVisual;
        lottieAnimatedVisual.SetUpLottie(m_compositor, m_rootVisual, L"ms-appx:///LottieLogo1.json");

        // Commented out due to events not working in C# WinRT component code generator
        //winrt::LottieVisualWinRT::LottieVisualSourceWinRT lottieVisualSource();
        //auto token = lottieVisualSource.AnimatedVisualInvalidated([](winrt::LottieVisualWinRT::LottieVisualSourceWinRT&& sender, auto&& args)
        //    {
        //        winrt::Windows::Foundation::IInspectable diagnostics;
        //        // This creates a DependencyObject, which fails if XAML has not been spun up (but in this sample it has been spun up)
        //        winrt::Microsoft::UI::Xaml::Controls::IAnimatedVisual animatedVisual = sender.TryCreateAnimatedVisual(m_compositor, diagnostics);

        //        // Set up lottie
        //        m_rootVisual.Children().InsertAtTop(animatedVisual.RootVisual());
        //        auto animation = m_compositor.CreateScalarKeyFrameAnimation();
        //        animation.Duration(animatedVisual.Duration());
        //        auto linearEasing = m_compositor.CreateLinearEasingFunction();
        //        animation.InsertKeyFrame(0, 0);
        //        animation.InsertKeyFrame(1, 1, linearEasing);
        //        animation.IterationBehavior(winrt::Microsoft::UI::Composition::AnimationIterationBehavior::Forever);
        //        animatedVisual.RootVisual().Properties().StartAnimation(L"Progress", animation);
        //    });
        //lottieVisualSource.LoadLottie(L"ms-appx:///LottieLogo1.json");

        
    }

}
