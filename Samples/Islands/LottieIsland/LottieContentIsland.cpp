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

        InitializeTree();

        winrt::LottieVisualWinRT::Class1::SayHello();
    }

    int32_t LottieContentIsland::MyProperty()
    {
        return m_myProperty;
    }

    void LottieContentIsland::MyProperty(int32_t value)
    {
        m_myProperty = value;
    }

    void LottieContentIsland::InitializeTree()
    {
        // Make a blue square with a red square inside of it.
        // Add some animations to the red square

        // 300 x 300 blue background
        auto blueVisual = m_compositor.CreateSpriteVisual();
        auto blueBrush = m_compositor.CreateColorBrush(winrt::Windows::UI::Colors::Blue());
        blueVisual.Brush(blueBrush);
        blueVisual.Size({ 300, 300 });

        m_rootVisual.Children().InsertAtTop(blueVisual);

        // 50 x 50 red square
        auto redVisual = m_compositor.CreateSpriteVisual();
        auto redBrush = m_compositor.CreateColorBrush(winrt::Windows::UI::Colors::Red());
        redVisual.Brush(redBrush);
        redVisual.Size({ 50, 50 });

        m_rootVisual.Children().InsertAtTop(redVisual);

        // Setup an animation

        auto keyFrameAnimation = m_compositor.CreateVector3KeyFrameAnimation();
        keyFrameAnimation.InsertKeyFrame(0.0f, { 0, 0, 0 });
        keyFrameAnimation.InsertKeyFrame(1.0f, { 250.f, 250.f, 0 });

        // Bounce back and forth forever
        keyFrameAnimation.Duration(2000ms);
        keyFrameAnimation.Direction(winrt::AnimationDirection::Alternate);
        keyFrameAnimation.IterationBehavior(winrt::AnimationIterationBehavior::Forever);

        // Start animation
        redVisual.StartAnimation(L"Offset", keyFrameAnimation);

        // Set up lottie
        auto lottieVisual = m_compositor.CreateContainerVisual();
        m_rootVisual.Children().InsertAtTop(lottieVisual);
        winrt::LottieVisualWinRT::Class1 class1;
        class1.SetUpLottie(m_compositor, lottieVisual);
    }
}
