#!/usr/bin/env python3

import subprocess
import os
import sys

good_dir = "./known_good_test_files"
bad_dir = "./known_bad_test_files"

good_tests = os.listdir(good_dir)
bad_tests = os.listdir(bad_dir)

for file_name in good_tests:
  result = subprocess.run(["./compiler", good_dir + "/" + file_name], capture_output=True)
  if (result.returncode != 0):
    print("Error: Expected " + file_name + " to pass tests but it failed\n")

for file_name in bad_tests:
  result = subprocess.run(["./compiler", bad_dir + "/" + file_name])
  if (result.returncode == 0):
    print("Error: Expected " + file_name + " to fail tests but it passed\n")