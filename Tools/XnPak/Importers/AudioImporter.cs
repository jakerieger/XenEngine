using NAudio.Wave;

namespace XnPak.Importers;

public static class AudioImporter {
    public static byte[] Import(string filename) {
        using (var reader = new WaveFileReader(filename)) {
            if (reader.WaveFormat.BitsPerSample != 32 ||
                reader.WaveFormat.Encoding != WaveFormatEncoding.IeeeFloat)
                throw new FormatException("Audio importer only supports 32-bit float encoded data");

            var rawAudioData = new byte[reader.Length];
            var bytesRead = reader.Read(rawAudioData, 0, rawAudioData.Length);
            return rawAudioData;
        }
    }
}