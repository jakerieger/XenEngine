using CommandLine;
using XnPak.Importers;

namespace XnPak;

internal abstract class Program {
    public static void BuildManifest(ref Manifest manifest) {
        Console.WriteLine("- Building manifest.");

        var contentDir = Path.Combine(manifest.RootDirectory, manifest.OutputDirectory);
        if (Directory.Exists(contentDir)) Directory.Delete(contentDir, true);
        Directory.CreateDirectory(contentDir);

        Console.WriteLine("  | Created output directory: {0}", contentDir);
        Console.WriteLine("  | Found {0} assets.", manifest.Assets.Length);

        var assetCount = manifest.Assets.Length;
        var assetId = 1;
        foreach (var asset in manifest.Assets) {
            Console.WriteLine("  | [{0}/{1}] Building asset: {2}", assetId, assetCount,
                asset.Build);

            string outputPath;
            var fileName = asset.Name;
            if (asset.Name.Contains("/")) {
                // Get subdirectory and create it
                var subdir
                    = asset.Name.Substring(0, asset.Name.IndexOf("/", StringComparison.Ordinal));
                outputPath = Path.Combine(contentDir, subdir);
                fileName = asset.Name.Substring(asset.Name.IndexOf("/", StringComparison.Ordinal) +
                                                1);
                Directory.CreateDirectory(outputPath);
            }
            else
                outputPath = Path.Combine(contentDir, asset.Name);

            var pakFile = Path.Combine(outputPath, string.Format("{0}.pak", fileName));

            // This is the point where the different importer classes will come in handy.
            var bytes = new List<byte>();
            var originalPath = Path.Combine(manifest.RootDirectory, asset.Build);
            switch (asset.Type) {
                case AssetType.Texture:
                    bytes.AddRange(
                        TextureImporter.Import(originalPath));
                    break;
                case AssetType.Audio:
                    break;
                case AssetType.Font:
                    break;
                case AssetType.PlainText:
                    break;
            }

            File.WriteAllBytes(pakFile, bytes.ToArray());
            assetId++;
        }

        Console.WriteLine("- Build successful.");
    }

    public static void RebuildManifest(ref Manifest manifest) { }

    public static void CleanManifest(ref Manifest manifest) { }

    public static void Main(string[] args) {
        Parser.Default.ParseArguments<Options>(args)
            .WithParsed(ParseOptions)
            .WithNotParsed(HandleParseError);
    }

    private static void ParseOptions(Options opts) {
        string manifestPath;
        if (opts.Manifest is not null)
            manifestPath = opts.Manifest;
        else
            manifestPath = Path.Combine(Directory.GetCurrentDirectory(), "Content.manifest");

        if (!File.Exists(manifestPath))
            throw new FileNotFoundException("The manifest file was not found.");
        var manifest = new Manifest(manifestPath);

        Console.WriteLine("- Reading manifest from: {0}", manifestPath);

        if (opts.Build)
            BuildManifest(ref manifest);
        else if (opts.Rebuild)
            RebuildManifest(ref manifest);
        else if (opts.Clean) CleanManifest(ref manifest);
    }

    private static void HandleParseError(IEnumerable<Error> errs) {
        foreach (var err in errs) Console.WriteLine(err.ToString());
    }

    public class Options {
        [Option('m', "manifest", Required = false, HelpText = "Path to the manifest.")]
        public string? Manifest { get; set; }

        [Option('b', "build", Required = false, HelpText = "Build the manifest.",
            Group = "BuildAction")]
        public bool Build { get; set; }

        [Option('r', "rebuild", Required = false, HelpText = "Rebuild the manifest.",
            Group = "BuildAction")]
        public bool Rebuild { get; set; }

        [Option('c', "clean", Required = false, HelpText = "Clean the manifest.",
            Group = "BuildAction")]
        public bool Clean { get; set; }
    }
}