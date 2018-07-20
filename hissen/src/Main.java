import command.Command;
import hissen.Hissen;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.List;

/**
 * @author masata
 */
public class Main {
    public static void main(final String[] args) {
        if (args.length != 2) {
            System.err.println("USAGE: hissen <inputfile> <outputfile>");
        }
        try {
            run(args[0], args[1]);
        } catch (final IOException e) {
            e.printStackTrace();
        }
    }

    private static void run(final String inputFileName, final String outputFileName) throws IOException {
        List<Command> commands;
        try (final FileInputStream inputStream = new FileInputStream(inputFileName)) {
            commands = Hissen.parse(inputStream);
        }
        System.out.println(String.format("Successfully parse %d commands", commands.size()));
        try (final FileOutputStream fileOutputStream = new FileOutputStream(outputFileName)) {
            Hissen.write(commands, fileOutputStream);
        }
        System.out.println(String.format("Successfully write %d commands", commands.size()));
    }
}
