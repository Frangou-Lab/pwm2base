/*
 * Copyright 2018 Frangou Lab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef Help_h
#define Help_h

#include "Common.h"

#include <string>
#include <iostream>

inline void PrintHelp(FILE *destination)
{
    fprintf(destination, "\
OVERVIEW:\n\
PWM to RNA/DNA bases converter v0.3\n\
Build date: %s\n\
\n", __DATE__);
    fprintf(destination, "\
USAGE: pwm2base [options] <input path> [-o <output path>]\n\
\n\
OPTIONS:\n\
-h                     - Show this message\n\
-v                     – Verbose output (print the random seed used for sequence generation)\n\
-s <input path>        - Path to PWM sequences file\n\
-m <input path>        - Path to PWM weights file\n\
-o <output path>       - Assign a custom output name instead of an auto-generated one\n\
-f                     - Always override output file\n\
-dna (default)         - Produce DNA output sequences\n\
-rna                   - Produce RNA output sequences\n\
\n\
EXAMPLES:\n\
pwm2base -s ~/Documents/pwm_file.txt           - Convert a PWM file '~/Documents/pwm_file.txt' into DNA bases. The output will be located in the same folder the\n\
                                                 input file is, with an added suffix '-bases' appended to its name.\n\
\n\
pwm2base -m ~/Example_3_HUMAN_PWM.fasta        - Convert a PWM .fasta (or .txt) file '~/Example_3_HUMAN_PWM.fasta' into the file containing DNA sequences.\n\
\n\
pwm2base -m ~/PWM_Matrices/                    - Convert all files in the directory '~/PWM_Matrices/' into the file containing DNA sequences. The output\n\
                                                 will be located in '~/PWM_Matrices-bases.tsv'.\n\
\n\
pwm2base -m ~/jaspar2016.pfm                   - Convert a PWM .pfm(JASPAR2016) file '~/jaspar2016.pfm' into the file containing DNA sequences.\n\
\n\
pwm2base -m ~/Example_3_HUMAN_PWM.fasta -rna   - Convert a PWM .fasta (or .txt) file '~/Example_3_HUMAN_PWM.fasta' into the file containing RNA sequences.\n\
\n\
pwm2base -s ~/Documents/pwm_file.fasta         - Convert PWM FASTA file '~/Documents/pwm_file.fasta' into DNA/RNA bases. The output will be in FASTA format as well\n\
                                                 and it will be located in the same folder as the input, but with '-bases' suffix appended to its name.\n\
\n\
pwm2base -s ~/Documents/pwm_file.txt -o ./output.tsv        - Convert PWM .txt file '~/Documents/pwm_file.txt' into the file containing DNA bases.\n\
                                                               The output will be located in the current directory with a name 'output.tsv'.\n\
\n\
pwm2base -rna -m ~/Example_3_HUMAN_PWM.txt  -o ./output.tsv - Convert PWM .txt (.tsv) file '~/Example_3_HUMAN_PWM.txt' into the file containing RNA sequences.\n\
                                                              The output will be located in the current directory with a name 'output.tsv'.\n\
\n");
}

class ArgumentsParser {
 public:
    std::string input_path;
    std::string output_path;
    
    Format output_format{Format::DNA};
    bool matrix_file_provided{false};
    bool verbose{false};
    bool override_output{false};
    
    ArgumentsParser(int argc, const char *argv[])
    {
        if (argc == 1) {
            PrintHelp(stdout);
            return;
        }
        
        for (int i = 1; i < argc; ++i) {
            std::string arg{argv[i]};

            if (arg == "-v") {
                verbose = true;
            } else if (arg == "-s") {
                i++;
                if (i == argc || argv[i][0] == '-') {
                    std::cerr << "No input sequence file file provided. Aborting\n";
                    std::exit(1);
                }

                if (!input_path.empty()) {
                    std::cerr << "Can't provide both '-s' and '-m' files. Aborting\n";
                    std::exit(1);
                }
                input_path.assign(argv[i]);
            } else if (arg == "-m") {
                i++;
                if (i == argc || argv[i][0] == '-') {
                    std::cerr << "No weights matrix file provided. Aborting\n";
                    std::exit(1);
                }

                if (!input_path.empty()) {
                    std::cerr << "Can't provide both '-s' and '-m' files. Aborting\n";
                    std::exit(1);
                }
                matrix_file_provided = true;
                input_path.assign(argv[i]);
            } else if (arg == "-o") {
                i++;
                if (i == argc || argv[i][0] == '-') {
                    std::cerr << "No valid output path entered. Aborting\n";
                    std::exit(1);
                }
                output_path.assign(argv[i]);
            } else if (arg == "-h") {
                PrintHelp(stdout);
                break;
            } else if (arg == "-rna") {
                output_format = Format::RNA;
            } else if (arg == "-dna") {
                output_format = Format::DNA;
            } else if (arg == "-f") {
                override_output = true;
            } else if (!arg.empty() && arg[0] != '-') {
                input_path.assign(argv[i]);
            } else {
                PrintHelp(stderr);
                std::cerr << "Unknown flag '" << arg << "'\n";
                std::exit(1);
            }
        }
    }
};


#endif /* Help_h */
