using System;
using System.Collections.Generic;
using System.IO;
using CommandLine;

namespace XnPak;

internal abstract class Program {
    public static void buildManifest(ref Manifest manifest) {
        Console.WriteLine("- Building manifest.");

        var outputDir = Path.Combine(manifest.RootDirectory, manifest.OutputDirectory);
        if (!Directory.Exists(outputDir)) {
            Directory.CreateDirectory(outputDir);
        }
        else {
            Directory.Delete(outputDir, true);
            Directory.CreateDirectory(outputDir);
        }

        Console.WriteLine("  > Created output directory: {0}", outputDir);
        Console.WriteLine("  > Found {0} assets.", manifest.Assets.Length);

        foreach (var asset in manifest.Assets) {
            Console.WriteLine("  > Building asset: {0}", asset.Build);

            Console.WriteLine("  > Created asset: {0}", asset.Name);
        }

        Console.WriteLine("- Successfully built manifest.");
    }

    public static void rebuildManifest(ref Manifest manifest) {
    }

    public static void cleanManifest(ref Manifest manifest) {
    }

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

        if (!File.Exists(manifestPath)) throw new FileNotFoundException("The manifest file was not found.");
        var manifest = new Manifest(manifestPath);

        Console.WriteLine("- Reading manifest from: {0}", manifestPath);

        if (opts.Build)
            buildManifest(ref manifest);
        else if (opts.Rebuild)
            rebuildManifest(ref manifest);
        else if (opts.Clean) cleanManifest(ref manifest);
    }

    private static void HandleParseError(IEnumerable<Error> errs) {
    }

    public class Options {
        [Option('m', "manifest", Required = false, HelpText = "Path to the manifest.")]
        public string? Manifest { get; set; }

        [Option('b', "build", Required = false, HelpText = "Build the manifest.", Group = "BuildAction")]
        public bool Build { get; set; }

        [Option('r', "rebuild", Required = false, HelpText = "Rebuild the manifest.", Group = "BuildAction")]
        public bool Rebuild { get; set; }

        [Option('c', "clean", Required = false, HelpText = "Clean the manifest.", Group = "BuildAction")]
        public bool Clean { get; set; }
    }
}