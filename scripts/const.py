from pathlib import Path
import json

root = Path(__file__).parent.parent
conf_dir = root / "config"

with (conf_dir / "id.json").open() as f:
    _id = json.load(f)

public_id = _id["public"]
private_id = _id["private"]


if __name__ == '__main__':
    print(private_id)