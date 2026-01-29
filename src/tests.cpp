#include <gtest/gtest.h>
#include "core/structures/DynamicArray.h"
#include "core/structures/LinkedList.h"
#include "core/structures/Sequence.h"
#include "core/structures/binary_tree.h"
#include "core/structures/dictionary.h"
#include "core/structures/set.h"
#include "core/structures/sorted_sequence.h"
#include "core/structures/priority_queue.h"
#include "core/tasks/histogram.h"
#include "core/tasks/task_prioritizer.h"
#include "core/tasks/most_frequent_substrings.h"
#include <functional>
#include <vector>
#include <algorithm>
#include <string>
#include <map>
#include <unordered_set>
extern "C" 
{
    void _Exit(int status) { exit(status); }
    void quick_exit(int status) { exit(status); }
}

// 1. Тесты DynamicArray

TEST(DynamicArrayTest, EmptyArray) 
{
    DynamicArray<int> arr;
    EXPECT_EQ(arr.GetSize(), 0);
}

TEST(DynamicArrayTest, SizeConstructor) 
{
    DynamicArray<int> arr(5);
    EXPECT_EQ(arr.GetSize(), 5);
}

TEST(DynamicArrayTest, ArrayConstructor) 
{
    int data[] = {1, 2, 3, 4, 5};
    DynamicArray<int> arr(data, 5);
    EXPECT_EQ(arr.GetSize(), 5);
    EXPECT_EQ(arr.Get(0), 1);
    EXPECT_EQ(arr.Get(4), 5);
}

TEST(DynamicArrayTest, CopyConstructorDeepCopy) 
{
    // Проверяет: Глубокое копирование через конструктор копирования
    DynamicArray<int> arr1(3);
    arr1.Set(0, 10);
    arr1.Set(1, 20);
    arr1.Set(2, 30);
    
    DynamicArray<int> arr2(arr1);
    EXPECT_EQ(arr2.GetSize(), 3);
    EXPECT_EQ(arr2.Get(0), 10);
    EXPECT_EQ(arr2.Get(1), 20);
    EXPECT_EQ(arr2.Get(2), 30);
    
    // Изменение оригинала не должно влиять на копию
    arr1.Set(0, 100);
    EXPECT_EQ(arr2.Get(0), 10);
}

TEST(DynamicArrayTest, MoveConstructor) 
{
    // Проверяет: Перемещающий конструктор
    DynamicArray<int> arr1(3);
    arr1.Set(0, 100);
    arr1.Set(1, 200);
    arr1.Set(2, 300);
    
    DynamicArray<int> arr2(std::move(arr1));
    EXPECT_EQ(arr2.GetSize(), 3);
    EXPECT_EQ(arr1.GetSize(), 0);
    EXPECT_EQ(arr2.Get(0), 100);
    EXPECT_EQ(arr2.Get(2), 300);
}

TEST(DynamicArrayTest, AssignmentOperatorDeepCopy) 
{
    // Проверяет: Оператор присваивания с глубоким копированием
    DynamicArray<int> arr1(2);
    arr1.Set(0, 5);
    arr1.Set(1, 15);
    
    DynamicArray<int> arr2;
    arr2 = arr1;
    EXPECT_EQ(arr2.GetSize(), 2);
    EXPECT_EQ(arr2.Get(1), 15);
    
    // Само-присваивание
    arr2 = arr2;
    EXPECT_EQ(arr2.GetSize(), 2);
}

TEST(DynamicArrayTest, MoveAssignmentOperator) 
{
    // Проверяет: Перемещающий оператор присваивания
    DynamicArray<int> arr1(2);
    arr1.Set(0, 25);
    arr1.Set(1, 35);
    
    DynamicArray<int> arr2;
    arr2 = std::move(arr1);
    EXPECT_EQ(arr2.GetSize(), 2);
    EXPECT_EQ(arr2.Get(0), 25);
    EXPECT_EQ(arr1.GetSize(), 0);
}

TEST(DynamicArrayTest, IndexOperatorReadWrite) 
{
    // Проверяет: Оператор [] для чтения и записи
    DynamicArray<int> arr(3);
    arr[0] = 1;
    arr[1] = 2;
    arr[2] = 3;
    
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 2);
    EXPECT_EQ(arr[2], 3);
    
    arr[1] = 20;
    EXPECT_EQ(arr[1], 20);
    
    const DynamicArray<int>& constArr = arr;
    EXPECT_EQ(constArr[0], 1);
}

TEST(DynamicArrayTest, ResizeOperationGrow) 
{
    // Проверяет: Увеличение размера массива
    DynamicArray<int> arr(3);
    arr.Set(0, 1);
    arr.Set(1, 2);
    arr.Set(2, 3);
    
    arr.Resize(5);
    EXPECT_EQ(arr.GetSize(), 5);
    EXPECT_EQ(arr.Get(0), 1);
    EXPECT_EQ(arr.Get(1), 2);
    EXPECT_EQ(arr.Get(2), 3);
}

TEST(DynamicArrayTest, ResizeOperationShrink) 
{
    // Проверяет: Уменьшение размера массива
    DynamicArray<int> arr(5);
    for (int i = 0; i < 5; i++) {
        arr.Set(i, i + 1);
    }
    
    arr.Resize(2);
    EXPECT_EQ(arr.GetSize(), 2);
    EXPECT_EQ(arr.Get(0), 1);
    EXPECT_EQ(arr.Get(1), 2);
}

TEST(DynamicArrayTest, InsertAtBeginning) 
{
    // Проверяет: Вставку элемента в начало массива
    DynamicArray<int> arr(3);
    arr.Set(0, 2);
    arr.Set(1, 3);
    arr.Set(2, 4);
    
    arr.InsertAt(1, 0);
    EXPECT_EQ(arr.GetSize(), 4);
    EXPECT_EQ(arr.Get(0), 1);
    EXPECT_EQ(arr.Get(1), 2);
    EXPECT_EQ(arr.Get(2), 3);
    EXPECT_EQ(arr.Get(3), 4);
}

TEST(DynamicArrayTest, InsertAtEnd) 
{
    // Проверяет: Вставку элемента в конец массива
    DynamicArray<int> arr(3);
    arr.Set(0, 1);
    arr.Set(1, 2);
    arr.Set(2, 3);
    
    arr.InsertAt(4, 3); // Вставляем в конец
    EXPECT_EQ(arr.GetSize(), 4);
    EXPECT_EQ(arr.Get(3), 4);
}

TEST(DynamicArrayTest, InsertAtMiddle) 
{
    // Проверяет: Вставку элемента в середину массива
    DynamicArray<int> arr(4);
    arr.Set(0, 1);
    arr.Set(1, 2);
    arr.Set(2, 4);
    arr.Set(3, 5);
    
    arr.InsertAt(3, 2);
    EXPECT_EQ(arr.GetSize(), 5);
    EXPECT_EQ(arr.Get(0), 1);
    EXPECT_EQ(arr.Get(1), 2);
    EXPECT_EQ(arr.Get(2), 3);
    EXPECT_EQ(arr.Get(3), 4);
    EXPECT_EQ(arr.Get(4), 5);
}

TEST(DynamicArrayTest, NegativeSizeThrows) 
{
    // Проверяет: Обработку отрицательного размера массива
    EXPECT_THROW(DynamicArray<int> arr(-1), std::invalid_argument);
}

TEST(DynamicArrayTest, OutOfRangeAccessThrows) 
{
    // Проверяет: Обработку выхода за границы массива
    DynamicArray<int> arr(3);
    EXPECT_THROW(arr.Get(5), std::out_of_range);
    EXPECT_THROW(arr.Set(5, 10), std::out_of_range);
    EXPECT_THROW(arr[5] = 10, std::out_of_range);
    
    const DynamicArray<int>& constArr = arr;
    EXPECT_THROW(constArr.Get(5), std::out_of_range);
}

TEST(DynamicArrayTest, StringArrayOperations) 
{
    // Проверяет: Работу DynamicArray с строковыми типами
    DynamicArray<std::string> arr(2);
    arr.Set(0, "Hello");
    arr.Set(1, "World");
    
    EXPECT_EQ(arr.Get(0), "Hello");
    EXPECT_EQ(arr.Get(1), "World");
    
    arr.InsertAt("Middle", 1);
    EXPECT_EQ(arr.GetSize(), 3);
    EXPECT_EQ(arr.Get(1), "Middle");
}

TEST(DynamicArrayTest, LargeArrayPerformance) 
{
    // Проверяет: Производительность работы с большим массивом
    const int SIZE = 10000;
    DynamicArray<int> arr(SIZE);
    
    for (int i = 0; i < SIZE; i++) {
        arr.Set(i, i * 2);
    }
    
    EXPECT_EQ(arr.GetSize(), SIZE);
    
    // Проверяем случайные доступы
    EXPECT_EQ(arr.Get(0), 0);
    EXPECT_EQ(arr.Get(SIZE/2), SIZE);
    EXPECT_EQ(arr.Get(SIZE-1), (SIZE-1)*2);
}

