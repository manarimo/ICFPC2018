from pathlib import Path
import json

root = Path(__file__).parent.parent
conf_dir = root / "config"

with (conf_dir / "id.json").open() as f:
    _id = json.load(f)

public_id = _id["public"]
private_id = _id["private"]

env_file = conf_dir / "env.txt"
if env_file.exists():
    with env_file.open() as f:
        env = f.read().strip()
else:
    env = "dev"


if __name__ == '__main__':
    print(private_id)