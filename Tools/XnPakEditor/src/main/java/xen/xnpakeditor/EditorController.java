package xen.xnpakeditor;

import javafx.concurrent.Task;
import javafx.fxml.FXML;
import javafx.scene.control.*;
import javafx.scene.layout.AnchorPane;
import javafx.stage.FileChooser;
import javafx.stage.Stage;
import org.jetbrains.annotations.NotNull;
import xen.xnpak.Manifest;

import java.io.PrintStream;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.concurrent.atomic.AtomicBoolean;

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
    private final AtomicBoolean isBuilding = new AtomicBoolean(false);

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
        if (editorState.currentManifestProperty().get() != null) {
            editorState.currentManifestProperty().get().write();
        }
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

    // TODO: Current blocking the UI, make this asynchronous.
    @FXML
    protected void onBuild() {
        if (editorState.currentManifestProperty().get() != null && !isBuilding.get()) {
            isBuilding.set(true);

            Task<Void> buildTask = new Task<>() {
                @Override
                protected Void call() {
                    editorState.currentManifestProperty().get().build();
                    return null;
                }
            };

            buildTask.setOnSucceeded(event -> {
                isBuilding.set(false);
                var msg = new Alert(Alert.AlertType.INFORMATION);
                msg.setTitle("Build");
                msg.setHeaderText("Manifest built successfully.");
                msg.showAndWait();
            });

            buildTask.setOnFailed(event -> {
                isBuilding.set(false);
                Throwable exception = buildTask.getException();
                Alert alert = new Alert(Alert.AlertType.ERROR);
                alert.setTitle("Build Error");
                alert.setHeaderText("Error building manifest");
                alert.setContentText(exception != null ? exception.getMessage() : "An unknown error occurred.");
                alert.showAndWait();
            });

            // This runs the task on a background thread.
            new Thread(buildTask).start();
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

    @NotNull
    private ArrayList<MenuItem> contextMenu() {
        ArrayList<MenuItem> menuItems = new ArrayList<>();

        var importAssetItem = new MenuItem("Import Asset");
        importAssetItem.setOnAction(event -> {
            onImportAsset();
        });

        var newAssetItem = new MenuItem("New Asset");
        newAssetItem.setOnAction(event -> {
            onNewAsset();
        });

        var importFolderItem = new MenuItem("Import Folder");
        importFolderItem.setOnAction(event -> {
            onImportFolder();
        });

        var newFolderItem = new MenuItem("New Folder");
        newFolderItem.setOnAction(event -> {
            onNewFolder();
        });

        var buildItem = new MenuItem("Build");
        buildItem.setOnAction(event -> {
            onBuild();
        });

        var cleanItem = new MenuItem("Clean");
        cleanItem.setOnAction(event -> {
            onClean();
        });

        menuItems.add(importAssetItem);
        menuItems.add(newAssetItem);
        menuItems.add(importFolderItem);
        menuItems.add(newFolderItem);
        menuItems.add(buildItem);
        menuItems.add(cleanItem);

        return menuItems;
    }

    private void updateContentTree() {
        var manifest = editorState.currentManifestProperty().get();
        if (manifest != null) {
            var rootItem = new TreeItem<>(manifest.name.split("\\.")[0]);
            contentTree.setRoot(rootItem);

            var rootContextMenu = new ContextMenu();
            rootContextMenu.getItems().addAll(contextMenu());
            contentTree.setContextMenu(rootContextMenu);

            for (var asset : manifest.assets) {
                var treeItem = new TreeItem<>(asset.name);
                treeItem.setExpanded(true);
                rootItem.getChildren().add(treeItem);
            }
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