// 2. Тесты LinkedList

TEST(LinkedListTest, EmptyList) 
{
    // Проверяет: Создание пустого списка
    LinkedList<int> list;
    EXPECT_EQ(list.GetLength(), 0);
    EXPECT_THROW(list.GetFirst(), std::runtime_error);
    EXPECT_THROW(list.GetLast(), std::runtime_error);
    EXPECT_THROW(list.Get(0), std::out_of_range);
}

TEST(LinkedListTest, AppendOperationMultiple) 
{
    // Проверяет: Добавление нескольких элементов в конец списка
    LinkedList<int> list;
    list.Append(1);
    list.Append(2);
    list.Append(3);
    EXPECT_EQ(list.GetLength(), 3);
    EXPECT_EQ(list.GetFirst(), 1);
    EXPECT_EQ(list.GetLast(), 3);
    EXPECT_EQ(list.Get(1), 2);
}

TEST(LinkedListTest, PrependOperationMultiple) 
{
    // Проверяет: Добавление нескольких элементов в начало списка
    LinkedList<int> list;
    list.Prepend(3);
    list.Prepend(2);
    list.Prepend(1);
    EXPECT_EQ(list.GetLength(), 3);
    EXPECT_EQ(list.GetFirst(), 1);
    EXPECT_EQ(list.GetLast(), 3);
    EXPECT_EQ(list.Get(1), 2);
}

TEST(LinkedListTest, InsertAtBeginning) 
{
    // Проверяет: Вставку элемента в начало списка
    LinkedList<int> list;
    list.Append(2);
    list.Append(3);
    list.InsertAt(1, 0); 
    
    EXPECT_EQ(list.GetLength(), 3);
    EXPECT_EQ(list.GetFirst(), 1);
    EXPECT_EQ(list.Get(1), 2);
    EXPECT_EQ(list.Get(2), 3);
}

TEST(LinkedListTest, InsertAtEnd) 
{
    // Проверяет: Вставку элемента в конец списка
    LinkedList<int> list;
    list.Append(1);
    list.Append(2);
    list.InsertAt(3, 2);
    
    EXPECT_EQ(list.GetLength(), 3);
    EXPECT_EQ(list.GetFirst(), 1);
    EXPECT_EQ(list.GetLast(), 3);
}

TEST(LinkedListTest, InsertAtMiddle) 
{
    // Проверяет: Вставку элемента в середину списка
    LinkedList<int> list;
    list.Append(1);
    list.Append(3);
    list.Append(4);
    list.InsertAt(2, 1);
    
    EXPECT_EQ(list.GetLength(), 4);
    EXPECT_EQ(list.Get(0), 1);
    EXPECT_EQ(list.Get(1), 2);
    EXPECT_EQ(list.Get(2), 3);
    EXPECT_EQ(list.Get(3), 4);
}

TEST(LinkedListTest, GetSubListValidRange) 
{
    // Проверяет: Получение подсписка в допустимом диапазоне
    LinkedList<int> list;
    for (int i = 0; i < 10; i++) {
        list.Append(i * 10);
    }
    
    LinkedList<int> sublist = list.GetSubList(2, 5);
    EXPECT_EQ(sublist.GetLength(), 4);
    EXPECT_EQ(sublist.Get(0), 20);
    EXPECT_EQ(sublist.Get(1), 30);
    EXPECT_EQ(sublist.Get(2), 40);
    EXPECT_EQ(sublist.Get(3), 50);
}

TEST(LinkedListTest, GetSubListSingleElement) 
{
    // Проверяет: Получение подсписка из одного элемента
    LinkedList<int> list;
    list.Append(10);
    list.Append(20);
    list.Append(30);
    
    LinkedList<int> sublist = list.GetSubList(1, 1);
    EXPECT_EQ(sublist.GetLength(), 1);
    EXPECT_EQ(sublist.Get(0), 20);
}

TEST(LinkedListTest, GetSubListInvalidRangeThrows) 
{
    // Проверяет: Обработку неверного диапазона для подсписка
    LinkedList<int> list;
    list.Append(1);
    list.Append(2);
    
    EXPECT_THROW(list.GetSubList(-1, 2), std::out_of_range);
    EXPECT_THROW(list.GetSubList(0, 3), std::out_of_range);
    EXPECT_THROW(list.GetSubList(2, 1), std::out_of_range);
}

TEST(LinkedListTest, ConcatOperationWithEmpty) 
{
    // Проверяет: Конкатенацию списка с пустым списком
    LinkedList<int> list1;
    list1.Append(1);
    list1.Append(2);
    
    LinkedList<int> list2;
    
    LinkedList<int> result = list1.Concat(list2);
    EXPECT_EQ(result.GetLength(), 2);
    EXPECT_EQ(result.GetFirst(), 1);
    EXPECT_EQ(result.GetLast(), 2);
}

TEST(LinkedListTest, ConcatOperationTwoNonEmpty) 
{
    // Проверяет: Конкатенацию двух непустых списков
    LinkedList<int> list1;
    list1.Append(1);
    list1.Append(2);
    
    LinkedList<int> list2;
    list2.Append(3);
    list2.Append(4);
    
    LinkedList<int> result = list1.Concat(list2);
    EXPECT_EQ(result.GetLength(), 4);
    EXPECT_EQ(result.Get(0), 1);
    EXPECT_EQ(result.Get(1), 2);
    EXPECT_EQ(result.Get(2), 3);
    EXPECT_EQ(result.Get(3), 4);
}

TEST(LinkedListTest, CopyConstructorDeepCopy) 
{
    // Проверяет: Глубокое копирование через конструктор копирования
    LinkedList<int> list1;
    list1.Append(10);
    list1.Append(20);
    list1.Append(30);
    
    LinkedList<int> list2(list1);
    EXPECT_EQ(list2.GetLength(), 3);
    EXPECT_EQ(list2.GetFirst(), 10);
    EXPECT_EQ(list2.GetLast(), 30);
    
    list1.Append(40);
    EXPECT_EQ(list1.GetLength(), 4);
    EXPECT_EQ(list2.GetLength(), 3);
}

TEST(LinkedListTest, MoveConstructor) 
{
    // Проверяет: Перемещающий конструктор
    LinkedList<int> list1;
    list1.Append(100);
    list1.Append(200);
    list1.Append(300);
    
    LinkedList<int> list2(std::move(list1));
    EXPECT_EQ(list2.GetLength(), 3);
    EXPECT_EQ(list1.GetLength(), 0);
    EXPECT_EQ(list2.GetFirst(), 100);
    EXPECT_EQ(list2.GetLast(), 300);
}

TEST(LinkedListTest, AssignmentOperatorDeepCopy) 
{
    // Проверяет: Оператор присваивания с глубоким копированием
    LinkedList<int> list1;
    list1.Append(100);
    list1.Append(200);
    
    LinkedList<int> list2;
    list2 = list1;
    EXPECT_EQ(list2.GetLength(), 2);
    EXPECT_EQ(list2.GetLast(), 200);
    
    // Само-присваивание
    list2 = list2;
    EXPECT_EQ(list2.GetLength(), 2);
}

TEST(LinkedListTest, MoveAssignmentOperator) 
{
    // Проверяет: Перемещающий оператор присваивания
    LinkedList<int> list1;
    list1.Append(25);
    list1.Append(35);
    
    LinkedList<int> list2;
    list2 = std::move(list1);
    EXPECT_EQ(list2.GetLength(), 2);
    EXPECT_EQ(list2.Get(0), 25);
    EXPECT_EQ(list1.GetLength(), 0);
}

TEST(LinkedListTest, IndexOperatorReadWrite) 
{
    // Проверяет: Оператор [] для чтения и записи
    LinkedList<int> list;
    list.Append(5);
    list.Append(15);
    list.Append(25);
    
    EXPECT_EQ(list[0], 5);
    EXPECT_EQ(list[2], 25);
    
    list[1] = 10;
    EXPECT_EQ(list.Get(1), 10);
    
    const LinkedList<int>& constList = list;
    EXPECT_EQ(constList[0], 5);
}

TEST(LinkedListTest, LargeLinkedList) 
{
    // Проверяет: Работу с большим связным списком
    LinkedList<int> list;
    const int COUNT = 1000;
    
    for (int i = 0; i < COUNT; i++) {
        list.Append(i);
    }
    
    EXPECT_EQ(list.GetLength(), COUNT);
    EXPECT_EQ(list.GetFirst(), 0);
    EXPECT_EQ(list.GetLast(), COUNT - 1);
    
    // Проверяем случайные доступы
    EXPECT_EQ(list.Get(500), 500);
    EXPECT_EQ(list.Get(999), 999);
}

