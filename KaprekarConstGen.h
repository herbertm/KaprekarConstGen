// KaprekarConstGen.h, (c) Herbert Meiler, June 2015

#ifndef __KAPREKARCONSTGEN_H__
#   define __KAPREKARCONSTGEN_H__

#   include <cassert>
#   include <algorithm>
#   include <vector>

class KaprekarConstGen
{
private:

    class CandidateDistribution
    {
    public:

        explicit CandidateDistribution(int base = 10) :
            base(base),
            length(0),
            differentDigits(0),
            frequency(base, 0),
            outerDigit1(-1),
            outerDigit2(-1)
        {
            assert(base >= 2);
        }

        int Base() const noexcept
        {
            return base;
        }

        int Length() const noexcept
        {
            return length;
        }

        int DifferentDigits() const noexcept
        {
            return differentDigits;
        }

        void SetFreq(int digit, int frequency) noexcept
        {
            const int difference = frequency - GetFreq(digit);
            if (difference > 0)
            {
                IncFreq(digit, difference);
            }
            else
            {
                DecFreq(digit, -difference);
            }
        }

        int GetFreq(int digit) const noexcept
        {
            assert(digit >= 0);
            assert(digit < frequency.size());

            return frequency[digit];
        }

        int IncFreq(int digit, int frequency = 1) noexcept
        {
            assert(digit >= 0);
            assert(digit < this->frequency.size());

            assert(frequency >= 0);

            int& digitFrequency = this->frequency[digit];

            if (frequency > 0)
            {
                if (digitFrequency == 0)
                {
                    ++differentDigits;
                }

                //

                length += frequency;
                digitFrequency += frequency;

                assert(digitFrequency > 0);
            }

            return digitFrequency;
        }

        int DecFreq(int digit, int frequency = 1) noexcept
        {
            assert(digit >= 0);
            assert(digit < this->frequency.size());

            assert(frequency >= 0);

            int& digitFrequency = this->frequency[digit];

            if (frequency > 0)
            {
                length -= frequency;
                digitFrequency -= frequency;

                //

                if (digitFrequency == 0)
                {
                    --differentDigits;
                }
            }

            return digitFrequency;
        }

