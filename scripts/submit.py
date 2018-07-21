from argparse import ArgumentParser
from pathlib import Path
import hashlib
from typing import Dict
from zipfile import ZipFile

import boto3
import api


S3_BUCKET = "manarimo-icfpc2018"


def generate_zip(destination: Path, blobs: Dict[str, bytes]):
    with ZipFile(str(destination), "w") as zf:
        for name, blob in blobs.items():
            zf.writestr("{}.nbt".format(name), blob)


def upload_to_s3(filepath: Path):
    hasher = hashlib.sha256()
    with filepath.open("rb") as f:
        hasher.update(f.read())
    digest = hasher.hexdigest()
    print("SHA256 Digest: {}".format(digest))

    path = "submissions/{}.zip".format(digest)
    destination = "s3://{}/{}".format(S3_BUCKET, path)
    print("Uploading a file to {}".format(destination))
    s3 = boto3.resource('s3')
    with filepath.open("rb") as f:
        s3.Bucket(S3_BUCKET).put_object(Key=path, Body=f)
    public_url = "http://{}.s3-website.ap-northeast-1.amazonaws.com/{}".format(S3_BUCKET, path)
    return digest, public_url


def do_submit(filepath: Path):
    digest, public_url = upload_to_s3(filepath)
    print("The uploaded file is avialable at: {}".format(public_url))
    response = api.do_submit(public_url, digest)
    print(response)


def main():
    parser = ArgumentParser()
    parser.add_argument("zipfile", type=Path)
    args = parser.parse_args()
    do_submit(args.zipfile)


if __name__ == '__main__':
    main()