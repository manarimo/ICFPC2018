package hissen;

import command.Command;
import data.FarDistance;
import data.LongLinearDistance;
import data.NearDistance;
import data.ShortLinearDistance;

import java.io.*;
import java.util.ArrayList;
import java.util.List;

/**
 * @author masata
 */
public class Hissen {
    public static List<Command> parse(final InputStream inputStream) throws IOException {
        final BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(inputStream));
        final List<Command> commands = new ArrayList<>();
        String line;
        int lineNumber = 0;
        while ((line = bufferedReader.readLine()) != null) {
            lineNumber++;
            try {
                commands.add(parseCommand(lineNumber, line));
            } catch (final HissenParseException e) {
                System.err.println(e.getMessage());
            }
        }
        return commands;
    }

    public static void write(final List<Command> commands, final OutputStream outputStream) throws IOException {
        for (final Command command : commands) {
            outputStream.write(command.encode());
        }
    }

    private static Command parseCommand(final int lineNumber, final String line) throws HissenParseException {
        final String[] array = line.split(" ");
        if (array.length == 0) {
            throw HissenParseException.byEmptyLine(lineNumber, line);
        }
        final String commandName = array[0];

        switch (commandName) {
            case "halt":
                if (array.length != 1) {
                    throw HissenParseException.byParamNum(lineNumber, line);
                }
                return new Command.Halt();

            case "wait":
                if (array.length != 1) {
                    throw HissenParseException.byParamNum(lineNumber, line);
                }
                return new Command.Wait();

            case "flip":
                if (array.length != 1) {
                    throw HissenParseException.byParamNum(lineNumber, line);
                }
                return new Command.Flip();

            case "smove":
                if (array.length != 4) {
                    throw HissenParseException.byParamNum(lineNumber, line);
                }
                final LongLinearDistance lld = LongLinearDistance.parse(array[1], array[2], array[3], lineNumber, line);
                return new Command.SMove(lld);

            case "lmove":
                if (array.length != 7) {
                    throw HissenParseException.byParamNum(lineNumber, line);
                }
                final ShortLinearDistance sld1 = ShortLinearDistance.parse(array[1], array[2], array[3], lineNumber, line);
                final ShortLinearDistance sld2 = ShortLinearDistance.parse(array[4], array[5], array[6], lineNumber, line);
                return new Command.LMove(sld1, sld2);

            case "fusionp":
                if (array.length != 4) {
                    throw HissenParseException.byParamNum(lineNumber, line);
                }
                final NearDistance nd = NearDistance.parse(array[1], array[2], array[3], lineNumber, line);
                return new Command.FusionP(nd);

            case "fusions":
                if (array.length != 4) {
                    throw HissenParseException.byParamNum(lineNumber, line);
                }
                final NearDistance nd2 = NearDistance.parse(array[1], array[2], array[3], lineNumber, line);
                return new Command.FusionS(nd2);

            case "fission":
                if (array.length != 5) {
                    throw HissenParseException.byParamNum(lineNumber, line);
                }
                final NearDistance nd3 = NearDistance.parse(array[1], array[2], array[3], lineNumber, line);
                try {
                    int m = Integer.parseInt(array[4]);
                    if (m < 0 || m > 255) {
                        throw HissenParseException.byConstraint(lineNumber, line, "分裂時の数値は255まで");
                    }
                    return new Command.Fission(nd3, m);
                } catch (final NumberFormatException e) {
                    throw HissenParseException.byNumberFormat(lineNumber, line, e);
                }

            case "fill":
                if (array.length != 4) {
                    throw HissenParseException.byParamNum(lineNumber, line);
                }
                final NearDistance nd4 = NearDistance.parse(array[1], array[2], array[3], lineNumber, line);
                return new Command.Fill(nd4);

            case "void":
                if (array.length != 4) {
                    throw HissenParseException.byParamNum(lineNumber, line);
                }
                final NearDistance nd5 = NearDistance.parse(array[1], array[2], array[3], lineNumber, line);
                return new Command.Void(nd5);

            case "gfill":
                if (array.length != 7) {
                    throw HissenParseException.byParamNum(lineNumber, line);
                }
                final NearDistance nd6 = NearDistance.parse(array[1], array[2], array[3], lineNumber, line);
                final FarDistance fd6 = FarDistance.parse(array[1], array[2], array[3], lineNumber, line);
                return new Command.GFill(nd6, fd6);

            case "gvoid":
                if (array.length != 7) {
                    throw HissenParseException.byParamNum(lineNumber, line);
                }
                final NearDistance nd7 = NearDistance.parse(array[1], array[2], array[3], lineNumber, line);
                final FarDistance fd7 = FarDistance.parse(array[1], array[2], array[3], lineNumber, line);
                return new Command.GVoid(nd7, fd7);
        }
        throw HissenParseException.byUnknownCommand(lineNumber, commandName, line);
    }
}
