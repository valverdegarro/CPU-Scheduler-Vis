# SOA
Repository for Advance Operative Systems.

## Pre-requisites

### Unity set up
Run
`git submodule init`
`git submodule update`

## Run systrace
To run systrace go to `[repo]\tareas\primera` and execute:
`make`

To run unit test execute:
`make unittest`

## Add an unit test
- Write test in unittest.c file following name convention `test__(method name to test)__(pass or fail)`.
- Add test in runner file.
