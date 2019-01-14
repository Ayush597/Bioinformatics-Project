# Bionformatics project: Inducing the LCP-Array

This project is part of the [Bioinformatics course](https://www.fer.unizg.hr/en/course/bio) at the University of Zagreb, Faculty of Electrical Engineering and Computing.

## Project setup

Download the Escherichia coli genome sequence [here](ftp://ftp.ensemblgenomes.org/pub/bacteria/release-41/fasta/bacteria_63_collection/escherichia_coli_gca_000731455/dna/Escherichia_coli_gca_000731455.V1.dna.toplevel.fa.gz).
Unzip the file and place it in the `./data/` folder.

## Compiling and running the program

To compile, simply run `make` in the project root.
This will generate the executable file need to run the program. 
If you're on Linux, it will generate `sais_lcp_main.out`.
If you're on Windows, it will generate `sais_lcp_main.exe`.

The program needs the following arguments:
- `input_file`: the file to read the sequence from
- `output_file`: the output file for the  suffix array and LCP array
