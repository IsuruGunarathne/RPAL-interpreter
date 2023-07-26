# RPAL Interpreter

## Running

use `make` to compile the program

use `./rpal20 <filename>` to run a program

## Files
- lexer.h
- CSE.h
- Parser.h
- Token.h
- TokenStorage.h
- Tree.h
- TreeNode.h
- main.cpp

## Testing

### Manual

runs all test cases and provides indivitual outputs
to run use `cmd` and run `make` followed by `test`

### Automated

runs all test cases and compares outputs with default RPAL interpreter outputs.
To run use navigate to `AutomatedTesting` copy source files into source_files folder then run `python <testscript>.py <tests_folder>`
where `<testscript>` is the name of the test script and `<tests_folder>` is the name of the folder containing the test cases.
eg: `python test.py tests`
