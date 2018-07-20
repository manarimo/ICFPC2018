package data;

import hissen.HissenParseException;

/**
 * @author masata
 */
public class ShortLinearDistance {
    public final int a;
    public final int i;

    public ShortLinearDistance(final int a, final int i) {
        this.a = a;
        this.i = i;
    }

    public static ShortLinearDistance parse(final String sdx, final String sdy, final String sdz, final int lineNumber, final String line) throws HissenParseException {
        try {
            final int dx = Integer.parseInt(sdx);
            final int dy = Integer.parseInt(sdy);
            final int dz = Integer.parseInt(sdz);
            final int a;
            final int i;
            if (dy == 0 && dz == 0) {
                a = 1;
                i = dx + 5;
            } else if (dx == 0 && dz == 0) {
                a = 2;
                i = dy + 5;
            } else if (dx == 0 && dy == 0) {
                a = 3;
                i = dz + 5;
            } else {
                throw HissenParseException.byConstraint(lineNumber, line, "SLDは0以外を1つしか含まない。");
            }
            if (i < 0 || i > 10) {
                throw HissenParseException.byConstraint(lineNumber, line, "SLDは最大距離5まで。");
            }
            return new ShortLinearDistance(a, i);
        } catch (final NumberFormatException e) {
            throw HissenParseException.byNumberFormat(lineNumber, line, e);
        }
    }
}
