import os
import subprocess
from filecmp import cmp
from itertools import combinations

class bcolors:
    OKGREEN = '\033[92m'
    FAIL = '\033[91m'
    BOLD = '\033[1m'
    ENDC = '\033[0m'

s21_cat = "./s21_cat"
original_cat = "/bin/cat"
tests_files_dir = "tests/"
tests_files = [f"{str(i)}.txt" for i in range(1, 6)]
rem = "rm -rf *.txt"
run = "make"

short_flags = ['b', 'e', 'n', 's', 'v', 't']
long_flags = ['number-nonblank', 'number', 'squeeze-blank']
super_flags = ['E', 'T']
dic_flag = {'number-nonblank': '-b', 'number': '-n', 'squeeze-blank': '-s'}

flags_combs = long_flags[:]
for f in range(1, len(short_flags)+1):
    for comb in combinations(short_flags, f):
        flags_combs.append(list(comb))

for tests_file in tests_files:
    tests_file_path = os.path.join(tests_files_dir, tests_file)

    for flags_comb in flags_combs:
        for cat_cmd, output_file in [(s21_cat, "s21_output.txt"), (original_cat, "orig_output.txt")]:
            if (cat_cmd == original_cat and flags_comb in long_flags):
                flags_comb = dic_flag[flags_comb]
                flags = flags_comb
            else:
                flags = " ".join(f"-{flag}" for flag in flags_comb) if isinstance(flags_comb, list) else f"--{flags_comb}"
                    
            cmd = f"{cat_cmd} {flags} {tests_file_path} > {output_file}"
            print(bcolors.BOLD + f"TEST: {cmd}" + bcolors.ENDC)
            subprocess.run(cmd, shell=True)

        if cmp("s21_output.txt", "orig_output.txt"):
            print(bcolors.OKGREEN + f"TEST PASSED: {tests_file} with flag combination {flags}" + bcolors.ENDC)
        else:
            print(bcolors.FAIL + f"TEST FAILED: {tests_file} with flag combination {flags}" + bcolors.ENDC)
            exit(1)

cmd = "./s21_cat -T tests/1.txt > s21_output.txt"
subprocess.run(cmd, shell=True)
print(bcolors.BOLD + f"TEST: {cmd}" + bcolors.ENDC)
if cmp("s21_output.txt", "tests/tests_ET/T.txt"):
    print(bcolors.OKGREEN + f"TEST PASSED: 1.txt with flag combination -T" + bcolors.ENDC)
else:
    print(bcolors.FAIL + f"TEST FAILED: 1.txt with flag combination -T" + bcolors.ENDC)
    exit(1)

cmd = "./s21_cat -ET tests/2.txt > s21_output.txt"
subprocess.run(cmd, shell=True)
print(bcolors.BOLD + f"TEST: {cmd}" + bcolors.ENDC)
if cmp("s21_output.txt", "tests/tests_ET/ET.txt"):
    print(bcolors.OKGREEN + f"TEST PASSED: 2.txt with flag combination -ET" + bcolors.ENDC)
else:
    print(bcolors.FAIL + f"TEST FAILED: 2.txt with flag combination -ET" + bcolors.ENDC)
    exit(1)

cmd = "./s21_cat -E tests/3.txt > s21_output.txt"
subprocess.run(cmd, shell=True)
print(bcolors.BOLD + f"TEST: {cmd}" + bcolors.ENDC)
if cmp("s21_output.txt", "tests/tests_ET/E.txt"):
    print(bcolors.OKGREEN + f"TEST PASSED: 3.txt with flag combination -E" + bcolors.ENDC)
else:
    print(bcolors.FAIL + f"TEST FAILED: 3.txt with flag combination -E" + bcolors.ENDC)
    exit(1)

cmd = "./s21_cat -E -T tests/4.txt > s21_output.txt"
subprocess.run(cmd, shell=True)
print(bcolors.BOLD + f"TEST: {cmd}" + bcolors.ENDC)
if cmp("s21_output.txt", "tests/tests_ET/ET_1.txt"):
    print(bcolors.OKGREEN + f"TEST PASSED: 4.txt with flag combination -E -T" + bcolors.ENDC)
else:
    print(bcolors.FAIL + f"TEST FAILED: 4.txt with flag combination -E -T" + bcolors.ENDC)
    exit(1)

subprocess.run(rem, shell=True)