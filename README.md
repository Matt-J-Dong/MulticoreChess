# Mulitcore Chess Project

In this project, we build a parallel chess engine using OpenMP and run experiments to explain the speed of the algorithms compared to their sequential counterpart. This program is made as part of the Multicore Processors class in NYU.

## Original repository

This chess engine is built upon the [Stockdory](https://github.com/TheBlackPlague/StockDory) chess system. We only used the chessboard logic, so we replaced the original repository's engine and evaluation function to implement our own.

## Instructions to run program (for Crunchy Machines)

1. Download the repository onto your local computer and unzip.
2. scp the folder to the Crunchy machines.
3. cd into the folder with cd:

```
cd MulticoreChess
```

4. Run the following commands

```
cmake -B Build -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -G Ninja
cmake --build Build --config Release
```
5. To run the program:

```
./Build/MulticoreChess <depth of search>
```

Note that the depth of search only applies to testing each function individually. It does not apply to the testing function (choice 8) - The testing function depths are fixed according to how we tested them.

The results are written onto disk with the name "results-mN.txt" where N represents the mate number.

### Additional Notes

Note that there are some extra commented out test cases at the bottom. These are either:
1. The same code as the normal tests, but removes argc and argv to run locally
2. The test cases for finding critical section entries and moves evaluated in YBWC and PVS

These are for the user to use for their own volition.

Note that we also commented out the mate in 2 fens and mate in 1 fens since running these all in one chunk would take a long time. To test the other FENs, uncomment them.