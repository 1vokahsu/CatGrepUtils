import os
import re
import random
import subprocess
from filecmp import cmp
from itertools import chain, combinations

class bcolors:
    OKGREEN = '\033[92m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'

def limited_combinations(lst, r=2):
    return list(chain.from_iterable(combinations(lst, i) for i in range(1, r + 1)))

your_grep = "./s21_grep"
original_grep = "grep"
test_files_dir = "tests/"
pattern_files_dir = "patterns/"
remove_files = "rm -rf *.txt"
compile_gcc = "make"
test_files = [f"{i}.txt" for i in range(1, 6)]
pattern_files = [f"{i}.txt" for i in range(1, 2)]

flags = ['e', 'i', 'v', 'c', 'l', 'n', 'h', 's', 'f', 'o']
manual_tests = [
    '-e ing -e as -e the -e not -e is tests/6', 
    '-e ing -e as -e the -e not -e is tests/*',
    '-ce \'\' tests/5.txt', 
    '-ce . tests/2.txt', 
    '-c \'[1-9]\' tests/*.txt nofile.txt'
]
all_words = set()
word_pattern = re.compile(r'\w+')

for test_file in test_files:
    with open(os.path.join(test_files_dir, test_file), 'r') as f:
        content = f.read()
        words = word_pattern.findall(content)
        all_words.update(words)
        
num_words_to_select = min(2, len(all_words))
patterns = random.sample(list(all_words), num_words_to_select)

flag_combinations = limited_combinations(flags)
for comb in manual_tests:
    for grep_cmd, output_file in [(your_grep, "s21_output.txt"), (original_grep, "original_output.txt")]:
        cmd = f"{grep_cmd} {comb} > {output_file}"
        subprocess.run(cmd, shell=True)
        print(bcolors.BOLD + f"TEST: {cmd}" + bcolors.ENDC)
        # if (cmd[0:len(your_grep)] == your_grep):
        #     subprocess.run("valgrind --tool=memcheck --leak-check=yes " + cmd, shell=True)
    
    if cmp("s21_output.txt", "original_output.txt"):
        print(bcolors.OKGREEN + f"TEST PASSED: {cmd}" + bcolors.ENDC)
    else:
        print(bcolors.FAIL + f"TEST FAILED: {cmd}" + bcolors.ENDC)
        exit(1)

for flags_combination in flag_combinations:
    if (('e' in flags_combination or 'f' in flags_combination) and len(flags_combination) != 1) or (('v' in flags_combination) and ('o' in flags_combination)) or (('o' in flags_combination) and ('c' in flags_combination) or (('l' in flags_combination) and len(flags_combination) > 1)):
        continue
    for test_file in test_files:
        test_file_path = os.path.join(test_files_dir, test_file)
        for pattern in patterns:
            for grep_cmd, output_file in [(your_grep, "s21_output.txt"), (original_grep, "original_output.txt")]:
                if 'f' in flags_combination:
                    pattern_file_path = os.path.join(pattern_files_dir, f"{patterns.index(pattern)+1}.txt")
                    cmd = f"{grep_cmd} -{''.join(flags_combination)} {pattern_file_path} {test_file_path} > {output_file}"
                    print(bcolors.BOLD + f"TEST: {cmd}" + bcolors.ENDC)
                else:
                    cmd = f"{grep_cmd} -{''.join(flags_combination)} {pattern} {test_file_path} > {output_file}"
                    print(bcolors.BOLD + f"TEST: {cmd}" + bcolors.ENDC)
                
                subprocess.run(cmd, shell=True)
                # if (cmd[0:len(your_grep)] == your_grep):
                #     subprocess.run("valgrind --tool=memcheck --leak-check=yes " + cmd, shell=True)

            if cmp("s21_output.txt", "original_output.txt"):
                print(bcolors.OKGREEN + f"TEST PASSED: {test_file} with flags -{''.join(flags_combination)} and pattern {pattern}" + bcolors.ENDC)
            else:
                print(bcolors.FAIL + f"TEST FAILED: {test_file} with flags -{''.join(flags_combination)} and pattern {pattern}" + bcolors.ENDC)
                exit(1)

subprocess.run(remove_files, shell=True)