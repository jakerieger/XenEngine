using System;
using System.Collections.Generic;
using System.IO;
using CommandLine;

namespace XnPak;

internal abstract class Program {
    public static void buildManifest(ref Manifest manifest) {
        Console.WriteLine("- Building manifest.");

        var contentDir = Path.Combine(manifest.RootDirectory, manifest.OutputDirectory);
        if (Directory.Exists(contentDir)) Directory.Delete(contentDir, true);
        Directory.CreateDirectory(contentDir);

        Console.WriteLine("  | Created output directory: {0}", contentDir);
        Console.WriteLine("  | Found {0} assets.", manifest.Assets.Length);

        foreach (var asset in manifest.Assets) {
            Console.WriteLine("  | Building asset: {0}", asset.Build);

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
            switch (asset.Type) {
                case AssetType.Texture:
                    break;
                case AssetType.Audio:
                    break;
                case AssetType.Font:
                    break;
                case AssetType.PlainText:
                    break;
            }

            var assetBytes
                = File.ReadAllBytes(Path.Combine(manifest.RootDirectory, asset.Build));

            File.WriteAllBytes(pakFile, assetBytes);

            Console.WriteLine("  | Created asset: {0}", asset.Name);
        }

        Console.WriteLine("- Successfully built manifest.");
    }

    public static void rebuildManifest(ref Manifest manifest) { }

    public static void cleanManifest(ref Manifest manifest) { }

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
            buildManifest(ref manifest);
        else if (opts.Rebuild)
            rebuildManifest(ref manifest);
        else if (opts.Clean) cleanManifest(ref manifest);
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