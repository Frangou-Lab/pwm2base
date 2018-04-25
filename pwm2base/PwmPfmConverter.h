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

#ifndef PwmPfmConverter_h
#define PwmPfmConverter_h

#include <vector>
#include <string>
#include <array>
#include <unordered_map>
#include <stdexcept>

#include "../libgene/source/file/sequence/SequenceRecord.hpp"
#include "../libgene/source/file/sequence/SequenceFile.hpp"
#include "../libgene/source/utils/Tokenizer.hpp"

#include "PwmConverterBase.h"

class PwmPfmConverter : public PwmConverter {
 private:
    struct PwmPfmBlock {
        std::string id;
        
        std::vector<int> a;
        std::vector<int> c;
        std::vector<int> g;
        std::vector<int> t;
    };
    
 public:
    explicit PwmPfmConverter(Format output_format)
    : PwmConverter(output_format)
    { }
    
    template<int Size_> constexpr
    int PickFromSetBasedOnMatrix(const std::array<int, 4>& weights)
    {
        double max_weight = 0.0;
        int arg_max = -1;
        for (int i = 0; i < 4; ++i) {
            if (weights[i] > max_weight) {
                arg_max = i;
                max_weight = weights[i];
            }
        }
        
        if (arg_max == -1) {
            // Coludn't select the base based on the matrix (all weights are zeroes, for example)
            return BaseToNumber(PickUniformlyRandomFromSet<Size_>(output_format_ == Format::DNA ?
                                                                  "ACGT" : "ACGU"));
        }
        return arg_max;
    }
    
    virtual void Convert(std::string& id, std::string& pfm_sequence) override
    {
        PwmPfmBlock pfm_block;
        std::string result;
        Tokenizer splitter(pfm_sequence, ' ');
        
        auto FillWithInts = [&splitter](std::vector<int>& v) {
            while (splitter.ReadNext()) {
                std::string value = splitter.GetNextToken();
                if (value.empty())
                    continue;
                
                if (value.size() == 1 && isalpha(value[0]))
                    continue;
                
                if (!value.empty()) {
                    if (value[0] == '[') {
                        if (value.size() > 1) {
                            value.erase(0, 1);
                        } else {
                            continue;
                        }
                    }

                    if (value[0] == ']')
                        break;
                }
                v.push_back(std::stoi(value));
            }
        };
        
        // Read 'A's
        FillWithInts(pfm_block.a);
        // Read 'C's
        FillWithInts(pfm_block.c);
        // Read 'G's
        FillWithInts(pfm_block.g);
        // Read 'T's
        FillWithInts(pfm_block.t);

        for (int i = 0; i < pfm_block.a.size(); ++i) {
            std::array<int, 4> weights;

            weights[0] = pfm_block.a[i];
            weights[1] = pfm_block.c[i];
            weights[2] = pfm_block.g[i];
            weights[3] = pfm_block.t[i];
            result += NumberToBase(PickFromSetBasedOnMatrix<4>(weights),
                                   output_format_);
        }
        pfm_sequence = std::move(result);
    }
};

#endif /* PwmPfmConverter_h */
