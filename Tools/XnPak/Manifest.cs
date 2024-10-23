using System.Xml.Linq;
using System;
using System.Linq;

namespace XnPak {
    public struct Asset {
        public string Name { get; set; }
        public string Type { get; set; }
        public string Build { get; set; }

        public Asset(string name, string type, string build) {
            this.Name = name;
            this.Type = type;
            this.Build = build;
        }

        public override string ToString() {
            return $"Name: {Name}, Type: {Type}, Build: {Build}";
        }
    }

    public struct Manifest {
        public string OutputDirectory { get; set; }
        public bool Compress { get; set; }
        public Asset[] Assets { get; set; }

        private XDocument _doc;

        public Manifest(string filename) {
            _doc = XDocument.Load(filename);

            OutputDirectory = _doc.Root.Element("OutputDir").Value;
            Compress = _doc.Root.Element("Compress").Value.Equals("true", StringComparison.OrdinalIgnoreCase);
            Assets = _doc.Root.Element("Content").Elements("Asset")
                .Select(asset => new Asset(
                    name: asset.Attribute("name").Value,
                    type: asset.Element("Type").Value,
                    build: asset.Element("Build").Value))
                .ToArray();
        }
    }
}