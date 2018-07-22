import const
import mitty
from pathlib import Path
from tempfile import NamedTemporaryFile
import subprocess


def assemble(asm_path: Path, dest_path: Path):
    hissen_bin = const.root / "hissen" / "hissen"
    subprocess.call([hissen_bin, asm_path, dest_path])


def main():
    nobita_dir = const.root / "assets" / "dfltTracesF"
    for problem_id in range(1, 187):
        for problem_type in ["A", "D", "R"]:
            nobita_path = nobita_dir / "F{}{:03}.nbt".format(problem_type, problem_id)
            print("Testing on {}".format(nobita_path))

            with nobita_path.open("rb") as f:
                commands = mitty.disassemble(f.read())
            with NamedTemporaryFile() as asm_file, NamedTemporaryFile() as dest_file:
                asm_path = Path(asm_file.name)
                dest_path = Path(dest_file.name)
                with asm_path.open("w") as f:
                    for command in commands:
                        print(command.serialize(), file=f)
                assemble(asm_path, dest_path)
                with nobita_path.open("rb") as expected_f, dest_path.open("rb") as actual_f:
                    if expected_f.read() != actual_f.read():
                        raise ValueError("failed roundtrip test")


if __name__ == '__main__':
    main()