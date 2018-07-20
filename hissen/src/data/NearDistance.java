package data;

import hissen.HissenParseException;

import java.util.stream.IntStream;

/**
 * @author masata
 */
public class NearDistance {
    public final int enc;

    public NearDistance(final int enc) {
        this.enc = enc;
    }

    public static NearDistance parse(final String sdx, final String sdy, final String sdz, final int lineNumber, final String line) throws HissenParseException {
        try {
            final int dx = Integer.parseInt(sdx);
            final int dy = Integer.parseInt(sdy);
            final int dz = Integer.parseInt(sdz);
            if (Math.abs(dx) + Math.abs(dy) + Math.abs(dz) > 2) {
                throw HissenParseException.byConstraint(lineNumber, line, "NDはマンハッタン距離2以下。");
            }
            if (IntStream.of(Math.abs(dx), Math.abs(dy), Math.abs(dz)).max().orElse(0) != 1) {
                throw HissenParseException.byConstraint(lineNumber, line, "NDは座標差の最大は1。");
            }
            final int enc = (dx + 1) * 9 + (dy + 1) * 3 + (dz + 1);
            return new NearDistance(enc);
        } catch (final NumberFormatException e) {
            throw HissenParseException.byNumberFormat(lineNumber, line, e);
        }
    }
}
