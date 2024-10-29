package xen.xnpak;

import org.jetbrains.annotations.Contract;
import org.jetbrains.annotations.NotNull;

import javax.imageio.ImageIO;
import javax.sound.sampled.*;
import java.awt.image.BufferedImage;
import java.awt.image.DataBufferByte;
import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;

public class AssetProcessors {
    @NotNull
    @Contract(value = "_ -> new", pure = true)
    public static byte[] processTexture(String filename) {
        try {
            var image = ImageIO.read(new File(filename));
            var formattedImage = new BufferedImage(image.getWidth(), image.getHeight(), BufferedImage.TYPE_4BYTE_ABGR);
            formattedImage.getGraphics().drawImage(image, 0, 0, null);
            var dataBuffer = (DataBufferByte) formattedImage.getRaster().getDataBuffer();
            byte[] abgrData = dataBuffer.getData();
            byte[] rgbaData = new byte[abgrData.length];
            for (int i = 0; i < abgrData.length; i += 4) {
                rgbaData[i] = abgrData[i + 3]; // R
                rgbaData[i + 1] = abgrData[i + 2]; // G
                rgbaData[i + 2] = abgrData[i + 1]; // B
                rgbaData[i + 3] = abgrData[i];     // A
            }

            return rgbaData;
        } catch (IOException e) {
            System.err.println("Failed to load image " + filename);
            return new byte[0];
        }
    }

    @NotNull
    @Contract(value = "_ -> new", pure = true)
    public static byte[] processAudio(String filename) {
        var wavFile = new File(filename);
        try {
            AudioInputStream inputStream = AudioSystem.getAudioInputStream(wavFile);
            AudioFormat format = inputStream.getFormat();

            // Get audio properties
            int sampleSizeInBytes = format.getSampleSizeInBits() / 8;
            int numChannels = format.getChannels();
            boolean isBigEndian = format.isBigEndian();

            // Buffer to store a single frame of audio data
            byte[] audioBuffer = new byte[sampleSizeInBytes * numChannels];
            var readBytes = inputStream.read(audioBuffer, 0, audioBuffer.length);

            return audioBuffer;
        } catch (UnsupportedAudioFileException | IOException e) {
            System.err.println("Failed to load wav file: " + filename);
            return new byte[0];
        }
    }

    private static int getSample(int sampleSizeInBytes, boolean isBigEndian, byte[] audioBuffer) {
        int sample = 0;

        if (sampleSizeInBytes == 2) { // for 16-bit sample size
            if (isBigEndian) {
                sample = ((audioBuffer[0] << 8) | (audioBuffer[1] & 0xFF));
            } else {
                sample = ((audioBuffer[1] << 8) | (audioBuffer[0] & 0xFF));
            }
        } else if (sampleSizeInBytes == 1) { // for 8-bit sample size
            sample = audioBuffer[0]; // often unsigned, so adjust as needed
        }
        return sample;
    }

    @NotNull
    @Contract(value = "_ -> new", pure = true)
    public static byte[] processData(String filename) {
        var path = Path.of(filename);
        try {
            return Files.readAllBytes(path);
        } catch (IOException e) {
            System.err.println("Failed to load file " + filename);
            return new byte[0];
        }
    }
}
