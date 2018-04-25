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

#ifndef Utils_h
#define Utils_h

enum class Format {
    PWM,
    RNA,
    DNA
};


//
// Convert base (ACGT) into its number in accordance with the file format that Gus
// provided.
//
constexpr
int8_t BaseToNumber(char base)
{
    switch (base) {
        case 'A': return 0;
        case 'C': return 1;
        case 'G': return 2;
        case 'T': [[fallthrough]];
        case 'U': return 3;
        default: return -1;
    }
}

//
//
//
constexpr
char NumberToBase(uint8_t index, Format output_format = Format::DNA)
{
    switch (index) {
        case 0: return 'A';
        case 1: return 'C';
        case 2: return 'G';
        case 3: return (output_format == Format::DNA) ? 'T' : 'U';
        default: return -1;
    }
}

#endif /* Utils_h */
