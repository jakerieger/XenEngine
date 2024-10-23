using System;
using System.Runtime.CompilerServices;

namespace XnPak;

public static class Utilities {
    public static void Panic(
        string message,
        [CallerFilePath] string file = "",
        [CallerLineNumber] int line = 0,
        [CallerMemberName] string member = "") {
        Console.Error.WriteLine($"{file}({line}) :: PANIC");
        Console.Error.WriteLine($"  In: {member}");
        Console.Error.WriteLine($"  Message: {message}");
        Environment.FailFast("XnPak panicked.");
    }
}