package command;

import data.FarDistance;
import data.LongLinearDistance;
import data.NearDistance;
import data.ShortLinearDistance;

/**
 * @author masata
 */
public interface Command {
    byte[] encode();

    class Halt implements Command {
        @Override
        public byte[] encode() {
            return new byte[] { (byte) 0xff };
        }
    }

    class Wait implements Command {
        @Override
        public byte[] encode() {
            return new byte[] { (byte) 0xfe };
        }
    }

    class Flip implements Command {
        @Override
        public byte[] encode() {
            return new byte[] { (byte) 0xfd };
        }
    }

    class SMove implements Command {
        private final LongLinearDistance lld;

        public SMove(final LongLinearDistance lld) {
            this.lld = lld;
        }

        @Override
        public byte[] encode() {
            final byte[] bytes = new byte[2];
            bytes[0] = (byte) (lld.a << 4 | 0x4);
            bytes[1] = (byte) lld.i;
            return bytes;
        }
    }

    class LMove implements Command {
        private final ShortLinearDistance sld1;
        private final ShortLinearDistance sld2;

        public LMove(final ShortLinearDistance sld1, final ShortLinearDistance sld2) {
            this.sld1 = sld1;
            this.sld2 = sld2;
        }

        @Override
        public byte[] encode() {
            final byte[] bytes = new byte[2];
            bytes[0] = (byte) (sld2.a << 6 | sld1.a << 4 | 0xc);
            bytes[1] = (byte) (sld2.i << 4 | sld1.i);
            return bytes;
        }
    }

    class FusionP implements Command {
        private final NearDistance nd;

        public FusionP(final NearDistance nd) {
            this.nd = nd;
        }

        @Override
        public byte[] encode() {
            final byte[] bytes = new byte[1];
            bytes[0] = (byte) (nd.enc << 3 | 0x7);
            return bytes;
        }
    }

    class FusionS implements Command {
        private final NearDistance nd;

        public FusionS(final NearDistance nd) {
            this.nd = nd;
        }

        @Override
        public byte[] encode() {
            final byte[] bytes = new byte[1];
            bytes[0] = (byte) (nd.enc << 3 | 0x6);
            return bytes;
        }
    }

    class Fission implements Command {
        private final NearDistance nd;
        private final int numSeeds;

        public Fission(final NearDistance nd, final int numSeeds) {
            this.nd = nd;
            this.numSeeds = numSeeds;
        }

        @Override
        public byte[] encode() {
            final byte[] bytes = new byte[2];
            bytes[0] = (byte) (nd.enc << 3 | 0x5);
            bytes[1] = (byte) (numSeeds);
            return bytes;
        }
    }

    class Fill implements Command {
        private final NearDistance nd;

        public Fill(final NearDistance nd) {
            this.nd = nd;
        }

        @Override
        public byte[] encode() {
            final byte[] bytes = new byte[1];
            bytes[0] = (byte) (nd.enc << 3 | 0x3);
            return bytes;
        }
    }

    class Void implements Command {
        private final NearDistance nd;

        public Void(final NearDistance nd) {
            this.nd = nd;
        }

        @Override
        public byte[] encode() {
            final byte[] bytes = new byte[1];
            bytes[0] = (byte) (nd.enc << 3 | 0x2);
            return bytes;
        }
    }

    class GFill implements Command {
        private final NearDistance nd;
        private final FarDistance fd;

        public GFill(final NearDistance nd, final FarDistance fd) {
            this.nd = nd;
            this.fd = fd;
        }

        @Override
        public byte[] encode() {
            final byte[] bytes = new byte[4];
            bytes[0] = (byte) (nd.enc << 3 | 0x1);
            bytes[1] = (byte) fd.dx;
            bytes[2] = (byte) fd.dy;
            bytes[3] = (byte) fd.dz;
            return bytes;
        }
    }

    class GVoid implements Command {
        private final NearDistance nd;
        private final FarDistance fd;

        public GVoid(final NearDistance nd, final FarDistance fd) {
            this.nd = nd;
            this.fd = fd;
        }

        @Override
        public byte[] encode() {
            final byte[] bytes = new byte[4];
            bytes[0] = (byte) (nd.enc << 3);
            bytes[1] = (byte) fd.dx;
            bytes[2] = (byte) fd.dy;
            bytes[3] = (byte) fd.dz;
            return bytes;
        }
    }
}
