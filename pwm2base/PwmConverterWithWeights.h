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

#ifndef PwmConverterWithMeights_h
#define PwmConverterWithMeights_h

#include <vector>
#include <string>
#include <array>
#include <unordered_map>
#include <stdexcept>

#include "../libgene/source/file/sequence/SequenceRecord.hpp"
#include "../libgene/source/file/sequence/SequenceFile.hpp"
#include "../libgene/source/utils/Tokenizer.hpp"

#include "PwmConverterBase.h"

class PwmConverterWithWeights : public PwmConverter {
 private:
    struct PwmBlock {
        std::string id;
        std::vector<std::array<double, 4>> stats;
    };
    
 public:
    explicit PwmConverterWithWeights(Format output_format)
    : PwmConverter(output_format)
    { }
    
    template<int Size_> constexpr
    int PickFromSetBasedOnMatrix(const std::array<double, 4>& weights)
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
    
    virtual void Convert(std::string& id, std::string& pwm_sequence) override
    {
        PwmBlock block;
        block.id = id;
        std::string result;
        Tokenizer splitter(pwm_sequence, '\t');
        
        int i = 0;
        std::array<double, 4> statistics;

        while (splitter.ReadNext()) {
            statistics[i++] = std::stod(splitter.GetNextToken());
            if (i == 4) {
                block.stats.push_back(statistics);
                i = 0;
            }
        }
        
        for (const auto& weights : block.stats) {
            result += NumberToBase(PickFromSetBasedOnMatrix<4>(weights), output_format_);
        }
        pwm_sequence = std::move(result);
    }
};

#endif /* PwmConverterWithMeights_h */
