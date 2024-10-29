package xen.xnpak;

import java.util.Scanner;

public class CLI {
    public static void main(String[] args) {
        if (args.length == 0) {
            System.out.println("XnPak - version 0.0.1-dev");
            System.out.println("In order to use XnPak, please provide the required arguments.");
            System.out.println("You can run 'xnpak -h' for more information.\n");
            System.out.println("Press any key to exit...");

            var scanner = new Scanner(System.in);
            var _ = scanner.nextLine();
            scanner.close();
        }
    }
}

