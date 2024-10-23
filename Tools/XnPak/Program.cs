using System;
using System.IO;

namespace XnPak {
  internal abstract class Program {
    public static void Main(string[] args) {
      var manifest = new Manifest("../../Test/Content.manifest");
      Console.WriteLine(manifest.OutputDirectory);
      Console.WriteLine(manifest.Compress);
      Console.WriteLine(manifest.Assets.Length);
      foreach (var asset in manifest.Assets) {
        Console.WriteLine(asset);
      }
    }
  }
}