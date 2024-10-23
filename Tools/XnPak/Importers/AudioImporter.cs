namespace XnPak.Importers;

public static class AudioImporter {
    public static byte[] Import(string filename) {
        return File.ReadAllBytes(filename);
    }
}