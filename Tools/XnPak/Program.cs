using System.IO;

namespace XnPak;

internal abstract class Program {
    public static void buildManifest(ref Manifest manifest) {
        var cwd = Directory.GetCurrentDirectory();
    }

    public static void rebuildManifest(ref Manifest manifest) {
    }

    public static void cleanManifest(ref Manifest manifest) {
    }

    public static void Main(string[] args) {
        var manifest = new Manifest("../../Test/Content.manifest");
        buildManifest(ref manifest);
    }
}