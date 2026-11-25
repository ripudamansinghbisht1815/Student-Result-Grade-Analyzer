# Student Result & Grade Analyzer

A complete C11 console-based academic performance management and analytics project for a college hackathon.

## Core features
- Admin login with three-attempt lockout and password change
- Automatic student IDs; add, view, search, update and cascade-delete students
- Semester-aware subject management with maximum/pass marks and comparison groups
- Marks entry, display and update with validation and stale-result invalidation
- Complete-result protection: final results are refused when required subject marks are missing
- Configurable grading stored in files, with gap/overlap validation
- Subject-level pass/fail plus overall percentage and grade
- Weak-subject detection with configurable threshold and priority ordering
- Competition ranking with correct ties (1, 2, 2, 4)
- Top-five performers
- Class and subject analytics
- Semester comparison plus comparable-subject-group changes
- Persistent text report generation
- Academic dashboard
- Backup and restore
- Binary-file persistence

## Build
From the project root:

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic src/*.c -Iinclude -lm -o student_analyzer
```

Windows/MinGW:

```powershell
.\student_analyzer.exe
```

Linux/macOS:

```bash
./student_analyzer
```

Run from the project root so the relative `data/`, `reports/`, and `backup/` paths resolve correctly.

## Demo login
- Username: `admin`
- Password: `admin123`

Change the password from System Settings before a real demonstration. Authentication is intentionally demonstration-level local access control, not production security.

## Project layout
- `include/` — structures, constants and function declarations
- `src/` — application implementation
- `data/` — persistent binary data files
- `reports/` — generated student reports
- `backup/` — backup copies
- `docs/` — project notes/test evidence

## Verified sample dataset
The supplied demo data contains 10 CSE students and 5 Semester-1 subjects. It deliberately includes a ranking tie and a failing student so the analytics can be demonstrated.

Known verified values:
- Aman Sharma: 371/500 = 74.20%, B+, PASS
- Riya Verma and Arjun Singh: 86.00%, both Rank 2
- Rahul Kumar: 46.00%, FAIL because Mathematics and Electronics are below passing marks
- Semester-1 class average: 73.20%
- Pass rate: 90.00%

## Important engineering rules
- Student IDs are immutable identifiers.
- Deleting a student also removes their marks and calculated results.
- Updating marks invalidates that student's stored result for the semester.
- Changing grade rules invalidates saved calculated results so stale grades are not retained.
- Results are finalized only when all registered subjects for the semester have marks.
- Grade ranges must cover 0–100 without gaps or overlaps.

## Hackathon positioning
This is not only a marks calculator. It persists academic records and converts them into weak-subject analysis, rankings, class/subject statistics, semester trends, and saved individual reports.