        template<typename Handler>
        bool SubtractMaxMin(Handler& handler) const noexcept
        {
            assert(length > 0);
            assert(differentDigits > 0);

            assert((length == 1) || (differentDigits > 1));

            int distributionValueHi = base;
            while (frequency[--distributionValueHi] == 0)
            {
            }

            int distributionValueLo = 0 - 1;
            while (frequency[++distributionValueLo] == 0)
            {
            }

            // first digit (borrow = 0)

            int digitValue = distributionValueLo - distributionValueHi;

            if (digitValue == 0)
            {
                assert(length == 1);
                return handler(0, digitValue, 1);
            }
            else
            {
                assert(digitValue < 0);
                assert(length > 1);

                digitValue += base;
            }

            assert(digitValue >= 0);
            assert(digitValue < base);

            if (!handler(0, digitValue, 1))
            {
                return false;
            }

            // exclude duplicates!

            assert(outerDigit1 != 0);
            assert(outerDigit2 != 0);

            if (outerDigit1 > 0)
            {
                assert(outerDigit2 > 0);

                if ((digitValue != outerDigit1) && (digitValue != outerDigit2))
                {
                    return false;
                }
            }

            // -> borrow = 1

            int digitIndex = 0 + 1;

            int distributionValueHiIndex = 0;
            int distributionValueLoIndex = 0;

            int distributionFrequencyHi = frequency[distributionValueHi];
            int distributionFrequencyLo = frequency[distributionValueLo];

            auto nextHi = [&distributionValueHi, &distributionValueHiIndex, &distributionFrequencyHi, this](int count = 1)
            {
                if ((distributionValueHiIndex += count) >= distributionFrequencyHi)
                {
                    while (frequency[--distributionValueHi] == 0)
                    {
                    }

                    distributionValueHiIndex = 0;

                    distributionFrequencyHi = frequency[distributionValueHi];
                }
            };

            auto nextLo = [&distributionValueLo, &distributionValueLoIndex, &distributionFrequencyLo, this](int count = 1)
            {
                if ((distributionValueLoIndex += count) >= distributionFrequencyLo)
                {
                    while (frequency[++distributionValueLo] == 0)
                    {
                    }

                    distributionValueLoIndex = 0;

                    distributionFrequencyLo = frequency[distributionValueLo];
                }
            };

            nextHi();
            nextLo();

            //

            while (distributionValueLo <= distributionValueHi)
            {
                const int digitCount = std::min(
                    distributionFrequencyHi - distributionValueHiIndex,
                    distributionFrequencyLo - distributionValueLoIndex);
                assert(digitCount > 0);

                digitValue = distributionValueLo - distributionValueHi - 1 + base;
                assert(digitValue >= 0);
                assert(digitValue < base);

                if (!handler(digitIndex, digitValue, digitCount))
                {
                    return false;
                }

                nextHi(digitCount);
                nextLo(digitCount);

                digitIndex += digitCount;
            }

            // -> borrow == 1

            digitValue = distributionValueLo - distributionValueHi - 1;
            assert(digitValue >= 0);
            assert(digitValue < base);

            if (!handler(digitIndex, digitValue, 1))
            {
                return false;
            }

            if (++digitIndex == length)
            {
                return true;
            }

            //

            nextHi();
            nextLo();

            //

            // -> borrow == 0;

            for (;;)
            {
                const int digitCount = std::min(
                    distributionFrequencyHi - distributionValueHiIndex,
                    distributionFrequencyLo - distributionValueLoIndex);
                assert(digitCount > 0);

                digitValue = distributionValueLo - distributionValueHi;
                assert(digitValue >= 0);
                assert(digitValue < base);

                //

                if (!handler(digitIndex, digitValue, digitCount))
                {
                    return false;
                }

                //

                digitIndex += digitCount;
                if (digitIndex == length)
                {
                    return true;
                }

                assert(digitIndex < length);

                //

                nextHi(digitCount);
                nextLo(digitCount);
            }
        }

        void SetOuterDigit() noexcept
        {
            SetOuterDigit(-1, -1);
        }

        void SetOuterDigit(int digit1, int digit2) noexcept
        {
            assert(((digit1 < 0) && (digit2 < 0)) ||
                (((digit1 + digit2) == base) && (digit1 > 0) && (digit1 < base)));

            outerDigit1 = digit1;
            outerDigit2 = digit2;
        }

        const std::vector<int>& Distribution() const noexcept
        {
            return frequency;
        }

    private:

        const int base;

        int length;
        int differentDigits;

        std::vector<int> frequency;

        int outerDigit1;
        int outerDigit2;
    };

public:

    explicit KaprekarConstGen(int base = 10) noexcept :
        base(base)
    {
        assert(base >= 2);
    }

    template<typename Collector>
    void Generate(int length, Collector& collector)
    {
        assert(length > 0);

        CandidateDistribution candidate(base);

        if (length == 1)
        {
            // special treatment for length = 1 (brute force :-))

            for (int digit = 0; digit != base; ++digit)
            {
                candidate.IncFreq(digit);

                Calculate(candidate, length, collector);

                candidate.DecFreq(digit);
            }
        }
        else if ((length < 4) || (base == 2))
        {
            // special treatment (no outer digits and compensation)

            Generate(candidate, length, collector);
        }
        else
        {
            for (int outerDigit1 = 1; outerDigit1 <= base / 2; ++outerDigit1)
            {
                const int outerDigit2 = base - outerDigit1;

                candidate.IncFreq(outerDigit1);
                candidate.IncFreq(outerDigit2);

                candidate.SetOuterDigit(outerDigit1, outerDigit2);

                //

                const int outerCompensation = base - 2;

                for (int outerCompensationDigit1 = 0;
                    outerCompensationDigit1 <= outerCompensation / 2;
                    ++outerCompensationDigit1)
                {
                    const int outerCompensationDigit2 = outerCompensation - outerCompensationDigit1;

                    candidate.IncFreq(outerCompensationDigit1);
                    candidate.IncFreq(outerCompensationDigit2);

                    Generate(candidate, length, collector);

                    candidate.DecFreq(outerCompensationDigit1);
                    candidate.DecFreq(outerCompensationDigit2);
                }

                //

                candidate.DecFreq(outerDigit1);
                candidate.DecFreq(outerDigit2);
            }
        }
    }

private:

