package xen.xnpakeditor;

import javafx.beans.property.SimpleObjectProperty;
import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;
import xen.xnpak.Manifest;

public class EditorState {
    private static final EditorState instance = new EditorState();

    private final StringProperty selectedManifestPath = new SimpleStringProperty();
    private final SimpleObjectProperty<Manifest> currentManifest = new SimpleObjectProperty<>();

    private EditorState() {
    }

    public static EditorState getInstance() {
        return instance;
    }

    public StringProperty selectedManifestPathProperty() {
        return selectedManifestPath;
    }

    public SimpleObjectProperty<Manifest> currentManifestProperty() {
        return currentManifest;
    }
}