// 3. Тесты Sequence

TEST(SequenceTest, ImmutableArraySequenceBasicOperations) 
{
    // Проверяет: Базовые операции ImmutableArraySequence
    int data[] = {1, 2, 3, 4, 5};
    ImmutableArraySequence<int> seq(data, 5);
    EXPECT_EQ(seq.GetFirst(), 1);
    EXPECT_EQ(seq.GetLast(), 5);
    EXPECT_EQ(seq.Get(2), 3);
    EXPECT_EQ(seq.GetLength(), 5);
}

TEST(SequenceTest, ImmutableArraySequenceEmpty) 
{
    // Проверяет: Поведение пустой ImmutableArraySequence
    ImmutableArraySequence<int> seq;
    EXPECT_EQ(seq.GetLength(), 0);
    EXPECT_THROW(seq.GetFirst(), std::out_of_range);
    EXPECT_THROW(seq.GetLast(), std::out_of_range);
    EXPECT_THROW(seq.Get(0), std::out_of_range);
}

TEST(SequenceTest, ImmutableArraySequenceSubsequenceValid) 
{
    // Проверяет: Получение корректной подпоследовательности
    int data[] = {1, 2, 3, 4, 5, 6, 7};
    ImmutableArraySequence<int> seq(data, 7);
    
    auto subseq = seq.GetSubsequence(2, 5);
    EXPECT_EQ(subseq->GetLength(), 4);
    EXPECT_EQ(subseq->GetFirst(), 3);
    EXPECT_EQ(subseq->GetLast(), 6);
    EXPECT_EQ(subseq->Get(1), 4);
    delete subseq;
}

TEST(SequenceTest, ImmutableArraySequenceSubsequenceSingle) 
{
    // Проверяет: Получение подпоследовательности из одного элемента
    int data[] = {10, 20, 30};
    ImmutableArraySequence<int> seq(data, 3);
    
    auto subseq = seq.GetSubsequence(1, 1);
    EXPECT_EQ(subseq->GetLength(), 1);
    EXPECT_EQ(subseq->GetFirst(), 20);
    delete subseq;
}

TEST(SequenceTest, ImmutableArraySequenceSubsequenceInvalid) 
{
    // Проверяет: Обработку неверных индексов подпоследовательности
    int data[] = {1, 2, 3};
    ImmutableArraySequence<int> seq(data, 3);
    
    EXPECT_THROW(seq.GetSubsequence(-1, 2), std::out_of_range);
    EXPECT_THROW(seq.GetSubsequence(0, 3), std::out_of_range);
    EXPECT_THROW(seq.GetSubsequence(2, 1), std::out_of_range);
}

TEST(SequenceTest, MutableArraySequenceAppend) 
{
    // Проверяет: Добавление элементов в MutableArraySequence
    MutableArraySequence<int> seq;
    seq.Append(1);
    seq.Append(2);
    seq.Append(3);
    
    EXPECT_EQ(seq.GetLength(), 3);
    EXPECT_EQ(seq.GetLast(), 3);
    EXPECT_EQ(seq.Get(1), 2);
}

TEST(SequenceTest, MutableArraySequencePrepend) 
{
    // Проверяет: Добавление элементов в начало MutableArraySequence
    MutableArraySequence<int> seq;
    seq.Prepend(3);
    seq.Prepend(2);
    seq.Prepend(1);
    
    EXPECT_EQ(seq.GetLength(), 3);
    EXPECT_EQ(seq.GetFirst(), 1);
    EXPECT_EQ(seq.Get(1), 2);
    EXPECT_EQ(seq.GetLast(), 3);
}

TEST(SequenceTest, MutableArraySequenceInsertAt) 
{
    // Проверяет: Вставку элемента в MutableArraySequence
    MutableArraySequence<int> seq;
    seq.Append(1);
    seq.Append(3);
    seq.InsertAt(2, 1);
    
    EXPECT_EQ(seq.GetLength(), 3);
    EXPECT_EQ(seq.Get(0), 1);
    EXPECT_EQ(seq.Get(1), 2);
    EXPECT_EQ(seq.Get(2), 3);
}

TEST(SequenceTest, MutableArraySequenceSet) 
{
    // Проверяет: Установку значения в MutableArraySequence
    int data[] = {1, 2, 3};
    MutableArraySequence<int> seq(data, 3);
    
    seq.Set(1, 20);
    EXPECT_EQ(seq.Get(1), 20);
    
    EXPECT_THROW(seq.Set(5, 100), std::out_of_range);
}

TEST(SequenceTest, MutableArraySequenceGetRef) 
{
    // Проверяет: Получение ссылки на элемент в MutableArraySequence
    MutableArraySequence<int> seq;
    seq.Append(10);
    seq.Append(20);
    
    int& ref = seq.GetRef(1);
    ref = 30;
    EXPECT_EQ(seq.Get(1), 30);
}

TEST(SequenceTest, MutableArraySequenceIteratorFullTraversal) 
{
    // Проверяет: Полный обход через итератор MutableArraySequence
    int data[] = {10, 20, 30};
    MutableArraySequence<int> seq(data, 3);
    
    auto iterator = seq.CreateIterator();
    std::vector<int> collected;
    
    while (iterator->HasNext()) {
        collected.push_back(iterator->GetCurrentItem());
        iterator->Next();
    }
    
    EXPECT_EQ(collected.size(), 3);
    EXPECT_EQ(collected[0], 10);
    EXPECT_EQ(collected[1], 20);
    EXPECT_EQ(collected[2], 30);
    
    iterator->Reset();
    EXPECT_EQ(iterator->GetCurrentItem(), 10);
    
    delete iterator;
}

TEST(SequenceTest, ImmutableListSequenceBasic) 
{
    // Проверяет: Базовые операции ImmutableListSequence
    int data[] = {5, 10, 15, 20};
    ImmutableListSequence<int> seq(data, 4);
    EXPECT_EQ(seq.GetFirst(), 5);
    EXPECT_EQ(seq.GetLast(), 20);
    EXPECT_EQ(seq.GetLength(), 4);
    EXPECT_EQ(seq.Get(2), 15);
}

TEST(SequenceTest, MutableListSequenceModifications) 
{
    // Проверяет: Модификации MutableListSequence
    MutableListSequence<int> seq;
    seq.Append(100);
    seq.Append(200);
    seq.Prepend(50);
    
    EXPECT_EQ(seq.GetLength(), 3);
    EXPECT_EQ(seq.GetFirst(), 50);
    EXPECT_EQ(seq.GetLast(), 200);
    
    seq.InsertAt(150, 2);
    EXPECT_EQ(seq.Get(2), 150);
}

TEST(SequenceTest, ListSequenceIterator) 
{
    // Проверяет: Итератор для ListSequence
    int data[] = {1, 2, 3};
    ImmutableListSequence<int> seq(data, 3);
    
    auto iterator = seq.CreateIterator();
    int sum = 0;
    int count = 0;
    
    while (iterator->HasNext()) {
        sum += iterator->GetCurrentItem();
        iterator->Next();
        count++;
    }
    
    EXPECT_EQ(sum, 6);
    EXPECT_EQ(count, 3);
    delete iterator;
}

TEST(SequenceTest, SequenceGetCount) 
{
    // Проверяет: Метод GetCount в Sequence
    int data[] = {1, 2, 3, 4, 5};
    ImmutableArraySequence<int> seq(data, 5);
    
    EXPECT_EQ(seq.GetCount(), 5);
    EXPECT_EQ(seq.GetLength(), 5);
}

TEST(SequenceTest, LargeMutableArraySequence) 
{
    // Проверяет: Работу с большой MutableArraySequence
    const int SIZE = 1000;
    MutableArraySequence<int> seq;
    
    for (int i = 0; i < SIZE; i++) {
        seq.Append(i);
    }
    
    EXPECT_EQ(seq.GetLength(), SIZE);
    
    // Проверяем что можем получить все элементы
    for (int i = 0; i < SIZE; i++) {
        EXPECT_EQ(seq.Get(i), i);
    }
    
    // Проверяем итератор на большом наборе
    auto iterator = seq.CreateIterator();
    int count = 0;
    while (iterator->HasNext()) {
        iterator->Next();
        count++;
    }
    EXPECT_EQ(count, SIZE);
    delete iterator;
}

TEST(SequenceTest, StringSequence) 
{
    // Проверяет: Sequence с строковыми типами
    MutableArraySequence<std::string> seq;
    seq.Append("Hello");
    seq.Append("World");
    seq.Prepend("Start");
    
    EXPECT_EQ(seq.GetLength(), 3);
    EXPECT_EQ(seq.GetFirst(), "Start");
    EXPECT_EQ(seq.GetLast(), "World");
    EXPECT_EQ(seq.Get(1), "Hello");
}

