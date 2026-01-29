#ifndef TASK_PRIORITIZER_H
#define TASK_PRIORITIZER_H

#include "../structures/dictionary.h"
#include "../structures/Sequence.h"
#include "../structures/priority_queue.h"
#include "../structures/DynamicArray.h"

DynamicArray<char> StringToDynamicArray(const char* str) 
{
    int length = 0;
    while (str[length] != '\0') length++;
    
    DynamicArray<char> result(length);
    for (int i = 0; i < length; i++)
    {
        result.Set(i, str[i]);
    }
    return result;
}

struct Incident 
{
    DynamicArray<DynamicArray<char>> keywords;
    int priority;
    
    Incident() : priority(1), keywords(DynamicArray<DynamicArray<char>>()) {}
    
    Incident(const DynamicArray<DynamicArray<char>>& keys, int prio = 1) : priority(prio), keywords(keys) {}
    
    Incident(const DynamicArray<char>& singleKeyword, int prio = 1) : priority(prio) 
    {
        keywords = DynamicArray<DynamicArray<char>>(1);
        keywords.Set(0, singleKeyword);
    }
    
    Incident(const char* singleKeyword, int prio = 1) : priority(prio) 
    {
        keywords = DynamicArray<DynamicArray<char>>(1);
        keywords.Set(0, StringToDynamicArray(singleKeyword));
    }
};

struct Task {
    DynamicArray<char> name;
    DynamicArray<DynamicArray<char>> keywords;
    int calculatedPriority;
    
    Task() : calculatedPriority(0), name(DynamicArray<char>()), keywords(DynamicArray<DynamicArray<char>>()) {}
    
    Task(const DynamicArray<char>& taskName, const DynamicArray<DynamicArray<char>>& keys) : name(taskName), calculatedPriority(0), keywords(keys) {}
    
    Task(const char* taskName, const DynamicArray<DynamicArray<char>>& keys): calculatedPriority(0), keywords(keys) 
    {
        name = StringToDynamicArray(taskName);
    }
    
    Task(const char* taskName, const char* singleKeyword) : calculatedPriority(0) 
    {
        name = StringToDynamicArray(taskName);
        keywords = DynamicArray<DynamicArray<char>>(1);
        keywords.Set(0, StringToDynamicArray(singleKeyword));
    }
    
    bool operator<(const Task& other) const 
    {
        return calculatedPriority < other.calculatedPriority;
    }
    
    bool operator>(const Task& other) const 
    {
        return calculatedPriority > other.calculatedPriority;
    }
    
    bool operator==(const Task& other) const 
    {
        if (name.GetSize() != other.name.GetSize()) return false;
        for (int i = 0; i < name.GetSize(); i++) 
        {
            if (name.Get(i) != other.name.Get(i)) return false;
        }
        return calculatedPriority == other.calculatedPriority;
    }
};

class TaskPrioritizer 
{
private:
    Dictionary<DynamicArray<char>, int> keywordPriorities;

    int my_min(int a, int b) 
    {
        if (a < b) 
        {
            return a;
        } 
        else 
        {
            return b;
        }
    }
    
    bool stringsEqual(const DynamicArray<char>& str1, const DynamicArray<char>& str2) const 
    {
        if (str1.GetSize() != str2.GetSize()) return false;
        for (int i = 0; i < str1.GetSize(); i++) 
        {
            if (str1.Get(i) != str2.Get(i)) return false;
        }
        return true;
    }

public:
    TaskPrioritizer() = default;
    
    void CalculateKeywordPriorities(const Sequence<Incident>* incidents) 
    {
        keywordPriorities.Clear();
        
        for (int i = 0; i < incidents->GetLength(); i++) 
        {
            const Incident& incident = incidents->Get(i);
            
            for (int j = 0; j < incident.keywords.GetSize(); j++) 
            {
                DynamicArray<char> keyword = incident.keywords.Get(j);
                
                if (keywordPriorities.ContainsKey(keyword)) 
                {
                    int current = keywordPriorities.Get(keyword);
                    keywordPriorities.Remove(keyword);
                    keywordPriorities.Add(keyword, current + incident.priority);
                } 
                else 
                {
                    keywordPriorities.Add(keyword, incident.priority);
                }
            }
        }
    }
    
    void PrioritizeTasks(Sequence<Task>* tasks) 
    {
        for (int i = 0; i < tasks->GetLength(); i++) 
        {
            Task task = tasks->Get(i);
            task.calculatedPriority = CalculateTaskPriority(task);
            
            if (auto mutableSeq = dynamic_cast<MutableArraySequence<Task>*>(tasks)) 
            {
                mutableSeq->Set(i, task);
            } else if (auto mutableListSeq = dynamic_cast<MutableListSequence<Task>*>(tasks)) 
            {
                mutableListSeq->Set(i, task);
            }
        }
        
        SortTasksByPriority(tasks);
    }
    
    PriorityQueue<Task> PrioritizeTasksWithQueue(const Sequence<Task>* tasks) 
    {
        auto comparator = [](const Task& a, const Task& b) 
        {
            return a.calculatedPriority < b.calculatedPriority;
        };
        
        PriorityQueue<Task> resultQueue(comparator);
        
        for (int i = 0; i < tasks->GetLength(); i++) 
        {
            Task task = tasks->Get(i);
            task.calculatedPriority = CalculateTaskPriority(task);
            resultQueue.Enqueue(task);
        }
        
        return resultQueue;
    }
    