    template<typename Collector>
    void Generate(
        CandidateDistribution& candidate,
        int length,
        Collector& collector)
    {
        const int digit = base - 1;
        const int restLength = length - candidate.Length();

        for (int frequency = (restLength & 1); frequency <= restLength; frequency += 2)
        {
            candidate.IncFreq(digit, frequency);

            //

            Generate(candidate, length, 0, collector);

            //

            candidate.DecFreq(digit, frequency);
        }
    }

    template<typename Collector>
    void Generate(
        CandidateDistribution& candidate,
        int length,
        int digit1,
        Collector& collector)
    {
        const int digit2 = base - 1 - digit1;

        assert(digit1 <= digit2);

        const int restLength = length - candidate.Length();
        assert((restLength & 1) == 0);

        const int nextDigit1 = digit1 + 1;
        const int nextDigit2 = digit2 - 1;

        if (nextDigit1 > nextDigit2)
        {
            const int frequency = restLength >> 1;

            candidate.IncFreq(digit1, frequency);
            candidate.IncFreq(digit2, frequency);

            Calculate(candidate, length, collector);

            candidate.DecFreq(digit1, frequency);
            candidate.DecFreq(digit2, frequency);
        }
        else
        {
            for (int frequency = 0; frequency <= restLength >> 1; ++frequency)
            {
                candidate.IncFreq(digit1, frequency);
                candidate.IncFreq(digit2, frequency);

                Generate(candidate, length, nextDigit1, collector);

                candidate.DecFreq(digit1, frequency);
                candidate.DecFreq(digit2, frequency);
            }
        }
    }

    template<typename Collector>
    void Calculate(
        CandidateDistribution& candidate,
        int length,
        Collector& collector)
    {
        assert(candidate.Length() == length);

        if (length > 1)
        {
            if (candidate.DifferentDigits() < 2)
            {
                return;
            }
        }

        //

        class FilterHandler
        {
        public:

            explicit FilterHandler(const CandidateDistribution& candidate) :
                candidate(candidate),
                result(candidate.Base())
            {
            }

            bool operator()(int digitIndex, int digitValue, int digitCount) noexcept
            {
                return (result.IncFreq(digitValue, digitCount) <= candidate.GetFreq(digitValue));
            }

        private:

            const CandidateDistribution& candidate;
            CandidateDistribution result;
        };

        FilterHandler handler(candidate);
        if (candidate.SubtractMaxMin(handler))
        {
            struct ResultHandler
            {
                explicit ResultHandler(int length) :
                    result(length)
                {
                }

                bool operator()(int digitIndex, int digitValue, int digitCount) noexcept
                {
                    auto it = result.begin() + digitIndex;
                    std::fill(it, it + digitCount, digitValue);

                    return true;
                }

                const std::vector<int>& Result() const noexcept
                {
                    return result;
                }

            private:

                std::vector<int> result;
            };
            
            ResultHandler resultHandler(length);
            candidate.SubtractMaxMin(resultHandler);

            collector.Collect(resultHandler.Result());
        }
    }

    //

    const int base;
};

#endif