// 4. Тесты BinaryTree

TEST(BinaryTreeTest, EmptyTreeOperations) 
{
    // Проверяет: Операции с пустым деревом
    BinaryTree<int> tree;
    EXPECT_TRUE(tree.isEmpty());
    EXPECT_FALSE(tree.contains(5));
    EXPECT_EQ(tree.getRoot(), nullptr);
}

TEST(BinaryTreeTest, InsertSingleElement) 
{
    // Проверяет: Вставку одного элемента в дерево
    BinaryTree<int> tree;
    tree.insert(5);
    
    EXPECT_FALSE(tree.isEmpty());
    EXPECT_TRUE(tree.contains(5));
    EXPECT_FALSE(tree.contains(10));
    EXPECT_NE(tree.getRoot(), nullptr);
    EXPECT_EQ(tree.getRoot()->data, 5);
}

TEST(BinaryTreeTest, InsertMultipleElements) 
{
    // Проверяет: Вставку нескольких элементов в дерево
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    tree.insert(2);
    tree.insert(4);
    tree.insert(6);
    tree.insert(8);
    
    EXPECT_FALSE(tree.isEmpty());
    EXPECT_TRUE(tree.contains(5));
    EXPECT_TRUE(tree.contains(3));
    EXPECT_TRUE(tree.contains(7));
    EXPECT_TRUE(tree.contains(2));
    EXPECT_TRUE(tree.contains(4));
    EXPECT_TRUE(tree.contains(6));
    EXPECT_TRUE(tree.contains(8));
    EXPECT_FALSE(tree.contains(10));
}

TEST(BinaryTreeTest, InsertDuplicateElements) 
{
    // Проверяет: Вставку дублирующихся элементов в дерево
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(5); // Дубликат
    tree.insert(5); // Еще один дубликат
    
    EXPECT_TRUE(tree.contains(5));
    
    int count = 0;
    tree.traverseInOrder([&](int value) 
    {
        if (value == 5) count++;
    });
    EXPECT_EQ(count, 1); // Должен быть только один узел со значением 5
}

TEST(BinaryTreeTest, RemoveLeafNode) 
{
    // Проверяет: Удаление листового узла из дерева
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    
    tree.remove(3);
    EXPECT_FALSE(tree.contains(3));
    EXPECT_TRUE(tree.contains(5));
    EXPECT_TRUE(tree.contains(7));
    
    // Проверяем что дерево корректно перестроилось
    EXPECT_NE(tree.getRoot(), nullptr);
    EXPECT_EQ(tree.getRoot()->data, 5);
}

TEST(BinaryTreeTest, RemoveNodeWithOneLeftChild) 
{
    // Проверяет: Удаление узла с одним левым потомком
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(2);
    
    tree.remove(3);
    EXPECT_FALSE(tree.contains(3));
    EXPECT_TRUE(tree.contains(2));
    EXPECT_TRUE(tree.contains(5));
    
    EXPECT_EQ(tree.getRoot()->left->data, 2);
}

TEST(BinaryTreeTest, RemoveNodeWithOneRightChild) 
{
    // Проверяет: Удаление узла с одним правым потомком
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(4);
    
    tree.remove(3);
    EXPECT_FALSE(tree.contains(3));
    EXPECT_TRUE(tree.contains(4));
    EXPECT_TRUE(tree.contains(5));
}

TEST(BinaryTreeTest, RemoveNodeWithTwoChildren) 
{
    // Проверяет: Удаление узла с двумя потомками
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    tree.insert(2);
    tree.insert(4);
    tree.insert(6);
    tree.insert(8);
    
    tree.remove(5);
    
    EXPECT_FALSE(tree.contains(5));
    EXPECT_TRUE(tree.contains(3));
    EXPECT_TRUE(tree.contains(7));
    EXPECT_TRUE(tree.contains(2));
    EXPECT_TRUE(tree.contains(4));
    EXPECT_TRUE(tree.contains(6));
    EXPECT_TRUE(tree.contains(8));
    
    EXPECT_NE(tree.getRoot(), nullptr);
}

TEST(BinaryTreeTest, RemoveRootWithOneChild) 
{
    // Проверяет: Удаление корня с одним потомком
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    
    tree.remove(5);
    EXPECT_FALSE(tree.contains(5));
    EXPECT_TRUE(tree.contains(3));
    EXPECT_EQ(tree.getRoot()->data, 3);
}

TEST(BinaryTreeTest, RemoveNonExistentElement) 
{
    // Проверяет: Удаление несуществующего элемента из дерева
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    
    EXPECT_NO_THROW(tree.remove(10));
    EXPECT_TRUE(tree.contains(5));
    EXPECT_TRUE(tree.contains(3));
    EXPECT_TRUE(tree.contains(7));
}

TEST(BinaryTreeTest, TraversePreOrder) 
{
    // Проверяет: Прямой обход дерева
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    tree.insert(1);
    tree.insert(4);
    
    std::vector<int> result;
    tree.traversePreOrder([&](int value) {
        result.push_back(value);
    });
    
    EXPECT_EQ(result.size(), 5);
    // В пре-порядке первым идет корень
    EXPECT_EQ(result[0], 5);
}

TEST(BinaryTreeTest, TraverseInOrderSorted) 
{
    // Проверяет: Симметричный обход дерева
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    tree.insert(1);
    tree.insert(4);
    tree.insert(6);
    tree.insert(8);
    
    std::vector<int> result;
    tree.traverseInOrder([&](int value) {
        result.push_back(value);
    });
    
    EXPECT_EQ(result.size(), 7);
    for (size_t i = 1; i < result.size(); i++) 
    {
        EXPECT_LE(result[i-1], result[i]);
    }
    EXPECT_EQ(result[0], 1);
    EXPECT_EQ(result[6], 8);
}

TEST(BinaryTreeTest, TraversePostOrder) 
{
    // Проверяет: Обратный обход дерева
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    
    std::vector<int> result;
    tree.traversePostOrder([&](int value) 
    {
        result.push_back(value);
    });
    
    EXPECT_EQ(result.size(), 3);

    EXPECT_EQ(result[2], 5);
}

TEST(BinaryTreeTest, TreeBalanceSimple) 
{
    // Проверяет: Балансировку дерева
    BinaryTree<int> tree;

    for (int i = 1; i <= 5; i++) 
    {
        tree.insert(i);
    }
    
    tree.balance();
    
    for (int i = 1; i <= 5; i++) 
    {
        EXPECT_TRUE(tree.contains(i));
    }
    
    std::vector<int> inOrder;
    tree.traverseInOrder([&](int value) 
    {
        inOrder.push_back(value);
    });
    EXPECT_EQ(inOrder.size(), 5);
    EXPECT_EQ(inOrder[0], 1);
    EXPECT_EQ(inOrder[4], 5);
}

TEST(BinaryTreeTest, TreeBalanceLarge) 
{
    // Проверяет: Балансировку большого дерева
    BinaryTree<int> tree;
    for (int i = 1; i <= 10; i++) 
    {
        tree.insert(i);
    }
    
    tree.balance();
    
    for (int i = 1; i <= 10; i++) 
    {
        EXPECT_TRUE(tree.contains(i));
    }
}

TEST(BinaryTreeTest, TreeBalanceEmpty) 
{
    // Проверяет: Балансировку пустого дерева
    BinaryTree<int> tree;

    EXPECT_NO_THROW(tree.balance());
    EXPECT_TRUE(tree.isEmpty());
}

TEST(BinaryTreeTest, TreeCopyConstructor) 
{
    // Проверяет: Конструктор копирования для дерева
    BinaryTree<int> tree1;
    tree1.insert(5);
    tree1.insert(3);
    tree1.insert(7);
    tree1.insert(1);
    tree1.insert(4);
    
    BinaryTree<int> tree2(tree1);
    
    EXPECT_TRUE(tree2.contains(5));
    EXPECT_TRUE(tree2.contains(3));
    EXPECT_TRUE(tree2.contains(7));
    EXPECT_TRUE(tree2.contains(1));
    EXPECT_TRUE(tree2.contains(4));
    
    tree1.insert(10);
    EXPECT_FALSE(tree2.contains(10));
}

TEST(BinaryTreeTest, TreeAssignmentOperator) 
{
    // Проверяет: Оператор присваивания для дерева
    BinaryTree<int> tree1;
    tree1.insert(10);
    tree1.insert(5);
    tree1.insert(15);
    
    BinaryTree<int> tree2;
    tree2.insert(100);
    
    tree2 = tree1;
    
    EXPECT_TRUE(tree2.contains(10));
    EXPECT_TRUE(tree2.contains(5));
    EXPECT_TRUE(tree2.contains(15));
    EXPECT_FALSE(tree2.contains(100));
    
    // Проверяем что это глубокое копирование
    tree1.insert(20);
    EXPECT_FALSE(tree2.contains(20));
}

