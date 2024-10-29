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
import java.nio.file.*;
import java.nio.file.attribute.BasicFileAttributes;
import java.util.ArrayList;
import java.util.Objects;

public class Manifest {
    public String name;
    public String rootDir;
    public String outputDir;
    public boolean compress;
    public ArrayList<Asset> assets;

    public Manifest(@NotBlank String filename) {
        assets = new ArrayList<>();

        // Set the root directory
        var filePath = Paths.get(filename).toAbsolutePath();
        rootDir = filePath.getParent().toString();
        name = filePath.getFileName().toString();

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
        System.out.println("(Info) Cleaning manifest.");
        var _ = createContentDirectory();
        System.out.println("(Info) Manifest cleaned.");
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
            if (Objects.requireNonNull(asset.type) == AssetType.Texture) {
                var bytes = AssetProcessors.processTexture(srcPath.toString());
                data.ensureCapacity(bytes.length);
                data.clear();
                for (byte b : bytes) {
                    data.add(b);
                }
                data.trimToSize();
            } else {
                var bytes = AssetProcessors.processData(srcPath.toString());
                data.ensureCapacity(bytes.length);
                data.clear();
                for (byte b : bytes) {
                    data.add(b);
                }
                data.trimToSize();
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

            assetId++;
        }
    }

    @Nullable
    private String createContentDirectory() {
        var contentDir = Paths.get(rootDir, outputDir).toString();
        var cd = new File(contentDir);

        try {
            cleanDirectories(cd.toPath());
        } catch (IOException e) {
            System.err.println("(Error) Failed to create directory: " + cd);
            System.err.println("  | " + e.getMessage());
            return null;
        }

        if (!cd.mkdirs()) {
            System.err.println("(Error) Failed to create " + cd);
            return null;
        }

        return contentDir;
    }

    private void cleanDirectories(Path root) throws IOException {
        // I hate this so much :(
        // Just give me rm rf
        Files.walkFileTree(root, new SimpleFileVisitor<>() {
            @Override
            public FileVisitResult visitFile(Path file, BasicFileAttributes attrs) throws IOException {
                Files.delete(file);
                return FileVisitResult.CONTINUE;
            }

            @Override
            public FileVisitResult postVisitDirectory(Path dir, IOException exc) throws IOException {
                Files.delete(dir);
                return FileVisitResult.CONTINUE;
            }
        });
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