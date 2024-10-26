package org.jakerieger.xnpakeditor;

import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;

public class EditorState {
    private static final EditorState instance = new EditorState();

    private final StringProperty selectedManifestPath = new SimpleStringProperty();

    private EditorState() {}

    public static EditorState getInstance() {
        return instance;
    }

    public StringProperty selectedManifestPathProperty() {
        return selectedManifestPath;
    }
}
