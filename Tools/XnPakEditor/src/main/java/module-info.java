module org.jakerieger.xnpakeditor {
    requires javafx.controls;
    requires javafx.fxml;
    requires java.xml;


    opens xen.xnpakeditor to javafx.fxml;
    exports xen.xnpakeditor;
}