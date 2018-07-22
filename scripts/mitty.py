#  mitty disassembler written in pure python. you do not need to use venv. easy!

from argparse import ArgumentParser
from pathlib import Path
import sys
import io
from typing import List, Type, Iterator
import enum


class EOFException(Exception): pass


def to_int(b: bytes):
    return int.from_bytes(b, "little")


def to_byte(x: int):
    return x.to_bytes(1, "little")


def read_or_die(f: io.BytesIO, length: int) -> bytes:
    b = f.read(length)
    if len(b) < length:
        raise EOFException()
    return b


class Token(object):
    def serialize(self) -> str:
        raise NotImplementedError()


class Operation(Token, enum.Enum):
    # special
    HALT = "halt"
    WAIT = "wait"
    FLIP = "flip"

    # 7 downto 0; smove, lmove = 4
    FUSIONP = "fusionp"
    FUSIONS = "fusions"
    FISSION = "fission"

    SMOVE = "smove"
    LMOVE = "lmove"

    FILL = "fill"
    VOID = "void"
    GFILL = "gfill"
    GVOID = "gvoid"

    def __init__(self, mnemonic):
        self.mnemonic = mnemonic

    def serialize(self):
        return self.mnemonic


def decode_nd(i: int) -> List[int]:
    z = i % 3 - 1
    i //= 3
    y = i % 3 - 1
    i //= 3
    x = i % 3 - 1
    return [x, y, z]


def decode_ld(a: int, i: int) -> List[int]:
    v = [0, 0, 0]
    v[a - 1] = i
    return v


class Command(object):
    def __init__(self, f: io.BytesIO):
        try:
            b = read_or_die(f, 1)
        except EOFException:
            raise StopIteration()

        code = to_int(b)
        # special codes
        if code == 0xff:
            self.op = Operation.HALT
            self.args = []
        elif code == 0xfe:
            self.op = Operation.WAIT
            self.args = []
        elif code == 0xfd:
            self.op = Operation.FLIP
            self.args = []
        else:
            subcode = to_int(b) & 7
            if subcode == 7:
                self.op = Operation.FUSIONP
                self.args = decode_nd(code >> 3)
            elif subcode == 6:
                self.op = Operation.FUSIONS
                self.args = decode_nd(code >> 3)
            elif subcode == 5:
                self.op = Operation.FISSION
                self.args = decode_nd(code >> 3) + [to_int(read_or_die(f, 1))]
            elif subcode == 4:
                if code & 15 == 4:
                    self.op = Operation.SMOVE
                    axis = code >> 4
                    dist = to_int(read_or_die(f, 1)) - 15
                    self.args = decode_ld(axis, dist)
                else:
                    self.op = Operation.LMOVE
                    axis1 = (code >> 4) & 3
                    axis2 = (code >> 6) & 3
                    dists = to_int(read_or_die(f, 1))
                    dist1 = (dists & 15) - 5
                    dist2 = (dists >> 4) - 5
                    self.args = decode_ld(axis1, dist1) + decode_ld(axis2, dist2)
            elif subcode == 3:
                self.op = Operation.FILL
                self.args = decode_nd(code >> 3)
            elif subcode == 2:
                self.op = Operation.VOID
                self.args = decode_nd(code >> 3)
            elif subcode == 1:
                self.op = Operation.GFILL
                self.args = decode_nd(code >> 3) + [to_int(read_or_die(f, 1)) - 30 for _ in range(3)]
            elif subcode == 0:
                self.op = Operation.GVOID
                self.args = decode_nd(code >> 3) + [to_int(read_or_die(f, 1)) - 30 for _ in range(3)]
            else:
                raise ValueError()

    def serialize(self) -> str:
        return ' '.join([self.op.serialize()] + [str(arg) for arg in self.args])


def disassemble(code: bytes) -> Iterator[Command]:
    stream = io.BytesIO(code)
    while True:
        yield Command(stream)


def main():
    parser = ArgumentParser()
    parser.add_argument("input", type=Path, nargs='?')

    args = parser.parse_args()
    if args.input:
        with args.input.open('rb') as f:
            code = f.read()
    else:
        code = sys.stdin.buffer.read()
    for command in disassemble(code):
        print(command.serialize())


if __name__ == '__main__':
    main()