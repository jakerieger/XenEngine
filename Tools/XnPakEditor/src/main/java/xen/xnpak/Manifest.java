package xen.xnpak;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import java.io.File;
import java.nio.file.Paths;
import java.util.ArrayList;

public class Manifest {
    public String rootDir;
    public String outputDir;
    public boolean compress;
    public ArrayList<Asset> assets;

    public Manifest(String filename) {
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

            if (!validateManifest(doc)) {
                // TODO: Give specific error messages for incorrect formatting errors
                System.err.println("Invalid manifest");
                return;
            }

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
            System.err.println("Failed to load manifest.\nReason: " + e.getMessage());
            return;
        }
    }

    public void build() {
    }

    public void clean() {
    }

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

    private boolean validateAsset() {
        return true;
    }

    private boolean validateManifest(Document doc) {
        return true;
    }

    private void buildAssets(String contentDir) {
    }

    private String createContentDirectory() {
        return "";
    }

    static ArrayList<String> splitPath(String path) {
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