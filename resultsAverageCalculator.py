with open("results-m2-nested.txt", "r") as file:
    # Read all lines from the file
    lines = file.readlines()
alphaBetaAlgos = {"Naive Parallel Alpha Beta","Naive YBWC Parallel Alpha Beta", "YBWC", "PVS"}
minimaxAlgos = {"Parallel Minimax"}
allAlgos = alphaBetaAlgos | minimaxAlgos
def oppositeDepth(depth):
    if depth == 3:
        return 4
    else:
        return 3
relativeInnerSpeedups = {}
relativeToSeqSpeedups = {}
depth = 4
fen = None
baseTime = 1
for line in lines:
    if line.startswith("Current Fen: "):
        fen = line.split("Current Fen: ")[1].strip()
    elif line.startswith("Minimax moves: "):
        depth = oppositeDepth(depth)
    elif line.startswith("AlphaBeta moves: "):
        continue
    elif line.startswith("Sequential Minimax"):
        seqMinimax = float(line.split(",")[2].strip())
    elif line.startswith("Sequential AlphaBeta"):
        seqAlphaBeta = float(line.split(",")[2].strip())
    else:
        splitted = line.split(",")
        algo = splitted[0]
        threads = int(splitted[1])
        time = float(splitted[2])
        key = (fen, algo, threads, depth)
        if threads == 1:
            relativeInnerSpeedups[key] = 1
            baseTime = time
        else:
            relativeInnerSpeedups[key] = baseTime/time
        if algo in minimaxAlgos:
            relativeToSeqSpeedups[key] = seqMinimax/time
        elif algo in alphaBetaAlgos:
            relativeToSeqSpeedups[key] = seqAlphaBeta/time

for k,v in relativeInnerSpeedups.items():
    print(k, v)
print("gap")
for k,v in relativeToSeqSpeedups.items():
    print(k, v)
fens = [
    "8/8/8/8/8/1k6/2nb4/1K6 b - - 0 1",
    "1r6/8/8/8/8/1k6/8/K7 b - - 0 1",
    "1q6/8/8/8/8/1k6/8/K7 b - - 0 1",
    "1q6/8/8/8/8/1k6/PPP5/K7 b - - 0 1",
    "8/8/8/8/1k6/n2q4/PP6/K6R b - - 0 1",
    "5k2/4p2Q/8/5P2/b5R1/8/3P4/3KR3 w - - 0 1",
    "5k2/3p4/5K1P/8/8/8/8/8 w - - 0 1"
]

innerSpeedupResultsDepth5 = {(j,2**i):0.0 for i in range(4) for j in allAlgos}
relativeToSeqSpeedupResultsDepth5 = {(j,2**i):0.0 for i in range(4) for j in allAlgos}
innerSpeedupResultsDepth6 = {(j,2**i):0.0 for i in range(4) for j in allAlgos}
relativeToSeqSpeedupResultsDepth6 = {(j,2**i):0.0 for i in range(4) for j in allAlgos}
iterHelp = [str(j)+","+str(2**i) for i in range(4) for j in allAlgos]
threads = [2**i for i in range(4)]
for iterHelper in iterHelp:
    algo = iterHelper.split(",")[0]
    thread = int(iterHelper.split(",")[1])
    innerTotal = 0
    toSeqTotal = 0
    count = 0
    for depth in range(3,5):
        for fen in fens:
            key = (fen, algo, thread, depth)
            innerTotal += relativeInnerSpeedups[key]
            toSeqTotal += relativeToSeqSpeedups[key]
            count += 1

        innerAverage = innerTotal/count
        toSeqAverage = toSeqTotal/count
        if depth == 3:
            innerSpeedupResultsDepth5[(algo, thread)] = innerAverage
            relativeToSeqSpeedupResultsDepth5[(algo, thread)] = toSeqAverage
        else:
            innerSpeedupResultsDepth6[(algo, thread)] = innerAverage
            relativeToSeqSpeedupResultsDepth6[(algo, thread)] = toSeqAverage

print(innerSpeedupResultsDepth5)
print(innerSpeedupResultsDepth6)
print(relativeToSeqSpeedupResultsDepth5)
print(relativeToSeqSpeedupResultsDepth6)

# File to write the results
output_file = "averageMateIn2NestedSpeedups.txt"

with open(output_file, "w") as file:
    # Write header
    file.write("Average Speedups for Mate in 1\n\n")

    # Write inner speedups for depth 5, grouped by algorithm
    file.write("Inner Speedup Results (Depth 3):\n")
    for algo in allAlgos:
        file.write(f"Algorithm: {algo}\n")
        for thread in threads:
            speedup = innerSpeedupResultsDepth5.get((algo, thread), 0.0)
            file.write(f"  Threads: {thread}, Speedup: {speedup:.2f}\n")
        file.write("\n")

    # Write inner speedups for depth 6, grouped by algorithm
    file.write("Inner Speedup Results (Depth 4):\n")
    for algo in allAlgos:
        file.write(f"Algorithm: {algo}\n")
        for thread in threads:
            speedup = innerSpeedupResultsDepth6.get((algo, thread), 0.0)
            file.write(f"  Threads: {thread}, Speedup: {speedup:.2f}\n")
        file.write("\n")

    # Write relative to sequential speedups for depth 5, grouped by algorithm
    file.write("Relative to Sequential Speedup Results (Depth 3):\n")
    for algo in allAlgos:
        file.write(f"Algorithm: {algo}\n")
        for thread in threads:
            speedup = relativeToSeqSpeedupResultsDepth5.get((algo, thread), 0.0)
            file.write(f"  Threads: {thread}, Speedup: {speedup:.2f}\n")
        file.write("\n")

    # Write relative to sequential speedups for depth 6, grouped by algorithm
    file.write("Relative to Sequential Speedup Results (Depth 4):\n")
    for algo in allAlgos:
        file.write(f"Algorithm: {algo}\n")
        for thread in threads:
            speedup = relativeToSeqSpeedupResultsDepth6.get((algo, thread), 0.0)
            file.write(f"  Threads: {thread}, Speedup: {speedup:.2f}\n")
        file.write("\n")

print(f"Results have been written to {output_file}")




