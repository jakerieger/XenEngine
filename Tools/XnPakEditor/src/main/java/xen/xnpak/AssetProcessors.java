package xen.xnpak;

import org.jetbrains.annotations.Contract;
import org.jetbrains.annotations.NotNull;

public class AssetProcessors {
    @NotNull
    @Contract(value = "_ -> new", pure = true)
    public static byte[] processTexture(String filename) {
        return new byte[0];
    }

    @NotNull
    @Contract(value = "_ -> new", pure = true)
    public static byte[] processAudio(String filename) {
        return new byte[0];
    }

    @NotNull
    @Contract(value = "_ -> new", pure = true)
    public static byte[] processData(String filename) {
        return new byte[0];
    }
}
