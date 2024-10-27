package xen.xnpakeditor;

import org.jetbrains.annotations.NotNull;

import java.io.IOException;
import java.io.OutputStream;

import javafx.scene.control.TextArea;

public class TextAreaOutputStream extends OutputStream {
    private final TextArea textArea;

    public TextAreaOutputStream(TextArea textArea) {
        this.textArea = textArea;
    }

    @Override
    public void write(int b) {
        textArea.appendText(String.valueOf((char) b));
    }

    @Override
    public void write(@NotNull byte[] b, int off, int len) {
        // Append a portion of an array to the TextArea
        textArea.appendText(new String(b, off, len));
    }
}