TEST(BinaryTreeTest, TreeSelfAssignment) 
{
    // Проверяет: Само-присваивание для дерева
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    
    tree = tree; // Само-присваивание
    
    EXPECT_TRUE(tree.contains(5));
    EXPECT_TRUE(tree.contains(3));
    EXPECT_TRUE(tree.contains(7));
    EXPECT_EQ(tree.getRoot()->data, 5);
}

TEST(BinaryTreeTest, TreeClear) 
{
    // Проверяет: Очистку дерева
    BinaryTree<int> tree;
    for (int i = 0; i < 10; i++) 
    {
        tree.insert(i);
    }
    
    EXPECT_FALSE(tree.isEmpty());
    EXPECT_NE(tree.getRoot(), nullptr);
    
    tree.clear();
    
    EXPECT_TRUE(tree.isEmpty());
    EXPECT_EQ(tree.getRoot(), nullptr);
    
    // Проверяем что после очистки можно снова добавлять элементы
    tree.insert(100);
    EXPECT_TRUE(tree.contains(100));
    EXPECT_FALSE(tree.isEmpty());
}

TEST(BinaryTreeTest, TreeMapOperation) 
{
    // Проверяет: Операцию Map для дерева
    BinaryTree<int> tree;
    tree.insert(1);
    tree.insert(2);
    tree.insert(3);
    
    auto newTree = tree.map([](int x) { return x * 2; });
    
    EXPECT_FALSE(newTree.isEmpty());
    EXPECT_TRUE(newTree.contains(2));
    EXPECT_TRUE(newTree.contains(4));
    EXPECT_TRUE(newTree.contains(6));
    EXPECT_FALSE(newTree.contains(1));
    
    EXPECT_TRUE(tree.contains(1));
    EXPECT_TRUE(tree.contains(2));
    EXPECT_TRUE(tree.contains(3));
}

TEST(BinaryTreeTest, TreeWhereOperation) 
{
    // Проверяет: Операцию Where для дерева
    BinaryTree<int> tree;
    for (int i = 1; i <= 10; i++) 
    {
        tree.insert(i);
    }
    
    auto evenTree = tree.where([](int x) { return x % 2 == 0; });
    
    EXPECT_FALSE(evenTree.isEmpty());
    EXPECT_TRUE(evenTree.contains(2));
    EXPECT_TRUE(evenTree.contains(4));
    EXPECT_TRUE(evenTree.contains(6));
    EXPECT_TRUE(evenTree.contains(8));
    EXPECT_TRUE(evenTree.contains(10));
    EXPECT_FALSE(evenTree.contains(1));
    EXPECT_FALSE(evenTree.contains(3));
    EXPECT_FALSE(evenTree.contains(5));
    
    // Оригинальное дерево не должно измениться
    for (int i = 1; i <= 10; i++) 
    {
        EXPECT_TRUE(tree.contains(i));
    }
}


TEST(BinaryTreeTest, TreeMergeOperation) 
{
    // Проверяет: Операцию Merge для дерева
    BinaryTree<int> tree1;
    tree1.insert(1);
    tree1.insert(2);
    tree1.insert(3);
    
    BinaryTree<int> tree2;
    tree2.insert(4);
    tree2.insert(5);
    tree2.insert(6);
    
    tree1.merge(tree2);
    
    EXPECT_TRUE(tree1.contains(1));
    EXPECT_TRUE(tree1.contains(2));
    EXPECT_TRUE(tree1.contains(3));
    EXPECT_TRUE(tree1.contains(4));
    EXPECT_TRUE(tree1.contains(5));
    EXPECT_TRUE(tree1.contains(6));
    
    EXPECT_TRUE(tree2.contains(4));
    EXPECT_TRUE(tree2.contains(5));
    EXPECT_TRUE(tree2.contains(6));
}

TEST(BinaryTreeTest, TreeMergeWithOverlap) 
{
    // Проверяет: Объединение деревьев с перекрывающимися элементами
    BinaryTree<int> tree1;
    tree1.insert(1);
    tree1.insert(2);
    tree1.insert(3);
    
    BinaryTree<int> tree2;
    tree2.insert(2);
    tree2.insert(3);
    tree2.insert(4);
    
    tree1.merge(tree2);
    
    EXPECT_TRUE(tree1.contains(1));
    EXPECT_TRUE(tree1.contains(2));
    EXPECT_TRUE(tree1.contains(3));
    EXPECT_TRUE(tree1.contains(4));
}

TEST(BinaryTreeTest, ExtractSubtree) 
{
    // Проверяет: Извлечение поддерева
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    tree.insert(2);
    tree.insert(4);
    tree.insert(6);
    tree.insert(8);
    
    auto subtree = tree.extractSubtree(7);
    
    EXPECT_FALSE(subtree.isEmpty());
    EXPECT_TRUE(subtree.contains(7));
    EXPECT_TRUE(subtree.contains(6));
    EXPECT_TRUE(subtree.contains(8));
    EXPECT_FALSE(subtree.contains(5));
    EXPECT_FALSE(subtree.contains(3));
    
    EXPECT_TRUE(tree.contains(5));
    EXPECT_TRUE(tree.contains(7));
}

TEST(BinaryTreeTest, ExtractSubtreeNonExistent) 
{
    // Проверяет: Извлечение несуществующего поддерева
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    
    auto subtree = tree.extractSubtree(10);
    
    EXPECT_TRUE(subtree.isEmpty());
}

TEST(BinaryTreeTest, FindNode) 
{
    // Проверяет: Поиск узла в дереве
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    tree.insert(1);
    tree.insert(4);
    
    auto node = tree.findNode(3);
    EXPECT_NE(node, nullptr);
    EXPECT_EQ(node->data, 3);
    
    node = tree.findNode(1);
    EXPECT_NE(node, nullptr);
    EXPECT_EQ(node->data, 1);
    
    node = tree.findNode(10);
    EXPECT_EQ(node, nullptr);
}

TEST(BinaryTreeTest, ContainsSubtree) 
{
    // Проверяет: Проверку содержания поддерева
    BinaryTree<int> tree1;
    tree1.insert(5);
    tree1.insert(3);
    tree1.insert(7);
    tree1.insert(2);
    tree1.insert(4);
    tree1.insert(6);
    tree1.insert(8);
    
    BinaryTree<int> subtree;
    subtree.insert(3);
    subtree.insert(2);
    subtree.insert(4);
    
    EXPECT_TRUE(tree1.containsSubtree(subtree));
    
    BinaryTree<int> notSubtree;
    notSubtree.insert(3);
    notSubtree.insert(2);
    notSubtree.insert(5);
    
    EXPECT_FALSE(tree1.containsSubtree(notSubtree));
}

TEST(BinaryTreeTest, TreeIteratorBasic) 
{
    // Проверяет: Базовые операции итератора дерева
    BinaryTree<int> tree;
    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    
    auto iterator = tree.CreateIterator();
    EXPECT_TRUE(iterator->HasNext());
    
    // Собираем все элементы
    std::vector<int> elements;
    while (iterator->HasNext()) 
    {
        elements.push_back(iterator->GetCurrentItem());
        iterator->Next();
    }
    
    EXPECT_EQ(elements.size(), 3);
    
    iterator->Reset();
    EXPECT_TRUE(iterator->HasNext());
    
    delete iterator;
}

TEST(BinaryTreeTest, TreeIteratorEmptyTree) 
{
    // Проверяет: Итератор для пустого дерева
    BinaryTree<int> tree;
    auto iterator = tree.CreateIterator();
    
    EXPECT_FALSE(iterator->HasNext());
    EXPECT_THROW(iterator->GetCurrentItem(), std::out_of_range);
    
    iterator->Reset();
    EXPECT_FALSE(iterator->HasNext());
    
    delete iterator;
}

TEST(BinaryTreeTest, TreeIteratorReset) 
{
    // Проверяет: Сброс итератора дерева
    BinaryTree<int> tree;
    tree.insert(1);
    tree.insert(2);
    tree.insert(3);
    
    auto iterator = tree.CreateIterator();
    
    while (iterator->HasNext()) 
    {
        iterator->Next();
    }
    
    EXPECT_FALSE(iterator->HasNext());
    
    iterator->Reset();
    EXPECT_TRUE(iterator->HasNext());
    EXPECT_EQ(iterator->GetCurrentItem(), 1);
    
    delete iterator;
}

