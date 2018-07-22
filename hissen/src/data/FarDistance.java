package data;

import hissen.HissenParseException;

import java.util.stream.IntStream;

/**
 * @author masata
 */
public class FarDistance {
    public final int dx;
    public final int dy;
    public final int dz;

    public FarDistance(final int dx, final int dy, final int dz) {
        this.dx = dx;
        this.dy = dy;
        this.dz = dz;
    }

    public static FarDistance parse(final String sdx, final String sdy, final String sdz, final int lineNumber, final String line) throws HissenParseException {
        try {
            final int dx = Integer.parseInt(sdx);
            final int dy = Integer.parseInt(sdy);
            final int dz = Integer.parseInt(sdz);
            if (IntStream.of(dx, dy, dz).map(Math::abs).max().orElse(99) > 30) {
                throw HissenParseException.byConstraint(lineNumber, line, "FDはチェビシェフ距離30以下。");
            }
            return new FarDistance(dx + 30, dy + 30, dz + 30);
        } catch (final NumberFormatException e) {
            throw HissenParseException.byNumberFormat(lineNumber, line, e);
        }
    }
}
