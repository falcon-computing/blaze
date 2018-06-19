#!/usr/bin/env python
import argparse
from blaze_client import blaze_client

# argument parser
parser = argparse.ArgumentParser(description='A Testing Client for AccRegister')
parser.add_argument('--ip', dest='ip', type=str, default='127.0.0.1',
                    help='The host IP of the destination NAM')
parser.add_argument('--id', dest='id', help='id for acc ')

args = parser.parse_args()
ip = args.ip
acc_id = args.id

client = blaze_client(ip)

if acc_id:
  client.reserve_acc(acc_id)
