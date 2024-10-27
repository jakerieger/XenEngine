module org.jakerieger.xnpakeditor {
    requires javafx.controls;
    requires javafx.fxml;
    requires annotations;
    requires java.desktop;


    opens xen.xnpakeditor to javafx.fxml;
    exports xen.xnpakeditor;
}