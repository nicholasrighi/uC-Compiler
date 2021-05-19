#!/usr/bin/env python3

import subprocess
import os
import sys
from typing import no_type_check_decorator

good_dir = "./known_good_test_files"
bad_dir = "./known_bad_test_files"

good_tests = os.listdir(good_dir)
bad_tests = os.listdir(bad_dir)

no_errors = True

for file_name in good_tests:
  result = subprocess.run(["./compiler", "-f" + good_dir + "/" + file_name], capture_output=True)
  if (result.returncode != 0):
    print("Error: Expected " + file_name + " to pass tests but it failed\n")
    no_errors = False

for file_name in bad_tests:
  result = subprocess.run(["./compiler", "-f" + bad_dir + "/" + file_name])
  if (result.returncode == 0):
    print("Error: Expected " + file_name + " to fail tests but it passed\n")
    no_errors = False

if (no_errors):
  print("Success, no unexpected errors!")
else:
  print("One or more compiler errors were discovered")