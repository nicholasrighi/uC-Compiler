#!/usr/bin/env python3

import subprocess
import os
from typing import no_type_check_decorator

good_dir = "./known_good_test_files"
bad_dir = "./known_bad_test_files"

good_tests = os.listdir(good_dir)
bad_tests = os.listdir(bad_dir)

tests_that_should_not_have_failed = []
tests_that_should_have_failed = []

for file_name in good_tests:
  result = subprocess.run(["./compiler", "-f", good_dir + "/" + file_name], capture_output=True)
  if (result.returncode != 0):
    print("Error: Expected " + file_name + " to pass tests but it failed\n")
    tests_that_should_not_have_failed.append(file_name)

for file_name in bad_tests:
  result = subprocess.run(["./compiler", "-f", bad_dir + "/" + file_name])
  if (result.returncode == 0):
    print("Error: Expected " + file_name + " to fail tests but it passed\n")
    tests_that_should_have_failed.append(file_name)

if (len(tests_that_should_have_failed) != 0 or len(tests_that_should_not_have_failed) != 0):
  print("\nErrors were discovered in the following files")
  print("\nTests that should have passed but didn't")
  for file_name in tests_that_should_not_have_failed:
      print(file_name)
  print("\nTests that should have failed but passed")
  for file_name in tests_that_should_have_failed:
      print(file_name)
else:
    print("\nNo errors detected in parser")