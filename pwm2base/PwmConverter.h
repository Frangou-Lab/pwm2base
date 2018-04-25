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

#ifndef PwmConverter_h
#define PwmConverter_h

#include <string>
#include <array>
#include <random>

#include "../libgene/source/log/Logger.hpp"

#include "PwmConverterBase.h"
#include "Common.h"

std::mt19937_64 mersenne_generator;
std::uniform_int_distribution<int> uniform2;
std::uniform_int_distribution<int> uniform3;
std::uniform_int_distribution<int> uniform4;

void InitRandom(bool verbose_output)
{
    std::random_device rd;
    auto seed = rd();
    
    if (verbose_output)
        logger::Log("Random seed: " + std::to_string(seed));
    
    mersenne_generator = std::mt19937_64{seed};
    uniform2 = std::uniform_int_distribution<int>{0, 1};
    uniform3 = std::uniform_int_distribution<int>{0, 2};
    uniform4 = std::uniform_int_distribution<int>{0, 3};
}

// Forward-declarations
template<int Size_> constexpr
char PickUniformlyRandomFromSet(const std::string& set) noexcept;

template<> constexpr
char PickUniformlyRandomFromSet<2>(const std::string& set) noexcept;

template<> constexpr
char PickUniformlyRandomFromSet<3>(const std::string& set) noexcept;

template<> constexpr
char PickUniformlyRandomFromSet<4>(const std::string& set) noexcept;
// End declarations


template<int _Size> constexpr
char PickUniformlyRandomFromSet(const std::string& set) noexcept
{
    static_assert(_Size > 1 && _Size < 5, "Attempted to instantiate PickUniformlyRandomFromSet with _Size beyound the specified limit");
    // Should be unreachable
    std::terminate();
    return '-';
}

template <> constexpr
char PickUniformlyRandomFromSet<2>(const std::string& set) noexcept
{
    return set[uniform2(mersenne_generator)];
}

template <> constexpr
char PickUniformlyRandomFromSet<3>(const std::string& set) noexcept
{
    return set[uniform3(mersenne_generator)];
}

template <> constexpr
char PickUniformlyRandomFromSet<4>(const std::string& set) noexcept
{
    return set[uniform4(mersenne_generator)];
}

class PwmConverterRandom : public PwmConverter {
 public:
    explicit PwmConverterRandom(Format output_format) : PwmConverter(output_format) {}
    
    virtual void Convert(std::string& id, std::string& pwm_sequence) override
    {
        bool should_output_dna = (output_format_ == Format::DNA);
        for (char& c: pwm_sequence) {
            switch (c) {
                case 'R': // A or G
                    c = PickUniformlyRandomFromSet<2>("AG");
                    break;
                case 'Y': // C or T
                    c = PickUniformlyRandomFromSet<2>(should_output_dna ? "CT" : "CU");
                    break;
                case 'S': // G or C
                    c = PickUniformlyRandomFromSet<2>("GC");
                    break;
                case 'W': // A or T
                    c = PickUniformlyRandomFromSet<2>(should_output_dna ? "AT" : "AU");
                    break;
                case 'K': // G or T
                    c = PickUniformlyRandomFromSet<2>(should_output_dna ? "GT" : "GU");
                    break;
                case 'M': // A or C
                    c = PickUniformlyRandomFromSet<2>("AC");
                    break;
                case 'B': // C or G or T
                    c = PickUniformlyRandomFromSet<3>(should_output_dna ? "CGT" : "CGU");
                    break;
                case 'D': // A or G or T
                    c = PickUniformlyRandomFromSet<3>(should_output_dna ? "AGT" : "AGU");
                    break;
                case 'H': // A or C or T
                    c = PickUniformlyRandomFromSet<3>(should_output_dna ? "ACT" : "ACU");
                    break;
                case 'V': // A or C or G
                    c = PickUniformlyRandomFromSet<3>("ACG");
                    break;
                case 'N': // any base
                    c = PickUniformlyRandomFromSet<4>(should_output_dna ? "ATGC" : "AUGC");
                    break;
                case 'T': // fallthrough
                case 'U':
                    c = (should_output_dna ? 'T' : 'U');
                    break;
                default:
                    // In this case the base should stay the same
                    // Those are omitted:
                    //
                    //    case 'A': // A (Adenine)  (doesn't change)
                    //    case 'C': // C (Cytosine) (doesn't change)
                    //    case 'G': // G (Guanine)  (doesn't change)
                    //    case '.': // gap
                    //    case '-': // gap
                    break;
            }
        }
    }
};

#endif /* PwmConverter_h */
