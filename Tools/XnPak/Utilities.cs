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
        Environment.Exit(-1);
    }
}

public readonly struct Result<T, E> {
    public readonly T Value;
    public readonly E Error;

    private Result(T v, E e, bool success) {
        Value = v;
        Error = e;
        IsOk = success;
    }

    public bool IsOk { get; }

    public static Result<T, E> Ok(T v) {
        return new Result<T, E>(v, default, true);
    }

    public static Result<T, E> Err(E e) {
        return new Result<T, E>(default, e, false);
    }

    public static implicit operator Result<T, E>(T v) {
        return new Result<T, E>(v, default, true);
    }

    public static implicit operator Result<T, E>(E e) {
        return new Result<T, E>(default, e, false);
    }

    public R Match<R>(
        Func<T, R> success,
        Func<E, R> failure) {
        return IsOk ? success(Value) : failure(Error);
    }
}