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
    ### DO NOT PRINT ANYTHING IN THIS FUNCTION!!!!!!!!!!!! RUN_WITH_SCORE.RB SERIOUSLY BREAKS!!!!!!!!!
    hasher = hashlib.sha256()
    with filepath.open("rb") as f:
        hasher.update(f.read())
    digest = hasher.hexdigest()

    path = "submissions/{}.zip".format(digest)
    destination = "s3://{}/{}".format(S3_BUCKET, path)
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
    parser.add_argument("submit_type", default="traces")
    args = parser.parse_args()

    if args.submit_type == 'traces':
        do_submit(args.zipfile)
    elif args.submit_type == 'sourcecode':
        digest, public_url = upload_to_s3(args.zipfile)
        print("Uploaded the zipfile to S3.")
        print("URL: {}".format(public_url))
        print("SHA256 digest: {}".format(digest))
        print("Don't forget to submit these info on the contest site!")


if __name__ == '__main__':
    main()