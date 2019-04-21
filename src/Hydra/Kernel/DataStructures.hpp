#pragma once


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// BaseArrayView implementation

template <typename STORAGE, typename T> BaseArrayView<STORAGE, T>::BaseArrayView() {}
template <typename STORAGE, typename T> BaseArrayView<STORAGE, T>::~BaseArrayView() {}
template <typename STORAGE, typename T> BaseArrayView<STORAGE, T>::BaseArrayView( const BaseArrayView<STORAGE, T> & other ) {}
template <typename STORAGE, typename T> BaseArrayView<STORAGE, T>& BaseArrayView<STORAGE, T>::operator=( const BaseArrayView<STORAGE, T>& other ) {}

template <typename STORAGE, typename T> T& BaseArrayView<STORAGE, T>::operator []( uint32 index ) {
    return data[index];
}

template <typename STORAGE, typename T> const T& BaseArrayView<STORAGE, T>::operator []( uint32 index ) const {
    return data[index];
}

template <typename STORAGE, typename T> T* BaseArrayView<STORAGE, T>::begin() {
    return &data[0];
}

template <typename STORAGE, typename T> const T* BaseArrayView<STORAGE, T>::begin() const {
    return &data[0];
}

template <typename STORAGE, typename T> T* BaseArrayView<STORAGE, T>::end() {
    return &data[_size];
}

template <typename STORAGE, typename T> const T* BaseArrayView<STORAGE, T>::end() const {
    return &data[_size];
}

template <typename STORAGE, typename T> T& BaseArrayView<STORAGE, T>::front() {
    return data[0];
}

template <typename STORAGE, typename T> const T& BaseArrayView<STORAGE, T>::front() const {
    return data[0];
}

template <typename STORAGE, typename T> T& BaseArrayView<STORAGE, T>::back() {
    return data[_size - 1];
}

template <typename STORAGE, typename T> const T& BaseArrayView<STORAGE, T>::back() const {
    return data[_size - 1];
}

//////////////////////////////////////////////////////////////////////////
// Array implementation
template <typename T> Array<T>::Array() : BaseArrayView(), capacity( 0 ) {
    data = nullptr;
    _size = 0;
}

template <typename T> Array<T>::~Array() {
    _aligned_free( data );
}

template <typename T>
void Array<T>::push_back( const T& item ) {
    if ( _size + 1 > capacity )
        grow( kAutomaticGrowSize );
    data[_size++] = item;
}

template <typename T>
void Array<T>::pop_back() {
    --_size;
}

template <typename T>
void Array<T>::trim() {
    setCapacity( _size );
}

static const uint32 kAutomaticGrowSize = 0;

template <typename T>
void Array<T>::resize( uint32 newSize ) {
    if ( newSize > capacity )
        grow( newSize );
    _size = newSize;
}

template <typename T>
void Array<T>::grow( uint32 minCapacity ) {
    uint32 newCapacity = capacity * 2 + 8;
    if ( newCapacity < minCapacity )
        newCapacity = minCapacity;
    setCapacity( newCapacity );
}

template <typename T>
void Array<T>::reserve( uint32 newCapacity ) {
    if ( newCapacity > capacity )
        setCapacity( newCapacity );
}

template <typename T>
void Array<T>::setCapacity( uint32 newCapacity ) {
    if ( newCapacity == capacity )
        return;

    if ( newCapacity < _size )
        resize( newCapacity );

    T* newData = 0;
    if ( newCapacity > 0 ) {
        newData = (T *)_aligned_malloc( sizeof( T ) * newCapacity, alignof( T ) );
        memcpy( newData, data, sizeof( T ) * _size );
    }

    _aligned_free( data );

    data = newData;
    capacity = newCapacity;
}

template <typename T>
void Array<T>::erase( uint32 index ) {
    Assert( index < _size );
    data[index] = back();
    --_size;
}

template <typename T>
void Array<T>::clear() {
    resize( 0 );
}

template <typename T>
bool Array<T>::empty() const {
    return _size == 0;
}

template <typename T>
uint32 Array<T>::size() const {
    return _size;
}
template <typename T>
ArrayView<T> Array<T>::createView() {
    return ArrayView<T>( data, _size );
}

//////////////////////////////////////////////////////////////////////////
// ArrayView implementation
template <typename T>
ArrayView<T>::ArrayView( T* data, uint32 size ) {
    this->data = data;
    _size = size;
}

template <typename T>
ArrayView<T>::~ArrayView() {
    this->data = nullptr;
    this->_size = 0;
}

template <typename T>
uint32 ArrayView<T>::length() const {
    return _size;
}

template <typename T>
ArrayView<T>::ArrayView( const ArrayView<T>& other ) {
    this->data = other.data;
    this->_size = other._size;
}

template <typename T>
ArrayView<T>& ArrayView<T>::operator=( const ArrayView<T>& other ) {
    this->data = other.data;
    this->_size = other._size;

    return *this;
}

//////////////////////////////////////////////////////////////////////////
// StaticArray implementation

template <typename T, uint32 count>
StaticArray<T, count>::StaticArray() : BaseArrayView() {
    _size = count;
}

template <typename T, uint32 count>
StaticArray<T, count>::~StaticArray() {}

template <typename T, uint32 count>
uint32 StaticArray<T, count>::maxSize() const {
    return count;
}

template <typename T, uint32 count>
ArrayView<T> StaticArray<T, count>::createView() {
    ArrayView<T> av( begin(), maxSize() );
    return av;
}
