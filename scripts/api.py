from typing import Dict
import const
import requests


API_ENDPOINT = "https://script.google.com/macros/s/AKfycbzQ7Etsj7NXCN5thGthCvApancl5vni5SFsb1UoKgZQwTzXlrH7/exec"


def do_request(params: Dict[str, str]):
    return requests.get(API_ENDPOINT, params).json


def do_submit(submission_url: str, submission_sha: str):
    return do_request({
        "action": "submit",
        "privateID": const.private_id,
        "submissionURL": submission_url,
        "submissionSHA": submission_sha
    })


if __name__ == '__main__':
    print(const)
    pass