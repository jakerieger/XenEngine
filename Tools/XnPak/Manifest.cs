using System;
using System.IO;
using System.Linq;
using System.Xml.Linq;

namespace XnPak;

public enum AssetType {
    Texture,
    Audio,
    Font,
    PlainText
}

public struct Asset {
    public string Name { get; set; }
    public AssetType Type { get; set; }
    public string Build { get; set; }

    public Asset(string name, string type, string build) {
        Name = name;
        Type = getType(type);
        Build = build;
    }

    public override string ToString() {
        return $"Name: {Name}, Type: {Type}, Build: {Build}";
    }

    private static AssetType getType(string type) {
        switch (type) {
            case "Texture":
                return AssetType.Texture;
            case "Audio":
                return AssetType.Audio;
            case "Font":
                return AssetType.Font;
            case "PlainText":
                return AssetType.PlainText;
        }

        return AssetType.PlainText;
    }
}

public struct Manifest {
    public string OutputDirectory { get; set; }
    public bool Compress { get; set; }
    public Asset[] Assets { get; set; }
    public string RootDirectory { get; set; }

    private bool validateAsset(XElement element) {
        if (element.Name != "Asset") return false;
        if (element.Attribute("name") is null) return false;
        if (element.Element("Type") is null) return false;
        if (element.Element("Build") is null) return false;

        return true;
    }

    private bool validateManifest(ref XDocument manifest) {
        if (manifest.Root is null) return false;
        if (manifest.Root.Name != "PakManifest") return false;
        if (manifest.Root.Element("OutputDir") is null) return false;
        if (manifest.Root.Element("Compress") is null) return false;
        if (manifest.Root.Element("Content") is null) return false;
        if (manifest.Root.Element("Content")!.Elements("Asset").Count() is 0) return false;

        foreach (var el in manifest.Root.Element("Content")!.Elements("Asset")) {
            if (!validateAsset(el))
                return false;
        }

        return true;
    }

    public Manifest(string filename) {
        RootDirectory = Path.GetDirectoryName(filename)!;

        var doc = XDocument.Load(filename);
        if (!validateManifest(ref doc)) throw new FormatException("Invalid manifest file");

        OutputDirectory = doc.Root!.Element("OutputDir")!.Value;
        Compress = doc.Root.Element("Compress")!.Value.Equals("true",
            StringComparison.OrdinalIgnoreCase);
        Assets = doc.Root.Element("Content")!.Elements("Asset")
            .Select(asset => new Asset(
                asset.Attribute("name")!.Value,
                asset.Element("Type")!.Value,
                asset.Element("Build")!.Value))
            .ToArray();
    }
}