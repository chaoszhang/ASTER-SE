# faster
F81-based Accurate Species Tree EstimatoR

# Compile for Linux/Unix
g++ -std=gnu++11 -pthread faster.cpp -o faster

# Run
faster [-o oFilePath -r nRound -s nSample -p probability -t nThread] inputlist

# Assumptions for Statistical Consistency
## The multi-species coalescent model
1. The gene trees are generated independently, and as the number of genes goes to infinity, FASTER is statistically consistent.
2. The coalescent units do not need to be the same across branches.

## Gene tree and sequence model
1. The mutation rates (per time) do not need to be the same across branches, and within each branch across different time, the mutation rate does not need to scale the same way as coalescent does, as long as being reasonable (e.g. infimum/minimum above zero and capped).
2. The length of each gene can be arbitrary and may be dependent on parameters above, as long as being reasonable (e.g. infimum/minimum above zero and capped).

## Felsenstein 1981 model-like
1. Base frequencies are provided and allowed to vary from 0.25, but the rate matrix must be F81-like.
2. The sum of top 2 base frequences must be less than 1. In other words, the number of categories must be at least 3, which unfortunately excludes binary inputs (e.g. major or minor alleles) but allowing nucleotides (4) and amino acids (20). (Base positions with the number of effective categories no more than 2 will neither contribute to nor bias the inferred species tree.)
3. Different base positions (or genes) are allowed to have different base frequencies and be depedent on parameters above, as long as being reasonable (e.g. non-zero for at least 3 categories) and provided.
