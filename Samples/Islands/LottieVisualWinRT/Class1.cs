using CommunityToolkit.WinUI.Lottie;
using Microsoft.UI.Composition;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Hosting;
using System.Diagnostics;

namespace LottieVisualWinRT
{
    // Dummy WinRT object that we are trying to reference from a C++ project
    public sealed class Class1
    {
        private Compositor? _compositor;
        private ContainerVisual? _rootVisual;
        private LottieVisualSource? _lottieVisualSource;

        public int SampleProperty { get; set; }

        public static void SayHello()
        {
            Debug.WriteLine("Hello from your C# WinRT component");
        }

        public static IAnimatedVisualSource? LoadLottie(string uri, Compositor compositor)
        {
            return LottieVisualSource.CreateFromString(uri);
        }

        public void SetUpLottie(Compositor compositor, ContainerVisual parent, String uri)
        {
            _compositor = compositor;
            _rootVisual = parent;

            _lottieVisualSource = LottieVisualSource.CreateFromString(uri);
            if (_lottieVisualSource != null)
            {
                _lottieVisualSource.AnimatedVisualInvalidated += LottieVisualSource_AnimatedVisualInvalidated;
                object? diagnostics = null;
                if (_lottieVisualSource.TryCreateAnimatedVisual(_compositor, out diagnostics) != null)
                {
                    LottieVisualSource_AnimatedVisualInvalidated(_lottieVisualSource, null);
                }
            }
        }

        private void LottieVisualSource_AnimatedVisualInvalidated(IDynamicAnimatedVisualSource? sender, object? args)
        {
            object? diagnostics = null;
            IAnimatedVisual? animatedVisual = sender?.TryCreateAnimatedVisual(_compositor, out diagnostics);

            if (_rootVisual != null)
            {
                _rootVisual.Children.RemoveAll();
                _rootVisual.Children.InsertAtTop(animatedVisual?.RootVisual);
                Debug.WriteLine("Added Lottie visual to root. beep boop");

                if (_compositor != null)
                {
                    var animation = _compositor.CreateScalarKeyFrameAnimation();
                    if (animatedVisual != null)
                    {
                        animation.Duration = animatedVisual.Duration;
                        var linearEasing = _compositor.CreateLinearEasingFunction();

                        // Play from beginning to end.
                        animation.InsertKeyFrame(0, 0);
                        animation.InsertKeyFrame(1, 1, linearEasing);

                        animation.IterationBehavior = AnimationIterationBehavior.Forever;

                        // Start the animation and get the controller.
                        animatedVisual.RootVisual.Properties.StartAnimation("Progress", animation);
                    }
                }
            }
        }
    }
}
