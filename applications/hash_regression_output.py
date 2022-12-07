#!/opt/homebrew/bin/python3
import csv
import sys
import hashlib
import os
from pathlib import Path

input_number = sys.argv[1]
regressions_file =  Path('regressions_{}.csv'.format(input_number))
results_file =  Path('regressions_{}.results.csv'.format(input_number))

def hashfile(file):
    md5 = hashlib.md5()

    with open(file, 'rb') as f:
        while True:
            data = f.read(BUF_SIZE)
            if not data:
                break
            md5.update(data)
    return md5

# BUF_SIZE is totally arbitrary, change for your app!
BUF_SIZE = 65536  # lets read stuff in 64kb chunks!

num_failures = 0
if regressions_file.is_file():
    print("Checking for expected output...")
    with open(results_file, 'w', newline='') as results:
        csvwriter = csv.writer(results)
        csvwriter.writerow(["FILE", "HASH"])
        with open(regressions_file, 'r', newline='') as csvfile:
            csvreader = csv.reader(csvfile)
            next(csvreader)
            for row in csvreader:
                [file, hash] = row
                md5 = hashfile(file)
                csvwriter.writerow([file, md5.hexdigest()])
                if md5.hexdigest() == hash:
                    print('{}: PASS'.format(file))
                else:
                    print('{}: FAIL'.format(file))
                    num_failures += 1
            

else:
    num_failures=1
    with open(regressions_file, 'w', newline='') as csvfile:
        csvwriter = csv.writer(csvfile)
        csvwriter.writerow(["FILE", "HASH"])
        for root, dirs, files in os.walk(os.getcwd()): 
            for file in files:
                if file.startswith("fort."):
                    md5 = hashfile(file)
                    csvwriter.writerow([file, md5.hexdigest()])

sys.exit(num_failures)