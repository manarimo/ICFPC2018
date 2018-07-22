import submit
import sys
from pathlib import Path

if __name__ == '__main__':
    digest, url = submit.upload_to_s3(Path(sys.argv[1]))
    print(url)