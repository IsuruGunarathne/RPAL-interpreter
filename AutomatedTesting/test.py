import os
import subprocess
import shutil
import filecmp
from colorama import init, Fore


def build_cpp_project():
    source_files_dir = os.getcwd() + "\\source_files"
    rpal20_exe = os.path.join(source_files_dir, "rpal20.exe")

    # Navigate to the source_files directory
    os.chdir(source_files_dir)

    # Run the 'make' command to build the C++ project
    try:
        subprocess.run(["make"], check=True)
    except subprocess.CalledProcessError:
        print("Error: Failed to build the C++ project.")
        exit(1)

    # Move rpal20.exe to the parent directory
    shutil.move(rpal20_exe, "..\\rpal20.exe")

    # Navigate back to the parent directory
    os.chdir("..")


def run_test_cases(test_folder):
    rpal20_exe = os.getcwd() + "\\rpal20.exe"
    rpal_exe = os.getcwd() + "\\rpal.exe"
    output_folder = os.getcwd() + "\\output"

    if not os.path.exists(output_folder):
        os.makedirs(output_folder)

    init()  # Initialize colorama

    test_files = sorted(os.listdir(test_folder))
    total_tests = len(test_files)
    passed_tests = 0
    failed_tests = []

    for test_file in test_files:
        test_input = os.path.join(test_folder, test_file)
        rpal20_output_file = os.path.join(output_folder, f"{test_file}_rpal20")
        rpal_output_file = os.path.join(output_folder, f"{test_file}_rpal")

        # Run tests with rpal20.exe
        subprocess.run(
            [rpal20_exe, test_input],
            stdout=open(rpal20_output_file, "w"),
            stderr=subprocess.DEVNULL,
        )

        # Run tests with rpal.exe
        subprocess.run(
            [rpal_exe, test_input],
            stdout=open(rpal_output_file, "w"),
            stderr=subprocess.DEVNULL,
        )

        # Compare output files
        if filecmp.cmp(rpal20_output_file, rpal_output_file):
            print(f"{test_file}: test passed")
            passed_tests += 1
        else:
            print(f"{test_file}: test failed")
            failed_tests.append(test_file)

    print("\nSummary:")
    print(f"Total test cases: {total_tests}")
    print(f"Passed test cases: {passed_tests}")
    print(f"Failed test cases: {total_tests - passed_tests}")
    if failed_tests:
        print("\nList of failed test cases:")
        for test_file in failed_tests:
            print(test_file)


if __name__ == "__main__":
    import sys

    if len(sys.argv) != 2:
        print("Usage: python test_script.py test_folder")
    else:
        test_folder = sys.argv[1]
        build_cpp_project()  # Build the C++ project in the source_files directory
        run_test_cases(test_folder)
