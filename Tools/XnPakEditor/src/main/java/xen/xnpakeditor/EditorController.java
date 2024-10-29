package xen.xnpakeditor;

import javafx.fxml.FXML;
import javafx.scene.control.*;
import javafx.scene.layout.AnchorPane;
import javafx.stage.FileChooser;
import javafx.stage.Stage;
import org.w3c.dom.Document;
import xen.xnpak.Manifest;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import java.io.File;
import java.io.PrintStream;
import java.nio.file.Paths;

public class EditorController {
    public MenuBar mainMenu;
    public ToolBar toolBar;
    public TreeView contentTree;
    public TableView propertyTable;
    public AnchorPane previewPane;
    public TextArea buildOutput;
    public Button openButton;
    public Button saveButton;
    public Button saveAsButton;
    public Button undoButton;
    public Button redoButton;
    public Button importAssetButton;
    public Button newAssetButton;
    public Button importFolder;
    public Button newFolder;
    public Button buildButton;
    public Button rebuildButton;
    public Button cleanButton;

    private final EditorState editorState = EditorState.getInstance();

    @FXML
    public void initialize() {
        // Redirect Stdout to our Build Output panel
        PrintStream printStream = new PrintStream(new TextAreaOutputStream(buildOutput));
        System.setOut(printStream);
        System.setErr(printStream);
        updateToolbarState();

        System.out.println("(Info) XnPak Editor has been initialized.");
    }

    @FXML
    protected void onOpen() {
        Stage stage = (Stage) openButton.getScene().getWindow();

        FileChooser fileChooser = new FileChooser();
        fileChooser.setTitle("Open Manifest");
        fileChooser.getExtensionFilters().addAll(
                new FileChooser.ExtensionFilter("XnPak Manifest", "* .manifest")
        );
        var file = fileChooser.showOpenDialog(stage);
        if (file != null) {
            loadManifest(file.getAbsolutePath());
        }
    }

    @FXML
    protected void onSave() {
    }

    @FXML
    protected void onSaveAs() {
    }

    @FXML
    protected void onUndo() {
    }

    @FXML
    protected void onRedo() {
    }

    @FXML
    protected void onImportAsset() {
    }

    @FXML
    protected void onNewAsset() {
    }

    @FXML
    protected void onImportFolder() {
    }

    @FXML
    protected void onNewFolder() {
    }

    @FXML
    protected void onBuild() {
        if (editorState.currentManifestProperty().get() != null) {
            editorState.currentManifestProperty().get().build();
            var msg = new Alert(Alert.AlertType.INFORMATION);
            msg.setTitle("Build");
            msg.setHeaderText("Manifest built successfully.");
            msg.showAndWait();
        }
    }

    @FXML
    protected void onRebuild() {
    }

    @FXML
    protected void onClean() {
        if (editorState.currentManifestProperty().get() != null) {
            editorState.currentManifestProperty().get().clean();
        }
    }

    private void updateToolbarState() {
        if (editorState.currentManifestProperty().get() != null) {
            saveButton.setDisable(false);
            saveAsButton.setDisable(false);
            undoButton.setDisable(false);
            redoButton.setDisable(false);
            importAssetButton.setDisable(false);
            newAssetButton.setDisable(false);
            importFolder.setDisable(false);
            newFolder.setDisable(false);
            buildButton.setDisable(false);
            rebuildButton.setDisable(false);
            cleanButton.setDisable(false);
        } else {
            saveButton.setDisable(true);
            saveAsButton.setDisable(true);
            undoButton.setDisable(true);
            redoButton.setDisable(true);
            importAssetButton.setDisable(true);
            newAssetButton.setDisable(true);
            importFolder.setDisable(true);
            newFolder.setDisable(true);
            buildButton.setDisable(true);
            rebuildButton.setDisable(true);
            cleanButton.setDisable(true);
        }
    }

    private void updateContentTree() {
        var manifest = editorState.currentManifestProperty().get();
        if (manifest != null) {
            var rootItem = new TreeItem<>(manifest.name.split("\\.")[0]);
            contentTree.setRoot(rootItem);

            var contextMenu = new ContextMenu();
            contextMenu.getItems().addAll(new MenuItem("Import Asset"), new MenuItem("New Asset"), new MenuItem("Import Folder"), new MenuItem("New Folder"), new MenuItem("Build"));
            contentTree.setContextMenu(contextMenu);
        }
    }

    private void loadManifest(String filename) {
        try {
            var manifest = new Manifest(filename);
            editorState.currentManifestProperty().setValue(manifest);
            if (manifest.outputDir != null) {
                buildOutput.setText("");
                System.out.println("(Info) Loaded manifest: " + Paths.get(filename).toAbsolutePath().toString());
            }
        } catch (Exception e) {
            System.err.println("Failed to load manifest");
            return;
        }

        updateContentTree();
        updateToolbarState();
    }
}