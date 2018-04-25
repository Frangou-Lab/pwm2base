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

#include "../libgene/source/file/sequence/SequenceFile.hpp"
#include "../libgene/source/utils/CppUtils.hpp"
#include "../libgene/source/utils/FileUtils.hpp"
#include "../libgene/source/utils/StringUtils.hpp"
#include "../libgene/source/def/Flags.hpp"

#include "Help.h"
#include "PwmConverter.h"
#include "PwmConverterWithWeights.h"
#include "PwmPfmConverter.h"

#include <iostream>
#include <string>
#include <random>
#include <cstdio>

int main(int argc, const char *argv[])
{
    ArgumentsParser arguments(argc, argv);
    
    if (arguments.input_path.empty()) {
        std::cerr << "No input files provided. Terminating\n";
        return 1;
    }
    
    InitRandom(arguments.verbose);
    std::unique_ptr<PwmConverter> converter;
    std::unique_ptr<PwmConverter> converter_pfm;
    try {
        if (arguments.matrix_file_provided) {
            converter = std::make_unique<PwmConverterWithWeights>(arguments.output_format);
            converter_pfm = std::make_unique<PwmPfmConverter>(arguments.output_format);
        } else {
            converter = std::make_unique<PwmConverterRandom>(arguments.output_format);
        }
    } catch (std::runtime_error err) {
        std::cerr << err.what() << '\n';
        return 1;
    }

    auto flags = std::make_unique<CommandLineFlags>();
    if (arguments.matrix_file_provided)
        flags->SetSetting(Flags::kInputFormat, "fasta");
    
    std::vector<std::unique_ptr<SequenceFile>> input_files;
    bool input_is_directory = false;
    if ((input_is_directory = utils::IsDirectory(arguments.input_path))) {
        if (arguments.input_path.back() != '/')
            arguments.input_path += '/';
        
        auto directory_contents = utils::GetDirectoryContents(arguments.input_path);
        for (const auto& path: directory_contents) {
            std::string extension = utils::GetExtension(path);

            if (extension != "txt" &&
                extension != "pfm" &&
                extension != "fasta" &&
                extension != "fa" &&
                extension != "fq") {
                // If the extension is none of those skip this file
                std::cerr << "Urecognized file extension '" << extension << "'. Skipping this file\n";
                continue;
            }

            auto input_file = SequenceFile::FileWithName(path, flags, OpenMode::Read);
            if (!input_file) {
                std::cerr << "File '" << path << "' couldn't be opened. Either it doesn't exist, or you don't have permissions to read it.\n";
                return 1;
            }
            input_files.emplace_back(std::move(input_file));
        }
    } else {
        auto input_file = SequenceFile::FileWithName(arguments.input_path,
                                                     flags,
                                                     OpenMode::Read);
        if (!input_file) {
            std::cerr << "Couldn't open input file '" << arguments.input_path << "'\n";
            return 1;
        }
        input_files.emplace_back(std::move(input_file));
    }
    
    if (input_files.empty()) {
        std::cerr << "No input files provided\n";
        return 1;
    }
    
    size_t dot_position = arguments.input_path.rfind('.');
    if (arguments.output_path.empty())
        arguments.output_path = arguments.input_path.substr(0, (input_is_directory ? arguments.input_path.size() - 1 : dot_position)) + "-bases" + ".tsv";
    
    std::unique_ptr<SequenceFile> out_file;
    if (!arguments.override_output) {
        FILE *test_out_file = fopen(arguments.output_path.c_str(), "wx");

        if (test_out_file == nullptr) {
            std::cout << "File '" << arguments.output_path << "' already exists. Do you wish to override it? [Y/n] ";
            char response;
            std::cin >> response;
            if (std::tolower(response) != 'y') {
                std::cerr << "Skipping file '" << arguments.input_path << "'\n";
                return 1;
            }
        } else {
            fclose(test_out_file);
        }
    }

    flags = std::make_unique<CommandLineFlags>();
    flags->SetSetting(Flags::kOutputFormat, "txt");
    if (!(out_file = SequenceFile::FileWithName(arguments.output_path,
                                                flags,
                                                OpenMode::Write))) {
        std::cerr << "Couldn't open the output file '" << arguments.output_path << "'\n";
        return 1;
    }
    
    SequenceRecord record;
    for (auto& input_file : input_files) {
        PwmConverter *selected_converter;
        
        if (utils::GetExtension(input_file->fileName()) == "pfm")
            selected_converter = converter_pfm.get();
        else
            selected_converter = converter.get();
        
        while (!(record = input_file->Read()).Empty()) {
            selected_converter->Convert(record.name, record.seq);
            std::string id = record.name;
            if (!record.desc.empty()) {
                id += ' ' + record.desc;
            }
            record.seq = '"' + id + '"' + '\t' + record.seq;
            out_file->Write(record);
        }
    }
    std::cout << "The output file is located at '" << arguments.output_path << "'\n";
    return 0;
}
