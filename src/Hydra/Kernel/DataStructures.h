#pragma once

#include <array>
#include <vector>
#include <map>
#include <list>
#include <forward_list>
#include <unordered_map>
#include <bitset>

#include <Hydra/Kernel/Assert.h>

namespace hydra {

    // Rename containers for now, until a proper implementation is done.
    template <typename T>
    using StdArray                  = std::vector<T>;

    template <typename T, size_t size>
    using StdStaticArray            = std::array<T, size>;

    template<typename T>
    using DoubleLinkedList          = std::list<T>;

    template<typename T>
    using LinkedList                = std::forward_list<T>;

    template <class Key, class T>
    using Map                       = std::map<Key, T>;

    template <class Key, class T>
    using HashMap                   = std::unordered_map<Key, T>;

    using String                    = std::string;

    using Flag                      = std::bitset<32>;
    using Flag8                     = std::bitset<8>;
    using Flag16                    = std::bitset<16>;
    using Flag64                    = std::bitset<64>;

    //////////////////////////////////////////////////////////////////////////
    // Defines the basic array interface over a storage.
    template <typename STORAGE, typename T>
    struct BaseArrayView {

        BaseArrayView();
        ~BaseArrayView();
        BaseArrayView( const BaseArrayView& other );
        BaseArrayView& operator=( const BaseArrayView& other );

        T& operator[]( uint32 index );
        const T& operator[]( uint32 index ) const;

        T* begin();
        const T* begin() const;
        T* end();
        const T* end() const;

        T& front();
        const T& front() const;
        T& back();
        const T& back() const;

        STORAGE data;
        uint32 _size;
    };


    //////////////////////////////////////////////////////////////////////////
    // Array View
    // Read/write view of a contiguous block of memory already allocated.
    template <typename T>
    struct ArrayView : public BaseArrayView<T*, T> {

        ArrayView() {}
        ArrayView( T* data, uint32 size );
        ~ArrayView();

        ArrayView( const ArrayView& other );
        ArrayView& operator=( const ArrayView& other );

        uint32 length() const;
    };


    //////////////////////////////////////////////////////////////////////////
    // Array for POD Only objects (objects that does not require a constructor)
    // ArrayView + memory allocation.
    template <typename T>
    struct Array : public BaseArrayView<T*, T> {

        Array();
        ~Array();

        // These operations must be more explicit, can hidden memcpys.
        Array( const Array& other ) = delete;
        Array& operator=( const Array& other ) = delete;

        ArrayView<T> createView();

        void push_back( const T& item );
        void pop_back();
        // TODO: emplace back ?

        // Erase by swapping element at index with last element.
        void erase( uint32 index );

        void resize( uint32 newSize );
        void clear();
        void setCapacity( uint32 newCapacity );
        void reserve( uint32 newCapacity );
        void grow( uint32 minCapacity );
        void trim();

        bool empty() const;
        uint32 size() const;

        // Allocator ?
        uint32 capacity;
    };

    //////////////////////////////////////////////////////////////////////////
    // Static Array, POD only.
    template <typename T, uint32 count>
    struct StaticArray : BaseArrayView<T[count], T> {

        StaticArray();
        ~StaticArray();

        // TODO: should these be implemented ?
        /*StaticArray( const StaticArray& other ) = delete;
        StaticArray& operator=( const StaticArray& other ) = delete;*/

        uint32 maxSize() const;

        ArrayView<T> createView();
    };


    // Convenience aliases
    using Buffer                    = Array<uint8>;

    template <uint32 count>
    using StaticBuffer              = StaticArray<uint8, count>;

    #include "Hydra/Kernel/DataStructures.hpp"
}