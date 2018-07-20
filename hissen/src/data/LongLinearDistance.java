package data;

import hissen.HissenParseException;

/**
 * @author masata
 */
public class LongLinearDistance {
    public final int a;
    public final int i;

    public LongLinearDistance(final int a, final int i) {
        this.a = a;
        this.i = i;
    }

    public static LongLinearDistance parse(final String sdx, final String sdy, final String sdz, final int lineNumber, final String line) throws HissenParseException {
        try {
            final int dx = Integer.parseInt(sdx);
            final int dy = Integer.parseInt(sdy);
            final int dz = Integer.parseInt(sdz);
            final int a;
            final int i;
            if (dy == 0 && dz == 0) {
                a = 1;
                i = dx + 15;
            } else if (dx == 0 && dz == 0) {
                a = 2;
                i = dy + 15;
            } else if (dx == 0 && dy == 0) {
                a = 3;
                i = dz + 15;
            } else {
                throw HissenParseException.byConstraint(lineNumber, line, "LLDは0以外を1つしか含まない。");
            }
            if (i < 0 || i > 30) {
                throw HissenParseException.byConstraint(lineNumber, line, "LLDは最大距離15まで。");
            }
            return new LongLinearDistance(a, i);
        } catch (final NumberFormatException e) {
            throw HissenParseException.byNumberFormat(lineNumber, line, e);
        }
    }
}
