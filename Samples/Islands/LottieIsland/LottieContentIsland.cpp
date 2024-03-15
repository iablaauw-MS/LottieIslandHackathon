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
    }

    int32_t LottieContentIsland::MyProperty()
    {
        return m_myProperty;
    }

    void LottieContentIsland::MyProperty(int32_t value)
    {
        m_myProperty = value;
    }

    winrt::Microsoft::UI::Xaml::Controls::IAnimatedVisualSource LottieContentIsland::AnimatedVisualSource() const
    {
        // Return the AnimatedVisualSource
        return m_animatedVisualSource;
    }

    void LottieContentIsland::AnimatedVisualSource(winrt::Microsoft::UI::Xaml::Controls::IAnimatedVisualSource const& value)
    {
        // Stop any running animation, if any currently is happening.
        StopAnimation();

        // Set the AnimatedVisualSource
        m_animatedVisualSource = value;
        winrt::Windows::Foundation::IInspectable diagnostics;
        m_animatedVisual = m_animatedVisualSource.TryCreateAnimatedVisual(m_compositor, diagnostics);

        // Set up lottie
        m_rootVisual.Children().InsertAtTop(m_animatedVisual.RootVisual());

        StartAnimation(0.0, 1.0, true /*loop*/);
    }

    winrt::Windows::Foundation::TimeSpan LottieContentIsland::Duration() const
    {
        if (m_animatedVisual == nullptr)
        {
            return 0ms;
        }

        return m_animatedVisual.Duration();
    }

    bool LottieContentIsland::IsAnimationLoaded() const
    {
        // Revisit this when we get JSON loading to work.
        return m_animatedVisual != nullptr;
    }

    bool LottieContentIsland::IsPlaying() const
    {
        return m_progressPropertySet != nullptr;
    }

    void LottieContentIsland::Pause()
    {
        if (m_animationController != nullptr)
        {
            m_animationController.Pause();
        }
    }

    winrt::Windows::Foundation::IAsyncAction LottieContentIsland::PlayAsync(double fromProgress, double toProgress, bool looped)
    {
        // TODO: actually implement this properly using composition batches.

        StartAnimation(fromProgress, toProgress, looped);
        co_return;
    }

    void LottieContentIsland::Resume()
    {
        if (m_animationController != nullptr)
        {
            m_animationController.Pause();
        }
    }

    void LottieContentIsland::Stop()
    {
        StopAnimation();
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
    }

    void LottieContentIsland::StartAnimation(double fromProgress, double toProgress, bool loop)
    {
        if (m_animatedVisual == nullptr)
        {
            throw winrt::hresult_illegal_method_call{ L"Cannot start an animation before the animation is loaded." };
        }

        auto animation = m_compositor.CreateScalarKeyFrameAnimation();
        animation.Duration(m_animatedVisual.Duration());
        auto linearEasing = m_compositor.CreateLinearEasingFunction();
        animation.InsertKeyFrame(0, fromProgress);
        animation.InsertKeyFrame(1, toProgress, linearEasing);
        if (loop)
        {
            animation.IterationBehavior(winrt::AnimationIterationBehavior::Forever);
        }
        else
        {
            animation.IterationBehavior(winrt::AnimationIterationBehavior::Count);
            animation.IterationCount(1);
        }

        m_progressPropertySet = m_animatedVisual.RootVisual().Properties();
        m_progressPropertySet.StartAnimation(L"Progress", animation);
        m_animationController = m_progressPropertySet.TryGetAnimationController(L"Progress");
        m_previousFromProgress = fromProgress;
    }

    void LottieContentIsland::StopAnimation()
    {
        if (m_progressPropertySet == nullptr)
        {
            // No-op
            return;
        }

        // Stop and snap to the beginning of the animation
        m_progressPropertySet.StopAnimation(L"Progress");
        m_progressPropertySet.InsertScalar(L"Progress", m_previousFromProgress);

        // Cleanup
        m_previousFromProgress = 0.0;
        m_animationController = nullptr;
        m_progressPropertySet = nullptr;
    }
}
