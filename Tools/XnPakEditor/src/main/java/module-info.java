module org.jakerieger.xnpakeditor {
    requires javafx.controls;
    requires javafx.fxml;
    requires java.xml;


    opens org.jakerieger.xnpakeditor to javafx.fxml;
    exports org.jakerieger.xnpakeditor;
}