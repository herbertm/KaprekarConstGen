/*
 * Copyright (c) 2015-2016, Herbert Meiler
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <time.h>
#include <iostream>

#include "KaprekarConstGen.h"

void Generate(std::ostream& os, int length, int base)
{
    struct Collector
    {
        explicit Collector(std::ostream& os) :
            os(os),
            count(0)
        {
        }

        inline int Count() const
        {
            return count;
        }
        
        inline void Collect(const std::vector<int>& result)
        {
            ++count;

            for (auto it = result.rbegin(); it != result.rend(); ++it)
            {
                char digit = (char)*it;
                digit += (digit < 10) ? '0' : ('A' - 10);

                os << digit;
            }
            os << std::endl;
        }

    private:

        std::ostream& os;
        int count;
    };

    //


    const clock_t start = clock();

    Collector collector(os);
    KaprekarConstGen(base).Generate(length, collector);

    const clock_t end = clock();

    const double time = ((double)(end - start)) / CLK_TCK;

    os << std::endl;
    os << "Base    : " << base << std::endl;
    os << "Length  : " << length << std::endl;
    os << "Results : " << collector.Count() << std::endl;
    os << "Duration: " << time << " seconds" << std::endl;
    os << std::endl;
}

void Usage(std::ostream& os)
{
    os << "usage: KaprekarConstGen length (base)" << std::endl;
    os << std::endl;
    os << "- length > 0" << std::endl;
    os << "- base [2;16], optional (default = 10)" << std::endl;
    os << std::endl;
}

int
main(int argc, char* argv[])
{
    std::ostream& os = std::cout;

    if ((argc != 2) && (argc != 3))
    {
        Usage(os);
    }
    else
    {
        int length = atoi(argv[1]);
           
        int base = 10;
        if (argc > 2)
        {
            base = atoi(argv[2]);
        }

        if ((length < 1) || (base < 2) || (base > 16))
        {
            Usage(os);
        }
        else
        {
            Generate(os, length, base);
        }
    }
} 
