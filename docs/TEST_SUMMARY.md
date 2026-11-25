# Verification Summary

The consolidated build was compiled with:

`gcc -std=c11 -Wall -Wextra -Wpedantic src/*.c -Iinclude -lm -o student_analyzer`

The strict build completed without compiler warnings in `build.log`.

## Automated integrity checks
See `QA_RESULTS.txt`. The final clean run reported `QA failures: 0` and verified:
- existing/missing student lookup
- duplicate roll detection
- mark-record lookup
- Aman result calculation
- subject failure forcing overall FAIL
- competition ranking tie behavior
- backup and restore
- stale-result invalidation
- cascade deletion of student + marks + results
- grade-gap rejection
- grade-overlap rejection
- weak-threshold validation

## Runtime demonstration checks
See `DEMO_RUNTIME_OUTPUT.txt`.
- 10 students and 5 subjects loaded
- tie ranking: Riya and Arjun both Rank 2
- class average: 73.20%
- pass rate: 90.00%
- Rahul weak subjects: Mathematics, Electronics, Physics
- Aman report generated successfully

The project should still be compiled and smoke-tested once on the user's own Windows/VS Code/MinGW environment before the live hackathon because compiler/runtime environments can differ.
