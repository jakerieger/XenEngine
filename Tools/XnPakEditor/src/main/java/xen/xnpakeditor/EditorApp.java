package xen.xnpakeditor;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Scene;
import javafx.scene.image.Image;
import javafx.stage.Stage;

import java.io.IOException;
import java.util.Objects;

public class EditorApp extends Application {
    @Override
    public void start(Stage stage) throws IOException {
        FXMLLoader fxmlLoader = new FXMLLoader(EditorApp.class.getResource("editor-view.fxml"));
        Scene scene = new Scene(fxmlLoader.load(), 1200, 750);

        Application.setUserAgentStylesheet(Objects.requireNonNull(EditorApp.class.getResource("cupertino-dark.css")).toExternalForm());

        stage.setTitle("XnPak Editor");
        var windowIcon = new Image(Objects.requireNonNull(EditorApp.class.getResourceAsStream("icon.png")));
        stage.getIcons().add(windowIcon);
        stage.setScene(scene);
        stage.show();
    }

    public static void main(String[] args) {
        launch(args);
    }
}