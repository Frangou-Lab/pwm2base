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

#ifndef PwmConverterBase_h
#define PwmConverterBase_h

#include "Common.h"

#include <string>

class PwmConverter {
 public:
    PwmConverter(Format output_format) : output_format_(output_format) {}
    virtual void Convert(std::string& id, std::string& pwm_sequence) = 0;

 protected:
    Format output_format_{Format::DNA};
};

#endif /* PwmConverterBase_h */
