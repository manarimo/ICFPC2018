from argparse import ArgumentParser
from pathlib import Path
import hashlib
import boto3
import api


S3_BUCKET = "manarimo-icfpc2018"


def main():
    parser = ArgumentParser()
    parser.add_argument("zipfile", type=Path)
    args = parser.parse_args()

    hasher = hashlib.sha256()
    with args.zipfile.open("rb") as f:
        hasher.update(f.read())
    digest = hasher.hexdigest()
    print("SHA256 Digest: {}".format(digest))

    path = "submissions/{}.zip".format(digest)
    destination = "s3://{}/{}".format(S3_BUCKET, path)
    print("Uploading a file to {}".format(destination))
    s3 = boto3.resource('s3')
    with args.zipfile.open("rb") as f:
        s3.Bucket(S3_BUCKET).put_object(Key=path, Body=f)
    public_url = "http://{}.s3-website.ap-northeast-1.amazonaws.com/{}".format(S3_BUCKET, path)
    print("The uploaded file is avialable at: {}".format(public_url))

    response = api.do_submit(public_url, digest)
    print(response)


if __name__ == '__main__':
    main()