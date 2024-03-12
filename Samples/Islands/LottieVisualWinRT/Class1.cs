using System.Diagnostics;

namespace LottieVisualWinRT
{
    // Dummy WinRT object that we are trying to reference from a C++ project
    public sealed class Class1
    {
        public int SampleProperty { get; set; }

        public static void SayHello()
        {
            Debug.WriteLine("Hello from your C# WinRT component");
        }
    }
}
