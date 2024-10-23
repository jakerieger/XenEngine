using System;
using System.Linq;
using System.Xml.Linq;

namespace XnPak;

public struct Asset {
    public string Name { get; set; }
    public string Type { get; set; }
    public string Build { get; set; }

    public Asset(string name, string type, string build) {
        Name = name;
        Type = type;
        Build = build;
    }

    public override string ToString() {
        return $"Name: {Name}, Type: {Type}, Build: {Build}";
    }
}

public struct Manifest {
    public string OutputDirectory { get; set; }
    public bool Compress { get; set; }
    public Asset[] Assets { get; set; }

    private bool validateAsset(ref XElement element) {
        return true;
    }

    private bool validateManifest(ref XDocument manifest) {
        return true;
    }

    public Manifest(string filename) {
        var doc = XDocument.Load(filename);
        if (!validateManifest(ref doc)) Utilities.Panic("Invalid manifest");

        OutputDirectory = doc.Root.Element("OutputDir").Value;
        Compress = doc.Root.Element("Compress").Value.Equals("true", StringComparison.OrdinalIgnoreCase);
        Assets = doc.Root.Element("Content").Elements("Asset")
            .Select(asset => new Asset(
                asset.Attribute("name").Value,
                asset.Element("Type").Value,
                asset.Element("Build").Value))
            .ToArray();
    }
}