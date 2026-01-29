#ifndef SORTED_SEQUENCE_H
#define SORTED_SEQUENCE_H

#include "binary_tree.h"
#include "Sequence.h"
#include <functional>

template <typename T>
class SortedSequence : public Sequence<T> 
{
private:
    BinaryTree<T> tree;
    mutable MutableArraySequence<T> cachedSequence;
    mutable bool isCacheValid;

    void rebuildCache() const 
    {
        if (!isCacheValid) 
        {
            cachedSequence = MutableArraySequence<T>();
            tree.traverseInOrder([this](const T& value) 
            {
                cachedSequence.Append(value);
            });
            isCacheValid = true;
        }
    }
    void ensureCacheValid() const
    {
        if(!isCacheValid)
        {
            rebuildCache();
        }
    }

public:
    SortedSequence() : isCacheValid(false) {}

    T GetFirst() const override 
    {
        ensureCacheValid();
        if (cachedSequence.GetLength() == 0) throw std::out_of_range("Sequence is empty");
        return cachedSequence.Get(0);
    }

    T GetLast() const override 
    {
        ensureCacheValid();
        int length = cachedSequence.GetLength();
        if (length == 0) throw std::out_of_range("Sequence is empty");
        return cachedSequence.Get(length - 1);
    }

    T Get(int index) const override 
    {
        ensureCacheValid();
        return cachedSequence.Get(index);
    }

    T Get(size_t index) const override
    {
        ensureCacheValid();
        return Get(static_cast<int>(index));
    }

    int GetLength() const override 
    {
        ensureCacheValid();        
        return cachedSequence.GetLength();
    }

    size_t GetCount() const override 
    {
        ensureCacheValid();        
        return static_cast<size_t>(GetLength());
    }

    void Add(const T& element) 
    {      
        tree.insert(element);
        isCacheValid = false;
    }

    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override 
    {
        ensureCacheValid();  
        return cachedSequence.GetSubsequence(startIndex, endIndex);
    }

    int IndexOf(const T& element) const 
    {
        ensureCacheValid();  
        for (int i = 0; i < cachedSequence.GetLength(); i++) 
        {
            if (cachedSequence.Get(i) == element) 
            {
                return i;
            }
        }
        return -1;
    }

    bool Contains(const T& element) const 
    {
        return tree.contains(element);
    }

    void Remove(const T& element) 
    {
        tree.remove(element);
        isCacheValid = false;
    }

    void Clear() 
    {
        tree.clear();
        isCacheValid = false;
    }
};

#endif