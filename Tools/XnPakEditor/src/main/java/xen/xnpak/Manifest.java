package xen.xnpak;

import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;

public class Manifest {
    public String rootDir;
    public String outputDir;
    public boolean compress;
    public ArrayList<Asset> assets;

    public Manifest(@NotBlank String filename) {
        assets = new ArrayList<>();

        // Set the root directory
        var filePath = Paths.get(filename).toAbsolutePath();
        rootDir = filePath.getParent().toString();

        // Parse the manifest file
        File file = new File(filePath.toString());
        DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
        try {
            DocumentBuilder builder = factory.newDocumentBuilder();
            Document doc = builder.parse(file);

            Element root = doc.getDocumentElement();
            outputDir = root.getElementsByTagName("OutputDir").item(0).getTextContent();
            compress = "true".equals(root.getElementsByTagName("Compress").item(0).getTextContent());

            Element contentRoot = (Element) root.getElementsByTagName("Content").item(0);
            NodeList assetNodes = contentRoot.getElementsByTagName("Asset");
            for (int i = 0; i < assetNodes.getLength(); i++) {
                Element asset = (Element) assetNodes.item(i);
                String name = asset.getAttribute("name");
                String type = asset.getElementsByTagName("Type").item(0).getTextContent();
                String build = asset.getElementsByTagName("Build").item(0).getTextContent();
                assets.add(new Asset(name, type, build));
            }
        } catch (Exception e) {
            System.err.println("(Error) Failed to load manifest.\nReason: " + e.getMessage());
        }
    }

    public void build() {
        System.out.println("(Info) Building manifest.");
        var contentDir = createContentDirectory();

        System.out.println("  | Created output directory: " + contentDir);
        System.out.println("  | Building " + assets.size() + " assets.");
        assert contentDir != null;
        buildAssets(contentDir);
    }

    public void clean() {
    }

    @Override
    public String toString() {
        return "OutputDir: " +
                outputDir +
                "\n" +
                "Compress: " +
                compress +
                "\n" +
                "Assets: " +
                assets.size() +
                "\n";
    }

    private void buildAssets(@NotBlank @NotNull String contentDir) {
        var assetCount = assets.size();
        var assetId = 1;
        for (var asset : assets) {
            System.out.println("  | [" + assetId + "/" + assetCount
                    + "] Building asset: " + asset.name);

            var split = splitPath(asset.name);
            // Name contains subdirectories, create them
            if (split.size() > 1) {
                StringBuilder currentPath = new StringBuilder(contentDir);
                for (int i = 0; i < split.size() - 1; i++) {
                    currentPath.append("/").append(split.get(i));
                    var currentPathFile = new File(currentPath.toString());
                    if (!currentPathFile.exists()) {
                        if (!currentPathFile.mkdirs()) {
                            System.err.println("(Error) Failed to create directory: " + currentPath.toString());
                            return;
                        }
                    }
                }
            }

            // Import asset data
            ArrayList<Byte> data = new ArrayList<>();
            var srcPath = Paths.get(rootDir, asset.build);
            switch (asset.type) {
                case Texture -> {
                    var bytes = AssetProcessors.processTexture(srcPath.toString());
                    data.ensureCapacity(bytes.length);
                    data.clear();
                    for (byte b : bytes) {
                        data.add(b);
                    }
                    data.trimToSize();
                }
                case Audio -> {
                    var bytes = AssetProcessors.processAudio(srcPath.toString());
                    data.ensureCapacity(bytes.length);
                    data.clear();
                    for (byte b : bytes) {
                        data.add(b);
                    }
                    data.trimToSize();
                }
                default -> {
                    var bytes = AssetProcessors.processData(srcPath.toString());
                    data.ensureCapacity(bytes.length);
                    data.clear();
                    for (byte b : bytes) {
                        data.add(b);
                    }
                    data.trimToSize();
                }
            }

            // Write asset to pak file
            var outputFile = Paths.get(contentDir, asset.name) + ".xpak";
            try (BufferedOutputStream bos = new BufferedOutputStream(new FileOutputStream(outputFile))) {
                for (Byte b : data) {
                    bos.write(b);
                }
            } catch (IOException e) {
                System.err.println("(Error) Failed to create output file: " + outputFile);
                return;
            }
        }
    }

    @Nullable
    private String createContentDirectory() {
        var contentDir = Paths.get(rootDir, outputDir).toString();
        var cd = new File(contentDir);
        try {
            // TODO: This fails because the content directory isn't empty.
            // I need to walk the directory and delete each entry individually because Java sucks
            // and doesn't have a recursive delete.
            Files.deleteIfExists(cd.toPath());
        } catch (IOException e) {
            System.err.println("(Error) Failed to delete directory: " + cd);
            return null;
        }

        if (!cd.mkdirs()) {
            System.err.println("(Error) Failed to create " + cd);
            return null;
        }

        return contentDir;
    }

    // Can you tell I normally write C?
    @NotNull
    static ArrayList<String> splitPath(@NotBlank String path) {
        ArrayList<String> result = new ArrayList<>();
        StringBuilder current = new StringBuilder();

        for (int i = 0; i < path.length(); i++) {
            var ch = path.charAt(i);
            if (ch == '/' || ch == '\\') {
                if (!current.toString().isBlank()) {
                    result.add(current.toString());
                    current = new StringBuilder();
                }
            } else {
                current.append(ch);
            }
        }

        if (!current.toString().isBlank()) {
            result.add(current.toString());
        }

        return result;
    }
}