#ifndef MOST_FREQUENT_SUBSTRINGS_H
#define MOST_FREQUENT_SUBSTRINGS_H

#include "../structures/binary_tree.h"
#include "../structures/Sequence.h"
#include "../structures/DynamicArray.h"

class MostFrequentSubstrings 
{
private:
    struct SubstringCount 
    {
        DynamicArray<char> substring;
        int count;
        
        SubstringCount() : substring(DynamicArray<char>()), count(0) {}
        
        SubstringCount(const DynamicArray<char>& sub, int cnt = 1) : substring(sub), count(cnt) {}
        
        bool operator<(const SubstringCount& other) const 
        {
            if (substring.GetSize() != other.substring.GetSize())
                return substring.GetSize() < other.substring.GetSize();
            
            int minSize;
            if (substring.GetSize() < other.substring.GetSize()) 
            {
                minSize = substring.GetSize();
            }
            else 
            {
                minSize = other.substring.GetSize();
            }
            for (int i = 0; i < minSize; i++) 
            {
                if (substring[i] != other.substring[i]) return substring[i] < other.substring[i];
            }
            return substring.GetSize() < other.substring.GetSize();
        }
        
        bool operator==(const SubstringCount& other) const 
        {
            if (substring.GetSize() != other.substring.GetSize())
                return false;
            
            for (int i = 0; i < substring.GetSize(); i++) 
            {
                if (substring[i] != other.substring[i]) return false;
            }
            return true;
        }
    };
    
    BinaryTree<SubstringCount> substringTree;
    int totalSubstrings;
    
public:
    MostFrequentSubstrings() : totalSubstrings(0) {}
    
    void FindMostFrequent(const Sequence<char>* sequence, int minLength, int maxLength) 
    {
        substringTree.clear();
        totalSubstrings = 0;
        
        int seqLength = sequence->GetLength();
    
        for (int start = 0; start < seqLength; start++) 
        {
            for (int length = minLength; length <= maxLength && start + length <= seqLength; length++) 
            {
                DynamicArray<char> substring(length);
                for (int i = 0; i < length; i++) 
                {
                    substring.Set(i, sequence->Get(start + i));
                }
                
                SubstringCount newPair(substring, 1);
            
                if (substringTree.contains(newPair)) 
                {
                    auto node = substringTree.findNode(newPair);
                    if (node) 
                    {
                        SubstringCount updated = node->data;
                        updated.count++;
                        substringTree.remove(node->data);
                        substringTree.insert(updated);
                    }
                } 
                else 
                {
                    substringTree.insert(newPair);
                }
                
                totalSubstrings++;
            }
        }
        
        substringTree.balance();
    }
    
    Sequence<SubstringCount>* GetTopFrequent(int topCount) const 
    {
        auto result = new MutableArraySequence<SubstringCount>();
        
        if (substringTree.isEmpty() || topCount <= 0) 
        {
            return result;
        }
        
        auto allSubstrings = new MutableArraySequence<SubstringCount>();
        substringTree.traverseInOrder([allSubstrings](const SubstringCount& sc) 
        {
            allSubstrings->Append(sc);
        });
        
        int n = allSubstrings->GetLength();
        for (int i = 0; i < n - 1; i++) {
            for (int j = 0; j < n - i - 1; j++) 
            {
                if (allSubstrings->Get(j).count < allSubstrings->Get(j + 1).count) 
                {
                    SubstringCount temp = allSubstrings->Get(j);
                    allSubstrings->Set(j, allSubstrings->Get(j + 1));
                    allSubstrings->Set(j + 1, temp);
                }
            }
        }
        
        for (int i = 0; i < topCount && i < n; i++) 
        {
            result->Append(allSubstrings->Get(i));
        }
        
        delete allSubstrings;
        return result;
    }
    
    void PrintUsingIterator(int maxToShow = 10) const 
    {
        if (substringTree.isEmpty()) {
            std::cout << "Дерево подстрок пусто.\n";
            return;
        }
        
        std::cout << "Вывод через итератор дерева (первые " << maxToShow << " записей):\n";
        
        IIterator<SubstringCount>* it = substringTree.CreateIterator();
        int count = 0;
        
        while (it->HasNext() && count < maxToShow) 
        {
            SubstringCount sc = it->GetCurrentItem();
            
            std::cout << "Подстрока: \"";
            for (int i = 0; i < sc.substring.GetSize(); i++) 
            {
                std::cout << sc.substring[i];
            }
            std::cout << "\" | Длина: " << sc.substring.GetSize() << " | Частота: " << sc.count << " (" << (sc.count * 100.0 / totalSubstrings) << "%)\n";
            
            it->Next();
            count++;
        }
        
        delete it;
        
        if (count == 0) 
        {
            std::cout << "Нет данных для отображения.\n";
        }
    }
    
    Sequence<SubstringCount>* GetAllViaIterator() const 
    {
        auto result = new MutableArraySequence<SubstringCount>();
        
        IIterator<SubstringCount>* it = substringTree.CreateIterator();
        while (it->HasNext()) 
        {
            result->Append(it->GetCurrentItem());
            it->Next();
        }
        
        delete it;
        return result;
    }
    
    void PrintStatistics() const 
    {
        std::cout << "=== Статистика поиска подстрок ===\n";
        std::cout << "Всего уникальных подстрок: " << GetUniqueCount() << "\n";
        std::cout << "Всего подстрок (с учетом повторов): " << totalSubstrings << "\n";
        
        if (!substringTree.isEmpty()) 
        {
            auto top = GetTopFrequent(1);
            if (top->GetLength() > 0) 
            {
                SubstringCount mostFreq = top->Get(0);
                std::cout << "Самая частая подстрока: \"";
                for (int i = 0; i < mostFreq.substring.GetSize(); i++) 
                {
                    std::cout << mostFreq.substring[i];
                }
                std::cout << "\" (встречается " << mostFreq.count << " раз)\n";
            }
            delete top;
        }
    }
    
    int GetUniqueCount() const 
    {
        int count = 0;
        substringTree.traverseInOrder([&count](const SubstringCount&) 
        {
            count++;
        });
        return count;
    }

    int GetTotalCount() const 
    {
        return totalSubstrings;
    }
    
    void Clear() 
    {
        substringTree.clear();
        totalSubstrings = 0;
    }
    
    DynamicArray<char> FromCString(const char* str) 
    {
        int length = 0;
        while (str[length] != '\0') length++;
        
        DynamicArray<char> result(length);
        for (int i = 0; i < length; i++) {
            result.Set(i, str[i]);
        }
        return result;
    }
    
    void PrintString(const DynamicArray<char>& arr) 
    {
        for (int i = 0; i < arr.GetSize(); i++) {
            std::cout << arr[i];
        }
    }
};

#endif