TEST(BinaryTreeTest, StringBinaryTreeOperations) 
{
    // Проверяет: Операции бинарного дерева со строковыми типами
    BinaryTree<std::string> tree;
    tree.insert("apple");
    tree.insert("banana");
    tree.insert("cherry");
    
    EXPECT_TRUE(tree.contains("banana"));
    EXPECT_FALSE(tree.contains("grape"));
    
    auto uppercaseTree = tree.map([](const std::string& s) 
    {
        std::string result = s;
        for (char& c : result) c = toupper(c);
        return result;
    });
    
    EXPECT_TRUE(uppercaseTree.contains("APPLE"));
    EXPECT_TRUE(uppercaseTree.contains("BANANA"));
    EXPECT_TRUE(uppercaseTree.contains("CHERRY"));
    EXPECT_FALSE(uppercaseTree.contains("apple")); // В нижнем регистре
}

// 5. Тесты Dictionary с простыми типами

TEST(DictionaryTest, EmptyDictionaryOperations) 
{
    // Проверяет: Операции с пустым словарем
    Dictionary<int, std::string> dict;
    EXPECT_TRUE(dict.IsEmpty());
    EXPECT_EQ(dict.GetCount(), 0);
    EXPECT_FALSE(dict.ContainsKey(1));
    EXPECT_THROW(dict.Get(1), std::runtime_error);
}

TEST(DictionaryTest, BasicAddAndGet) 
{
    // Проверяет: Базовое добавление и получение элементов
    Dictionary<int, std::string> dict;
    dict.Add(1, "one");
    dict.Add(2, "two");
    
    EXPECT_EQ(dict.GetCount(), 2);
    EXPECT_TRUE(dict.ContainsKey(1));
    EXPECT_TRUE(dict.ContainsKey(2));
    EXPECT_EQ(dict.Get(1), "one");
    EXPECT_EQ(dict.Get(2), "two");
}

TEST(DictionaryTest, AddDuplicateKey) 
{
    // Проверяет: Добавление дублирующегося ключа
    Dictionary<int, std::string> dict;
    dict.Add(1, "old");
    dict.Add(1, "new");
    
    EXPECT_EQ(dict.GetCount(), 1);
    EXPECT_EQ(dict.Get(1), "new");
}

TEST(DictionaryTest, RemoveOperations) 
{
    // Проверяет: Удаление элементов из словаря
    Dictionary<int, std::string> dict;
    dict.Add(1, "a");
    dict.Add(2, "b");
    dict.Add(3, "c");
    
    dict.Remove(2);
    EXPECT_FALSE(dict.ContainsKey(2));
    EXPECT_EQ(dict.GetCount(), 2);
    EXPECT_TRUE(dict.ContainsKey(1));
    EXPECT_TRUE(dict.ContainsKey(3));
    
    dict.Remove(10);
    EXPECT_EQ(dict.GetCount(), 2);
}

TEST(DictionaryTest, ClearDictionary) 
{
    // Проверяет: Очистку словаря
    Dictionary<int, std::string> dict;
    dict.Add(1, "a");
    dict.Add(2, "b");
    
    EXPECT_EQ(dict.GetCount(), 2);
    
    dict.Clear();
    EXPECT_TRUE(dict.IsEmpty());
    EXPECT_EQ(dict.GetCount(), 0);
    EXPECT_FALSE(dict.ContainsKey(1));
}

TEST(DictionaryTest, GetKeysAndValues) 
{
    // Проверяет: Получение ключей и значений словаря
    Dictionary<int, std::string> dict;
    dict.Add(1, "one");
    dict.Add(2, "two");
    dict.Add(3, "three");
    
    auto keys = dict.GetKeys();
    auto values = dict.GetValues();
    
    EXPECT_EQ(keys->GetLength(), 3);
    EXPECT_EQ(values->GetLength(), 3);
    
    // Проверяем что ключи корректны
    bool found1 = false, found2 = false, found3 = false;
    for (int i = 0; i < keys->GetLength(); i++) 
    {
        int key = keys->Get(i);
        if (key == 1) found1 = true;
        if (key == 2) found2 = true;
        if (key == 3) found3 = true;
    }
    EXPECT_TRUE(found1 && found2 && found3);
    
    delete keys;
    delete values;
}

TEST(DictionaryTest, ForEachOperations) 
{
    // Проверяет: Операции ForEach для словаря
    Dictionary<int, std::string> dict;
    dict.Add(1, "one");
    dict.Add(2, "two");
    dict.Add(3, "three");
    
    int keySum = 0;
    dict.ForEachKey([&](int key) { keySum += key; });
    EXPECT_EQ(keySum, 6);
    
    int valueCount = 0;
    dict.ForEachValue([&](const std::string&) { valueCount++; });
    EXPECT_EQ(valueCount, 3);
    
    std::string allValues;
    dict.ForEach([&](int key, const std::string& value) 
    {
        allValues += value;
    });
    EXPECT_FALSE(allValues.empty());
}

TEST(DictionaryTest, ContainsValue) 
{
    // Проверяет: Проверку содержания значения в словаре
    Dictionary<int, std::string> dict;
    dict.Add(1, "apple");
    dict.Add(2, "banana");
    dict.Add(3, "apple");

    EXPECT_TRUE(dict.ContainsValue("apple"));
    EXPECT_TRUE(dict.ContainsValue("banana"));
    EXPECT_FALSE(dict.ContainsValue("cherry"));
}

TEST(DictionaryTest, BalanceDictionary) 
{
    // Проверяет: Балансировку словаря
    Dictionary<int, std::string> dict;
    for (int i = 0; i < 100; i++) 
    {
        dict.Add(i, "value" + std::to_string(i));
    }
    
    dict.Balance();
    EXPECT_EQ(dict.GetCount(), 100);
    
    for (int i = 0; i < 100; i++) 
    {
        EXPECT_TRUE(dict.ContainsKey(i));
        EXPECT_EQ(dict.Get(i), "value" + std::to_string(i));
    }
}

TEST(DictionaryTest, StringKeyDictionary) 
{
    // Проверяет: Словарь со строковыми ключами
    Dictionary<std::string, int> dict;
    dict.Add("apple", 5);
    dict.Add("banana", 3);
    dict.Add("cherry", 7);
    
    EXPECT_TRUE(dict.ContainsKey("banana"));
    EXPECT_EQ(dict.Get("apple"), 5);
    
    dict.Remove("banana");
    EXPECT_FALSE(dict.ContainsKey("banana"));
}

TEST(DictionaryTest, LargeDictionaryOperations) 
{
    // Проверяет: Работу с большим словарем
    Dictionary<int, int> dict;
    const int COUNT = 1000;
    
    for (int i = 0; i < COUNT; i++) 
    {
        dict.Add(i, i * i);
    }
    
    EXPECT_EQ(dict.GetCount(), COUNT);
    
    for (int i = 0; i < COUNT; i++) 
    {
        EXPECT_TRUE(dict.ContainsKey(i));
        EXPECT_EQ(dict.Get(i), i * i);
    }
    
    // Удаляем каждое второе значение
    for (int i = 0; i < COUNT; i += 2) 
    {
        dict.Remove(i);
    }
    
    EXPECT_EQ(dict.GetCount(), COUNT / 2);
    
    for (int i = 1; i < COUNT; i += 2)
    {
        EXPECT_TRUE(dict.ContainsKey(i));
    }
    
    for (int i = 0; i < COUNT; i += 2) 
    {
        EXPECT_FALSE(dict.ContainsKey(i));
    }
}

// 6. Тесты для гистограммы

TEST(HistogramTest, EmptySequence) 
{
    // Проверяет: Построение гистограммы из пустой последовательности
    MutableArraySequence<double> sequence;
    Histogram<double> histogram;
    histogram.CreateUniformRanges(0, 10, 5);
    histogram.BuildHistogram(&sequence);
    auto counts = histogram.GetCounts();
    EXPECT_EQ(counts.GetCount(), 0);
    EXPECT_EQ(histogram.GetTotalCount(), 0);
}

