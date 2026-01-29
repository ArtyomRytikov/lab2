#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "../structures/dictionary.h"
#include "../structures/Sequence.h"
#include "../structures/DynamicArray.h"

// Функция для преобразования числа в строку
template<typename T>
DynamicArray<char> NumberToString(T value) 
{
    DynamicArray<char> result;
    
    if (value == 0) 
    {
        result.InsertAt('0', 0);
        return result;
    }
    
    bool negative = false;
    if (value < 0) {
        negative = true;
        value = -value;
    }
    
    // Собираем цифры в обратном порядке
    DynamicArray<char> digits;
    while (value > 0) 
    {
        char digit = '0' + (value % 10);
        digits.InsertAt(digit, 0);
        value /= 10;
    }
    
    if (negative) 
    {
        result.InsertAt('-', 0);
    }
    
    // Копируем цифры в результат
    for (int i = 0; i < digits.GetSize(); i++) 
    {
        result.InsertAt(digits.Get(i), result.GetSize());
    }
    
    return result;
}

// Функция для сравнения двух строк
bool StringsEqual(const DynamicArray<char>& str1, const DynamicArray<char>& str2) 
{
    if (str1.GetSize() != str2.GetSize()) 
    {
        return false;
    }
    
    for (int i = 0; i < str1.GetSize(); i++) 
    {
        if (str1.Get(i) != str2.Get(i)) 
        {
            return false;
        }
    }
    return true;
}

// Специализация для double
template <typename T>
DynamicArray<char> NumberToString<double>(double value) 
{
    DynamicArray<char> result;
    
    int intPart = (int)value;
    DynamicArray<char> intStr = NumberToString(intPart);
    
    for (int i = 0; i < intStr.GetSize(); i++) 
    {
        result.InsertAt(intStr.Get(i), result.GetSize());
    }
    
    result.InsertAt('.', result.GetSize());
    
    double fractional = value - intPart;
    if (fractional < 0) fractional = -fractional;
    
    for (int i = 0; i < 2; i++) 
    {
        fractional *= 10;
        int digit = (int)fractional % 10;
        result.InsertAt('0' + digit, result.GetSize());
    }
    
    return result;
}

template<typename T>
struct Range 
{
    T min;
    T max;
    DynamicArray<char> label;
    
    Range() : min(T()), max(T()), label(DynamicArray<char>()) {}
    
    Range(T minVal, T maxVal, const DynamicArray<char>& rangeLabel = DynamicArray<char>()) : min(minVal), max(maxVal), label(rangeLabel) {}
    
    bool Contains(T value) const 
    {
        return value >= min && value <= max;
    }
    
    DynamicArray<char> ToString() const 
    {
        if (label.GetSize() == 0) 
        {
            DynamicArray<char> result;
            
            result.InsertAt('[', 0);
            
            DynamicArray<char> minStr = NumberToString(min);
            for (int i = 0; i < minStr.GetSize(); i++) 
            {
                result.InsertAt(minStr.Get(i), result.GetSize());
            }
            
            result.InsertAt(' ', result.GetSize());
            result.InsertAt('-', result.GetSize());
            result.InsertAt(' ', result.GetSize());
            
            DynamicArray<char> maxStr = NumberToString(max);
            for (int i = 0; i < maxStr.GetSize(); i++) 
            {
                result.InsertAt(maxStr.Get(i), result.GetSize());
            }
            
            result.InsertAt(']', result.GetSize());
            
            return result;
        }
        return label;
    }
};

template<typename T>
class Histogram 
{
private:
    Dictionary<DynamicArray<char>, int> counts;
    DynamicArray<Range<T>> ranges;

    int GetMaxIntValue() const 
    {
        return 2147483647;
    }

public:
    Histogram() : ranges(DynamicArray<Range<T>>()) {}
    
    void CreateUniformRanges(T minVal, T maxVal, int numRanges) 
    {
        ranges = DynamicArray<Range<T>>();
        T rangeSize = (maxVal - minVal) / numRanges;
        
        for (int i = 0; i < numRanges; i++) 
        {
            T rangeMin = minVal + i * rangeSize;
            T rangeMax;
            if (i == numRanges - 1) 
            {
                rangeMax = maxVal;
            } 
            else 
            {
                rangeMax = rangeMin + rangeSize;
            }
            ranges.InsertAt(Range<T>(rangeMin, rangeMax), ranges.GetSize());
        }
    }
    
