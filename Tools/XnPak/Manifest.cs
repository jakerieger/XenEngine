using System.Xml.Linq;

namespace XnPak;

public enum AssetType {
    Texture,
    Audio,
    Font,
    PlainText
}

public struct Asset(string name, string type, string build) {
    public string Name { get; set; } = name;
    public AssetType Type { get; set; } = GetType(type);
    public string Build { get; set; } = build;

    public override string ToString() {
        return $"Name: {Name}, Type: {Type}, Build: {Build}";
    }

    private static AssetType GetType(string type) {
        return type switch {
            "Texture" => AssetType.Texture,
            "Audio" => AssetType.Audio,
            "Font" => AssetType.Font,
            "PlainText" => AssetType.PlainText,
            _ => AssetType.PlainText
        };
    }
}

public struct Manifest {
    public string OutputDirectory { get; set; }
    public bool Compress { get; set; }
    public Asset[] Assets { get; set; }
    public string RootDirectory { get; set; }

    private static bool ValidateAsset(XElement element) {
        if (element.Name != "Asset") return false;
        if (element.Attribute("name") is null) return false;
        if (element.Element("Type") is null) return false;
        if (element.Element("Build") is null) return false;

        return true;
    }

    private static bool ValidateManifest(ref XDocument manifest) {
        if (manifest.Root is null) return false;
        if (manifest.Root.Name != "PakManifest") return false;
        if (manifest.Root.Element("OutputDir") is null) return false;
        if (manifest.Root.Element("Compress") is null) return false;
        if (manifest.Root.Element("Content") is null) return false;
        if (manifest.Root.Element("Content")!.Elements("Asset").Count() is 0) return false;

        return manifest.Root.Element("Content")!.Elements("Asset").All(ValidateAsset);
    }

    public Manifest(string filename) {
        RootDirectory = Path.GetDirectoryName(filename)!;

        var doc = XDocument.Load(filename);
        if (!ValidateManifest(ref doc)) throw new FormatException("Invalid manifest file");

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