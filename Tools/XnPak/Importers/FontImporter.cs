namespace XnPak.Importers;

public static class FontImporter {
    public static byte[] Import(string filename) {
        return File.ReadAllBytes(filename);
    }
}