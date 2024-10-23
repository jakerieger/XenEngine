using SixLabors.ImageSharp;
using SixLabors.ImageSharp.PixelFormats;

namespace XnPak.Importers;

public static class TextureImporter {
    public static byte[] Import(string filename) {
        using (var image = Image.Load<Rgba32>(filename)) {
            var width = image.Width;
            var height = image.Height;

            var pixels = new byte[width * height * 4];
            image.CopyPixelDataTo(pixels);

            return pixels;
        }
    }
}