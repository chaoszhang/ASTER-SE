# Accurate Species Tree ALgorithm for PaRalogs and Orthologs (ASTRAL-Pro)
ASTRAL-Pro stands for ASTRAL for PaRalogs and Orthologs. ASTRAL is a tool for estimating an unrooted species tree given a set of unrooted gene trees and is statistically consistent under the multi-species coalescent model (and thus is useful for handling incomplete lineage sorting, i.e., ILS). ASTRAL-pro extends ASTRAL to allow multi-copy genes. ASTRAL-pro finds the species tree that has the maximum number of shared induced quartet tree equivalent classes with the set of gene trees, subject to the constraint that the set of bipartitions in the species tree comes from a predefined set of bipartitions. Please see the paper below for the definition of the PL-quartet scores, which is what ASTRAL-Pro optimizes. We refer to the tool both as A-Pro and ASTRAL-Pro. 

ASTER re-implements [ASTRAL-Pro](https://github.com/chaoszhang/A-pro) in a **faster**, **more accurate**, and **easier to install** and **lower memory consumption** way. However, ASTER version of original ASTRAL may be **slower** and **less accuracy** with few small gene trees. Please choose wisely!

## Publication:

Chao Zhang, Celine Scornavacca, Erin K Molloy, Siavash Mirarab, ASTRAL-Pro: Quartet-Based Species-Tree Inference despite Paralogy, Molecular Biology and Evolution, , msaa139, https://doi.org/10.1093/molbev/msaa139

# Bug Reports:

Contact ``aster-users@googlegroups.com`` or post on [ASTER issues page](https://github.com/chaoszhang/ASTER/issues).

# Documentations
- The rest of this file
- [README.md](../README.md)
- Forums:
  - [User group discussions](https://groups.google.com/forum/#!forum/aster-users)
  - [ASTER issues page](https://github.com/chaoszhang/ASTER/issues)

# INSTALLATION
See [README.md](../README.md).

# EXECUTION
ASTRAL-Pro currently has no GUI. You need to run it through the command-line. In a terminal/PowerShell, go to `bin`(Linux/Unix/WSL) or `exe`(Win) in the location where you have downloaded the software, find the name of your program of interest, and issue the following command:

```
./astral-pro
```

This will give you a list of options available. Windows program name should include `.exe` extension.

To find the species tree with input gene trees from in a file called `INPUT_FILE`, use:

```
./astral-pro INPUT_FILE
```

Currently, INPUT_FILE is hard-coded to be the ***last argument***. 

The results will be outputted to the standard output. To save the results in a file use the `-o OUTPUT_FILE` option before `INPUT_FILE`(**Strongly recommended**):

```
./astral-pro -o OUTPUT_FILE INPUT_FILE
```

To save the logs (**also recommended**), run:

```
./astral-pro -o OUTPUT_FILE INPUT_FILE 2>LOG_FILE
```

ASTRAL-Pro supports multi-threading. To run program with 4 threads, add `-t 4` before `INPUT_FILE`:

```
./astral-pro -t 4 -o OUTPUT_FILE INPUT_FILE 2>LOG_FILE
```

Example: 
```
./astral-pro -t 4 -o ../example/multitree.astral-pro.nw ../example/multitree.nw 2>../example/multitree.astral-pro.log
```

## Advanced Options

ASTRAL-Pro algorithm first performs `R` (4 by default) rounds of search and then repeatedly performs `S` (4 by default) rounds of subsampling and exploration until no improvement found.

```
./astral-pro -t T -r R -s S -o OUTPUT_FILE INPUT_FILE 2>LOG_FILE
```

When `T>min(R,S)` and the number of gene is small may **increase** running time due to parallel overheads. 

###### Input:
* The input gene trees are in the Newick format
* The input trees can have missing taxa and multiple genes per species.
* Different from ASTRAL, ASTRAL-Pro by default **does not allow polytomies (unresolved branches)** and **no guarentee of accuracy** is provided for allowing polytomies by force.
* When multiple genes from the same species are available, you can ask ASTRAL to force them to be together in the species tree. You can do this in two ways.
  1. You can give multiple genes from the same species the same name in the input gene trees.
  2. OR, a mapping file needs to be provided using the `-a` option. This mapping file should have one line per genes, and each line needs to be in the following formats:

```
gene_A1 species_name_A
gene_A2 species_name_A
gene_B1 species_name_B
gene_B2 species_name_B
gene_B3 species_name_B
...
```
###### Output:
The output in is Newick format and gives:

* the species tree topology
* branch lengths in coalescent units (only for internal branches)
* branch supports measured as [local posterior probabilities](http://mbe.oxfordjournals.org/content/early/2016/05/12/molbev.msw079.short?rss=1)
* It can also annotate branches with other quantities, such as quartet support, as described in the [tutorial](astral-tutorial.md).

The ASTRAL tree leaves the branch length of terminal branches empty. Some tools for visualization and tree editing do not like this (e.g., ape). In FigTree, if you open the tree several times, it eventually opens up (at least on our machines). In ape, if you ask it to ignore branch lengths all together, it works. In general, if your tool does not like the lack of terminal branches, you can add a dummy branch length, [as in this script](https://github.com/smirarab/global/blob/master/src/mirphyl/utils/add-bl.py).

### Other features (local posterior, bootstrapping):