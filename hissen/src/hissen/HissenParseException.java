package hissen;

/**
 * @author masata
 */
public class HissenParseException extends Exception {
    public HissenParseException(final String message) {
        super(message);
    }

    public static HissenParseException byEmptyLine(final int lineNumber, final String line) {
        return new HissenParseException(String.format("Line %d: 空行があります。 raw=%s", lineNumber, line));
    }

    public static HissenParseException byParamNum(final int lineNumber, final String line) {
        return new HissenParseException(String.format("Line %d: パラメータの数が違います。 raw=%s", lineNumber, line));
    }

    public static HissenParseException byNumberFormat(final int lineNumber, final String line, final NumberFormatException e) {
        return new HissenParseException(String.format("Line %d: 数値のパースに失敗しました。 メッセージ=%s / raw=%s", lineNumber, e.getMessage(), line));
    }

    public static HissenParseException byConstraint(final int lineNumber, final String line, final String constraint) {
        return new HissenParseException(String.format("Line %d: 数値の範囲がおかしいです。 制約=%s / raw=%s", lineNumber, constraint, line));
    }

    public static HissenParseException byUnknownCommand(final int lineNumber, final String line, final String command) {
        return new HissenParseException(String.format("Line %d: %s は未知のコマンドです。 raw=%s", lineNumber, command, line));
    }
}