    void SetCustomRanges(const DynamicArray<Range<T>>& customRanges) 
    {
        ranges = customRanges;
    }
    
    void BuildHistogram(const Sequence<T>* sequence) 
    {
        counts.Clear();
        
        for (int i = 0; i < sequence->GetLength(); i++) 
        {
            T value = sequence->Get(i);
            DynamicArray<char> rangeKey = FindRange(value);
            if (rangeKey.GetSize() > 0) 
            {
                if (counts.ContainsKey(rangeKey)) 
                {
                    int current = counts.Get(rangeKey);
                    counts.Remove(rangeKey);
                    counts.Add(rangeKey, current + 1);
                } 
                else 
                {
                    counts.Add(rangeKey, 1);
                }
            }
        }
    }
    
    void BuildHistogramMapReduce(const Sequence<T>* sequence) 
    {
        counts.Clear();
        
        Dictionary<DynamicArray<char>, int> mapResults;
        for (int i = 0; i < sequence->GetLength(); i++) 
        {
            T value = sequence->Get(i);
            DynamicArray<char> rangeKey = FindRange(value);
            if (rangeKey.GetSize() > 0) 
            {
                if (mapResults.ContainsKey(rangeKey)) 
                {
                    int current = mapResults.Get(rangeKey);
                    mapResults.Remove(rangeKey);
                    mapResults.Add(rangeKey, current + 1);
                } 
                else 
                {
                    mapResults.Add(rangeKey, 1);
                }
            }
        }
        
        counts = mapResults;
    }
    
    const Dictionary<DynamicArray<char>, int>& GetCounts() const 
    {
        return counts;
    }
    
    void PrintHistogram() const 
    {
        std::cout << "Гистограмма:\n";
        
        auto keys = counts.GetKeys();
        for (int i = 0; i < keys->GetLength(); i++) 
        {
            DynamicArray<char> rangeKey = keys->Get(i);
            int count = counts.Get(rangeKey);
            
            std::cout << "Диапазон: ";
            for (int j = 0; j < rangeKey.GetSize(); j++) 
            {
                std::cout << rangeKey.Get(j);
            }
            std::cout << ": " << count << " элементов\n";
        }
        delete keys;
    }
    
    DynamicArray<char> FindRange(T value) const 
    {
        for (int i = 0; i < ranges.GetSize(); i++) 
        {
            Range<T> range = ranges.Get(i);
            if (range.Contains(value)) 
            {
                return range.ToString();
            }
        }
        return DynamicArray<char>();
    }
    
    const DynamicArray<Range<T>>& GetRanges() const 
    { 
        return ranges; 
    }
    
    int GetTotalCount() const 
    {
        int total = 0;
        
        auto keys = counts.GetKeys();
        for (int i = 0; i < keys->GetLength(); i++) 
        {
            DynamicArray<char> key = keys->Get(i);
            total += counts.Get(key);
        }
        delete keys;
        
        return total;
    }
    
    DynamicArray<char> GetMostFrequentRange() const 
    {
        if (counts.GetCount() == 0) return DynamicArray<char>();
        
        DynamicArray<char> mostFrequent;
        int maxCount = 0;
        
        auto keys = counts.GetKeys();
        for (int i = 0; i < keys->GetLength(); i++) 
        {
            DynamicArray<char> key = keys->Get(i);
            int count = counts.Get(key);
            if (count > maxCount) {
                maxCount = count;
                mostFrequent = key;
            }
        }
        delete keys;
        
        return mostFrequent;
    }

    DynamicArray<char> GetLeastFrequentRange() const 
    {
        if (counts.GetCount() == 0) return DynamicArray<char>();
        
        DynamicArray<char> leastFrequent;
        int minCount = GetMaxIntValue();
        
        auto keys = counts.GetKeys();
        for (int i = 0; i < keys->GetLength(); i++) {
            DynamicArray<char> key = keys->Get(i);
            int count = counts.Get(key);
            if (count < minCount) {
                minCount = count;
                leastFrequent = key;
            }
        }
        delete keys;
        
        return leastFrequent;
    }
    
    void Clear() 
    {
        counts.Clear();
        ranges = DynamicArray<Range<T>>();
    }
};

#endif