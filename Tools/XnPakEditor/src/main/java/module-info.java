module org.jakerieger.xnpakeditor {
    requires javafx.controls;
    requires javafx.fxml;


    opens org.jakerieger.xnpakeditor to javafx.fxml;
    exports org.jakerieger.xnpakeditor;
}