    int CalculateTaskPriority(const Task& task) const 
    {
        int priority = 0;
        for (int i = 0; i < task.keywords.GetSize(); i++) 
        {
            DynamicArray<char> keyword = task.keywords.Get(i);
            if (keywordPriorities.ContainsKey(keyword)) 
            {
                priority += keywordPriorities.Get(keyword);
            }
        }
        return priority;
    }

    void SortTasksByPriority(Sequence<Task>* tasks) 
    {
        // Простая пузырьковая сортировка для демонстрации
        if (auto mutableSeq = dynamic_cast<MutableArraySequence<Task>*>(tasks)) 
        {
            int n = mutableSeq->GetLength();
            for (int i = 0; i < n - 1; i++) 
            {
                for (int j = 0; j < n - i - 1; j++) 
                {
                    Task task1 = mutableSeq->Get(j);
                    Task task2 = mutableSeq->Get(j + 1);
                    if (task1.calculatedPriority < task2.calculatedPriority) 
                    {
                        mutableSeq->Set(j, task2);
                        mutableSeq->Set(j + 1, task1);
                    }
                }
            }
        }
    }
    
    const Dictionary<DynamicArray<char>, int>& GetKeywordPriorities() const 
    {
        return keywordPriorities;
    }
    
    MutableArraySequence<DynamicArray<char>> GetTopKeywords(int count) const 
    {
        MutableArraySequence<DynamicArray<char>> result;
        
        auto allPairs = new MutableArraySequence<DictionaryPair<DynamicArray<char>, int>>();

        keywordPriorities.ForEach([allPairs](const DynamicArray<char>& key, int value) 
        {
            allPairs->Append(DictionaryPair<DynamicArray<char>, int>(key, value));
        });
        
        for (int i = 0; i < allPairs->GetLength() - 1; i++) 
        {
            for (int j = 0; j < allPairs->GetLength() - i - 1; j++) 
            {
                auto pair1 = allPairs->Get(j);
                auto pair2 = allPairs->Get(j + 1);
                if (pair1.getValue() < pair2.getValue()) 
                {
                    DictionaryPair<DynamicArray<char>, int> temp = pair1;
                    allPairs->Set(j, pair2);
                    allPairs->Set(j + 1, temp);
                }
            }
        }
        
        int n = count;
        if (n > allPairs->GetLength()) 
        {
            n = allPairs->GetLength();
        }
        
        for (int i = 0; i < n; i++) 
        {
            result.Append(allPairs->Get(i).getKey());
        }
        
        delete allPairs;
        return result;
    }
    
    void PrintKeywordPriorities() const 
    {
        std::cout << "Приоритеты ключевых слов:\n";
        
        keywordPriorities.ForEach([](const DynamicArray<char>& key, int value) 
        {
            std::cout << "Ключ: ";
            for (int i = 0; i < key.GetSize(); i++) 
            {
                std::cout << key.Get(i);
            }
            std::cout << ": " << value << "\n";
        });
    }
    
    void PrintPrioritizedTasks(const Sequence<Task>* tasks) const 
    {
        std::cout << "Приоритезированные задачи:\n";
        for (int i = 0; i < tasks->GetLength(); i++) 
        {
            const Task& task = tasks->Get(i);
            std::cout << (i + 1) << ". ";
            for (int j = 0; j < task.name.GetSize(); j++) 
            {
                std::cout << task.name.Get(j);
            }
            std::cout << " (приоритет: " << task.calculatedPriority << ")\n";
        }
    }
    
    void Clear() 
    {
        keywordPriorities.Clear();
    }
    
    int GetKeywordCount() const 
    {
        return keywordPriorities.GetCount();
    }
    
    int GetTotalIncidentWeight() const 
    {
        int total = 0;
        
        keywordPriorities.ForEachValue([&total](int value) 
        {
            total += value;
        });
        
        return total;
    }

    static DynamicArray<DynamicArray<char>> ParseKeywords(const char* keywordsStr) 
    {
        DynamicArray<DynamicArray<char>> result;
        
        int start = 0;
        int length = 0;
        while (keywordsStr[length] != '\0') length++;
        
        int currentStart = 0;
        for (int i = 0; i <= length; i++) 
        {
            if (keywordsStr[i] == ',' || keywordsStr[i] == '\0') 
            {
                int wordLength = i - currentStart;
                if (wordLength > 0) 
                {
                    while (currentStart < i && (keywordsStr[currentStart] == ' ' || keywordsStr[currentStart] == '\t')) 
                    {
                        currentStart++;
                        wordLength--;
                    }
                    while (i > currentStart && (keywordsStr[i-1] == ' ' || keywordsStr[i-1] == '\t')) 
                    {
                        i--;
                        wordLength--;
                    }
                    
                    if (wordLength > 0) 
                    {
                        DynamicArray<char> keyword(wordLength);
                        for (int j = 0; j < wordLength; j++) 
                        {
                            keyword.Set(j, keywordsStr[currentStart + j]);
                        }
                        result.InsertAt(keyword, result.GetSize());
                    }
                }
                currentStart = i + 1;
            }
        }
        
        return result;
    }
    
    static void PrintDynamicArray(const DynamicArray<char>& arr) 
    {
        for (int i = 0; i < arr.GetSize(); i++) 
        {
            std::cout << arr.Get(i);
        }
    }
};

#endif