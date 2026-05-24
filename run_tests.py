

#!/usr/bin/env python3
import os
import argparse
import subprocess

COMPILATION_FLAGS = "-std=c++14 -DNDEBUG -Wall"
TIMEOUT = 10


def run_test(exe_file, test_id, tests_dir):
    """
    Run a single test:
    - Reads input from test{ID}.in
    - Writes student's output to test{ID}.res
    - Compares against test{ID}.out
    Returns True if the output matches, False otherwise.
    """
    input_file = os.path.join(tests_dir, f"test{test_id}.in")
    expected_output_file = os.path.join(tests_dir, f"test{test_id}.out")
    result_file = os.path.join(tests_dir, f"test{test_id}.res")

    if not os.path.isfile(input_file):
        print(f"Input file for test {test_id} not found: {input_file}")
        return False

    if not os.path.isfile(expected_output_file):
        print(f"Expected output file for test {test_id} not found: {expected_output_file}")
        return False

    # Execute the compiled binary with a timeout
    cmd = [exe_file]
    try:
        with open(input_file, "r") as stdin, open(result_file, "w") as stdout:
            subprocess.run(
                cmd,
                stdin=stdin,
                stdout=stdout,
                timeout=TIMEOUT,
                check=True,
            )
    except subprocess.TimeoutExpired:
        print(f"Test {test_id} Failed: Execution timed out after {TIMEOUT} seconds.")
        return False
    except subprocess.CalledProcessError as e:
        print(f"Test {test_id} Failed: Command execution error. {e}")
        return False

    # Check result against expected output
    if not os.path.isfile(result_file):
        print(f"Test {test_id} Failed: result file not created")
        return False

    with open(result_file, "r") as res, open(expected_output_file, "r") as expected:
        student_out = res.read().strip()
        expected_out = expected.read().strip()

        if student_out == expected_out:
            print(f"Test {test_id} Passed")
            return True
        else:
            print(f"Test {test_id} Failed: Output does not match expected.")
            return False


def clean_res_files(tests_dir):
    """
    Remove all .res files from tests_dir.
    """
    for fname in os.listdir(tests_dir):
        if fname.endswith(".res"):
            path = os.path.join(tests_dir, fname)
            try:
                os.remove(path)
                print(f"Removed {path}")
            except OSError as e:
                print(f"Could not remove {path}: {e}")


def compile_sources(compiler, code_dir, exe_file):
    """
    Compile all .cpp files in code_dir into exe_file.
    Returns True on success, False on failure.
    """
    cpp_files = [f for f in os.listdir(code_dir) if f.endswith(".cpp")]
    if not cpp_files:
        print(f"No .cpp files found in code_dir: {code_dir}")
        return False

    cpp_paths = [os.path.join(code_dir, f) for f in cpp_files]
    cmd = [compiler] + COMPILATION_FLAGS.split() + ["-o", exe_file] + cpp_paths

    print("Compiling:", " ".join(cmd))
    try:
        subprocess.run(cmd, check=True)
    except subprocess.CalledProcessError as e:
        print(f"Compilation failed with error: {e}")
        return False

    if not os.path.isfile(exe_file):
        print(f"Compilation succeeded, but the executable {exe_file} was not created.")
        return False

    return True


def main():
    parser = argparse.ArgumentParser(description="Run tests against expected outputs.")
    parser.add_argument(
        "--tests_dir",
        type=str,
        default="./tests/",
        help="Path to the dir with the tests to run (default: './tests/').",
    )
    parser.add_argument(
        "--code_dir",
        type=str,
        default="./",
        help="Path to the dir with the code to compile and test (default: './').",
    )
    parser.add_argument(
        "--compiler_path",
        type=str,
        default="g++",
        help="Path to the g++ compiler (default: 'g++').",
    )
    parser.add_argument(
        "--clean",
        action="store_true",
        help="Remove all .res files from the tests dir and exit.",
    )
    parser.add_argument(
        "--abort_on_fail",
        action="store_true",
        help="Abort on first test that fails.",
    )
    parser.add_argument(
        "-t",
        "--tests",
        type=int,
        nargs="*",
        help="List of test IDs to run (default: run all tests).",
        default=None,
    )
    args = parser.parse_args()

    # Handle cleaning without compiling
    if args.clean:
        clean_res_files(args.tests_dir)
        return 0

    exe_file = os.path.join(args.code_dir, "main.out")

    # Compile student code
    if not compile_sources(args.compiler_path, args.code_dir, exe_file):
        return -1

    # Get the list of tests to run
    if args.tests is None:
        tests = [
            int(f.split("test")[1].split(".in")[0])
            for f in os.listdir(args.tests_dir)
            if f.startswith("test") and f.endswith(".in")
        ]
        tests = sorted(set(tests))
    else:
        tests = args.tests

    if not tests:
        print("No tests to run.")
        return 0

    all_passed = True
    for test_id in tests:
        ok = run_test(exe_file, test_id, args.tests_dir)
        if not ok:
            all_passed = False
            if args.abort_on_fail:
                print("Aborting on first failed test (as requested).")
                return 1

    return 0 if all_passed else 1


if __name__ == "__main__":
    raise SystemExit(main())