using CommunityToolkit.WinUI.Lottie;
using Microsoft.UI.Composition;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Hosting;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace NiceFriendlyCSharpApp
{
    /// <summary>
    /// An example of how Lottie is used at the Composition Layer in C#
    /// </summary>
    public sealed partial class MainWindow : Window
    {
        Compositor m_compositor;
        ContainerVisual m_rootVisual;

        public MainWindow()
        {
            this.InitializeComponent();
            this.Activated += MainWindow_Activated;
        }

        private void MainWindow_Activated(object sender, WindowActivatedEventArgs args)
        {
            m_compositor = this.Compositor;
            m_rootVisual = m_compositor.CreateContainerVisual();
            ElementCompositionPreview.SetElementChildVisual(VisualHost, m_rootVisual);

            LottieVisualSource lottieVisualSource = LottieVisualSource.CreateFromString("ms-appx:///Assets/LottieLogo1.json");

            lottieVisualSource.AnimatedVisualInvalidated += LottieVisualSource_AnimatedVisualInvalidated;
        }

        private void LottieVisualSource_AnimatedVisualInvalidated(IDynamicAnimatedVisualSource sender, object args)
        {
            object? diagnostics = null;
            IAnimatedVisual? animatedVisual = sender?.TryCreateAnimatedVisual(m_compositor, out diagnostics);
            m_rootVisual?.Children.RemoveAll();

            m_rootVisual?.Children.InsertAtTop(animatedVisual?.RootVisual);

            var animation = m_compositor.CreateScalarKeyFrameAnimation();
            if (animatedVisual != null)
            {
                animation.Duration = animatedVisual.Duration;
                var linearEasing = m_compositor.CreateLinearEasingFunction();

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