TEST(HistogramTest, BasicHistogramConstruction) 
{
    // Проверяет: Базовое построение гистограммы
    double testData[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
    MutableArraySequence<double> sequence(testData, 10);
    
    Histogram<double> histogram;
    histogram.CreateUniformRanges(0.0, 10.0, 5);
    histogram.BuildHistogram(&sequence);
    
    auto counts = histogram.GetCounts();
    EXPECT_EQ(counts.GetCount(), 5);
    EXPECT_EQ(histogram.GetTotalCount(), 10);
}

TEST(HistogramTest, HistogramWithSingleValue) 
{
    // Проверяет: Гистограмму с одним значением
    MutableArraySequence<int> sequence;
    sequence.Append(5);
    sequence.Append(5);
    sequence.Append(5);
    
    Histogram<int> histogram;
    histogram.CreateUniformRanges(0, 10, 2);
    histogram.BuildHistogram(&sequence);
    
    EXPECT_EQ(histogram.GetTotalCount(), 3);
    
    auto mostFrequent = histogram.GetMostFrequentRange();
    auto leastFrequent = histogram.GetLeastFrequentRange();
    
    EXPECT_GT(mostFrequent.GetSize(), 0);
    EXPECT_GT(leastFrequent.GetSize(), 0);
}

TEST(HistogramTest, HistogramWithCustomRanges) 
{
    // Проверяет: Построение гистограммы с пользовательскими диапазонами
    double testData[] = {15.0, 25.0, 35.0, 45.0, 55.0, 65.0, 75.0, 85.0};
    MutableArraySequence<double> sequence(testData, 8);
    
    Histogram<double> histogram;
    
    DynamicArray<Range<double>> customRanges(3);
    customRanges.Set(0, Range<double>(0.0, 30.0));
    customRanges.Set(1, Range<double>(30.0, 60.0));
    customRanges.Set(2, Range<double>(60.0, 100.0));
    
    histogram.SetCustomRanges(customRanges);
    histogram.BuildHistogram(&sequence);
    
    EXPECT_EQ(histogram.GetTotalCount(), 8);
    EXPECT_EQ(histogram.GetRanges().GetSize(), 3);
    
    // Проверяем что диапазоны корректны
    for (int i = 0; i < histogram.GetRanges().GetSize(); i++) 
    {
        Range<double> range = histogram.GetRanges().Get(i);
        EXPECT_LE(range.min, range.max);
    }
}

TEST(HistogramTest, HistogramStatistics) 
{
    // Проверяет: Статистические расчеты для гистограммы
    double testData[] = {10.0, 10.0, 20.0, 30.0, 30.0, 30.0, 40.0, 50.0};
    MutableArraySequence<double> sequence(testData, 8);
    
    Histogram<double> histogram;
    histogram.CreateUniformRanges(0.0, 50.0, 5);
    histogram.BuildHistogram(&sequence);
    
    EXPECT_EQ(histogram.GetTotalCount(), 8);
    
    auto mostFrequent = histogram.GetMostFrequentRange();
    auto leastFrequent = histogram.GetLeastFrequentRange();
    
    EXPECT_GT(mostFrequent.GetSize(), 0);
    EXPECT_GT(leastFrequent.GetSize(), 0);
    
    EXPECT_TRUE(histogram.FindRange(30.0).GetSize() > 0);
}

TEST(HistogramTest, HistogramMapReduce) 
{
    // Проверяет: Построение гистограммы с использованием Map-Reduce
    double testData[] = {1.0, 2.0, 3.0, 2.0, 1.0, 3.0, 3.0};
    MutableArraySequence<double> sequence(testData, 7);
    
    Histogram<double> histogram;
    histogram.CreateUniformRanges(0.0, 4.0, 2);
    
    histogram.BuildHistogram(&sequence);
    int regularCount = histogram.GetTotalCount();

    histogram.Clear();
    histogram.CreateUniformRanges(0.0, 4.0, 2);
    histogram.BuildHistogramMapReduce(&sequence);
    int mapReduceCount = histogram.GetTotalCount();
    
    EXPECT_EQ(regularCount, mapReduceCount);
    EXPECT_EQ(regularCount, 7);
}


TEST(HistogramTest, ClearHistogram) 
{
    // Проверяет: Очистку гистограммы
    double testData[] = {1.0, 2.0, 3.0};
    MutableArraySequence<double> sequence(testData, 3);
    
    Histogram<double> histogram;
    histogram.CreateUniformRanges(0.0, 5.0, 3);
    histogram.BuildHistogram(&sequence);
    
    EXPECT_GT(histogram.GetTotalCount(), 0);
    EXPECT_GT(histogram.GetCounts().GetCount(), 0);
    EXPECT_GT(histogram.GetRanges().GetSize(), 0);
    
    histogram.Clear();
    EXPECT_EQ(histogram.GetTotalCount(), 0);
    EXPECT_EQ(histogram.GetCounts().GetCount(), 0);
    EXPECT_EQ(histogram.GetRanges().GetSize(), 0);
}

// 7. Тесты для приоритезации задач

TEST(TaskPrioritizerTest, EmptyIncidentsAndTasks) 
{
    // Проверяет: Приоритезацию пустых инцидентов и задач
    auto incidents = new MutableArraySequence<Incident>();
    auto tasks = new MutableArraySequence<Task>();
    
    TaskPrioritizer prioritizer;
    prioritizer.CalculateKeywordPriorities(incidents);
    prioritizer.PrioritizeTasks(tasks);
    
    EXPECT_EQ(tasks->GetLength(), 0);
    EXPECT_EQ(prioritizer.GetKeywordCount(), 0);
    EXPECT_EQ(prioritizer.GetTotalIncidentWeight(), 0);
    
    delete incidents;
    delete tasks;
}

TEST(TaskPrioritizerTest, SingleIncidentSingleTask) 
{
    // Проверяет: Приоритезацию одного инцидента и одной задачи
    auto incidents = new MutableArraySequence<Incident>();
    incidents->Append(Incident("bug", 5));
    
    auto tasks = new MutableArraySequence<Task>();
    tasks->Append(Task("Fix bug", "bug"));
    
    TaskPrioritizer prioritizer;
    prioritizer.CalculateKeywordPriorities(incidents);
    prioritizer.PrioritizeTasks(tasks);
    
    EXPECT_EQ(tasks->GetLength(), 1);
    EXPECT_GT(tasks->Get(0).calculatedPriority, 0);
    EXPECT_EQ(prioritizer.GetKeywordCount(), 1);
    EXPECT_EQ(prioritizer.GetTotalIncidentWeight(), 5);
    
    delete incidents;
    delete tasks;
}



TEST(TaskPrioritizerTest, TaskPrioritizationWithQueue) 
{
    // Проверяет: Приоритезацию задач с использованием очереди
    auto incidents = new MutableArraySequence<Incident>();
    incidents->Append(Incident("security", 10));
    incidents->Append(Incident("performance", 5));
    
    auto tasks = new MutableArraySequence<Task>();
    tasks->Append(Task("Secure login", "security"));
    tasks->Append(Task("Optimize query", "performance"));
    tasks->Append(Task("Update UI", "ui"));
    
    TaskPrioritizer prioritizer;
    prioritizer.CalculateKeywordPriorities(incidents);
    
    auto queue = prioritizer.PrioritizeTasksWithQueue(tasks);
    
    EXPECT_EQ(queue.GetCount(), 3);
    EXPECT_FALSE(queue.IsEmpty());
    
    Task topTask = queue.Peek();
    EXPECT_GT(topTask.calculatedPriority, 0);
    
    std::vector<Task> extracted;
    while (!queue.IsEmpty()) 
    {
        extracted.push_back(queue.Dequeue());
    }
    
    for (size_t i = 1; i < extracted.size(); i++) 
    {
        EXPECT_GE(extracted[i-1].calculatedPriority, extracted[i].calculatedPriority);
    }
    
    delete incidents;
    delete tasks;
}


// 8. Тесты для поиска подстрок

TEST(MostFrequentSubstringsTest, EmptyString) 
{
    // Проверяет: Поиск подстрок в пустой строке
    MostFrequentSubstrings finder;
    auto sequence = new MutableArraySequence<char>(0);
    
    finder.FindMostFrequent(sequence, 2, 4);
    EXPECT_EQ(finder.GetUniqueCount(), 0);
    EXPECT_EQ(finder.GetTotalCount(), 0);
    
    auto topResults = finder.GetTopFrequent(5);
    EXPECT_EQ(topResults->GetLength(), 0);
    
    
    delete topResults;
    delete sequence;
}

TEST(MostFrequentSubstringsTest, SingleCharacterString) 
{
    // Проверяет: Поиск подстрок в строке из одного символа
    MostFrequentSubstrings finder;
    std::string testStr = "aaaa";
    
    auto sequence = new MutableArraySequence<char>(static_cast<int>(testStr.length()));
    for (size_t i = 0; i < testStr.length(); i++) 
    {
        sequence->Set(static_cast<int>(i), testStr[i]);
    }
    
    finder.FindMostFrequent(sequence, 1, 2);
    EXPECT_GT(finder.GetUniqueCount(), 0);
    EXPECT_GT(finder.GetTotalCount(), 0);
    
    auto topResults = finder.GetTopFrequent(3);
    EXPECT_GT(topResults->GetLength(), 0);
    
    bool foundA = false;
    for (int i = 0; i < topResults->GetLength(); i++) 
    {
        auto sc = topResults->Get(i);
        if (sc.substring.GetSize() == 1 && sc.substring.Get(0) == 'a') 
        {
            foundA = true;
            EXPECT_EQ(sc.count, 4); 
            break;
        }
    }
    
    EXPECT_TRUE(foundA);
    
    
    delete topResults;
    delete sequence;
}

TEST(MostFrequentSubstringsTest, RepeatedPatternString) 
{
    // Проверяет: Поиск подстрок в строке с повторяющимся паттерном
    MostFrequentSubstrings finder;
    std::string testStr = "abcabcabc";
    
    auto sequence = new MutableArraySequence<char>(static_cast<int>(testStr.length()));
    for (size_t i = 0; i < testStr.length(); i++) 
    {
        sequence->Set(static_cast<int>(i), testStr[i]);
    }
    
    finder.FindMostFrequent(sequence, 3, 6);
    
    EXPECT_GT(finder.GetUniqueCount(), 0);
    EXPECT_GT(finder.GetTotalCount(), 0);
    
    auto topResults = finder.GetTopFrequent(1);
    if (topResults->GetLength() > 0) 
    {
        auto top = topResults->Get(0);
        EXPECT_GE(top.count, 3);

        if (top.substring.GetSize() == 3) 
        {
            EXPECT_EQ(top.substring.Get(0), 'a');
            EXPECT_EQ(top.substring.Get(1), 'b');
            EXPECT_EQ(top.substring.Get(2), 'c');
        }
    }
    
    auto allViaIterator = finder.GetAllViaIterator();
    EXPECT_GT(allViaIterator->GetLength(), 0);
    
    for (int i = 0; i < allViaIterator->GetLength(); i++) 
    {
        auto sc = allViaIterator->Get(i);
        EXPECT_GT(sc.substring.GetSize(), 0);
        EXPECT_GT(sc.count, 0);
    }
    
    
    delete topResults;
    delete allViaIterator;
    delete sequence;
}

TEST(MostFrequentSubstringsTest, ComplexStringAnalysis) 
{
    // Проверяет: Анализ сложной строки для поиска подстрок
    MostFrequentSubstrings finder;
    std::string testStr = "abracadabra";
    
    auto sequence = new MutableArraySequence<char>(static_cast<int>(testStr.length()));
    for (size_t i = 0; i < testStr.length(); i++) 
    {
        sequence->Set(static_cast<int>(i), testStr[i]);
    }
    
    finder.FindMostFrequent(sequence, 2, 4);
    
    EXPECT_GT(finder.GetUniqueCount(), 0);
    EXPECT_GT(finder.GetTotalCount(), 0);
    
    auto topResults = finder.GetTopFrequent(5);
    EXPECT_GT(topResults->GetLength(), 0);
    
    bool foundCommon = false;
    for (int i = 0; i < topResults->GetLength(); i++) 
    {
        auto sc = topResults->Get(i);
        if (sc.substring.GetSize() == 2) 
        {
            char c1 = sc.substring.Get(0);
            char c2 = sc.substring.Get(1);
            if ((c1 == 'a' && c2 == 'b') || (c1 == 'r' && c2 == 'a')) 
            {
                foundCommon = true;
                EXPECT_GT(sc.count, 1);
            }
        }
    }
    
    EXPECT_TRUE(foundCommon);
    
    delete topResults;
    delete sequence;
}

TEST(MostFrequentSubstringsTest, IteratorFunctionality) 
{
    // Проверяет: Функциональность итератора для поиска подстрок
    MostFrequentSubstrings finder;
    std::string testStr = "teststring";
    
    auto sequence = new MutableArraySequence<char>(static_cast<int>(testStr.length()));
    for (size_t i = 0; i < testStr.length(); i++) 
    {
        sequence->Set(static_cast<int>(i), testStr[i]);
    }
    
    finder.FindMostFrequent(sequence, 2, 4);
    
    auto allViaIterator = finder.GetAllViaIterator();
    EXPECT_GT(allViaIterator->GetLength(), 0);

    int uniqueCount = finder.GetUniqueCount();
    int totalCount = finder.GetTotalCount();
    EXPECT_GT(uniqueCount, 0);
    EXPECT_GT(totalCount, 0);
    
    finder.Clear();
    EXPECT_EQ(finder.GetUniqueCount(), 0);
    EXPECT_EQ(finder.GetTotalCount(), 0);
    
    delete allViaIterator;
    delete sequence;
}

// 9. Интеграционные тесты

TEST(IntegrationTest, HistogramWithDictionary) 
{
    // Проверяет: Интеграцию гистограммы и словаря
    double testData[] = {10.0, 20.0, 30.0, 40.0, 50.0};
    MutableArraySequence<double> sequence(testData, 5);
    
    Histogram<double> histogram;
    histogram.CreateUniformRanges(0.0, 60.0, 3);
    histogram.BuildHistogram(&sequence);
    
    auto counts = histogram.GetCounts();
    
    EXPECT_GT(counts.GetCount(), 0);
    
    int total = histogram.GetTotalCount();
    EXPECT_EQ(total, 5);
    
    auto mostFrequent = histogram.GetMostFrequentRange();
    auto leastFrequent = histogram.GetLeastFrequentRange();
    EXPECT_GT(mostFrequent.GetSize(), 0);
    
}

TEST(IntegrationTest, BinaryTreeWithSet) 
{
    // Проверяет: Интеграцию бинарного дерева и множества
    BinaryTree<int> tree;
    Set<int> set;
    
    for (int i = 1; i <= 5; i++) 
    {
        tree.insert(i);
        set.Add(i);
    }
    
    for (int i = 1; i <= 5; i++) 
    {
        EXPECT_TRUE(tree.contains(i));
        EXPECT_TRUE(set.Contains(i));
    }
    
    Set<int> set2;
    set2.Add(3);
    set2.Add(4);
    set2.Add(5);
    set2.Add(6);
    
    Set<int> unionSet = set.Union(set2);
    EXPECT_GT(unionSet.GetCount(), 0);
    
    Set<int> intersectionSet = set.Intersection(set2);
    EXPECT_TRUE(intersectionSet.Contains(3));
    EXPECT_TRUE(intersectionSet.Contains(4));
    EXPECT_TRUE(intersectionSet.Contains(5));
    EXPECT_FALSE(intersectionSet.Contains(1));
    EXPECT_FALSE(intersectionSet.Contains(6));
}


TEST(IntegrationTest, DictionaryAndBinaryTreeIntegration) 
{
    // Проверяет: Интеграцию словаря и бинарного дерева
    Dictionary<int, std::string> dict;
    
    for (int i = 0; i < 10; i++) 
    {
        dict.Add(i, "Value" + std::to_string(i));
    }
    
    EXPECT_EQ(dict.GetCount(), 10);
    
    auto keys = dict.GetKeys();
    auto values = dict.GetValues();
    
    EXPECT_EQ(keys->GetLength(), 10);
    EXPECT_EQ(values->GetLength(), 10);
    
    EXPECT_TRUE(dict.ContainsKey(5));
    EXPECT_EQ(dict.Get(5), "Value5");
    
    dict.Remove(5);
    EXPECT_FALSE(dict.ContainsKey(5));
    EXPECT_EQ(dict.GetCount(), 9);
    
    dict.Balance();
    
    delete keys;
    delete values;
}

TEST(IntegrationTest, CompleteWorkflow) 
{
    // Проверяет: Полный рабочий процесс с использованием нескольких структур данных

    MutableArraySequence<double> dataSequence;
    for (int i = 0; i < 100; i++) 
    {
        dataSequence.Append((i % 20) * 5.0);
    }
    
    // 2. Строим гистограмму
    Histogram<double> histogram;
    histogram.CreateUniformRanges(0.0, 100.0, 10);
    histogram.BuildHistogram(&dataSequence);
    
    // 3. Проверяем результаты гистограммы
    EXPECT_GT(histogram.GetTotalCount(), 0);
    
    // 4. Создаем приоритетную очередь для статистики
    PriorityQueue<int> statsQueue;
    statsQueue.Enqueue(histogram.GetTotalCount());
    
    // 5. Проверяем что можем извлечь статистику
    EXPECT_FALSE(statsQueue.IsEmpty());
    
    // 6. Создаем дерево для хранения уникальных значений
    BinaryTree<double> uniqueValues;
    for (int i = 0; i < dataSequence.GetLength(); i++) 
    {
        uniqueValues.insert(dataSequence.Get(i));
    }
    
    // 7. Проверяем что дерево содержит некоторые значения
    EXPECT_TRUE(uniqueValues.contains(0.0));
    EXPECT_TRUE(uniqueValues.contains(95.0));
    
    // 8. Балансируем дерево
    uniqueValues.balance();
    
    // 9. Проверяем что все элементы все еще на месте
    for (int i = 0; i < 20; i++) 
    {
        EXPECT_TRUE(uniqueValues.contains(i * 5.0));
    }
}

int main(int argc, char **argv) 
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}