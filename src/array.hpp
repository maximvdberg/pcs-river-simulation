#pragma once

#include <functional>
#include <cmath>

#include "print.hpp"



constexpr unsigned getIndex( const int value, const int modulus ) {
    int mod = value % modulus;
    return mod + (mod >> (sizeof (int) - 1) & modulus);
}

inline unsigned getIndex( const int value, const unsigned modulus ) {
    int mod = value % (int) modulus;
    if (mod < 0) {
        return mod + (int) modulus;
    }
    return mod;
}

/**
    * Check if two ArrayBase instances are compatiable in size. Of course
    * this can only be done if the two implementations both have constant
    * expression sizes.
    */
template<typename Impl1, typename Impl2, typename = void>
struct hasCompatibleSizeImpl : std::true_type {
};
template<typename Impl1, typename Impl2>
struct hasCompatibleSizeImpl<Impl1, Impl2,
        std::enable_if_t<is_constexpr(Impl1::size) &&
                            is_constexpr(Impl2::size)>> :
    std::integral_constant<bool, Impl1::size() == Impl2::size()> {
};
template<typename Impl1, typename Impl2>
constexpr bool hasCompatibleSize = hasCompatibleSizeImpl<Impl1, Impl2>::value;



template<typename Impl>
struct ArrayBase : Impl {
    using Impl::size;
    using Impl::length;
    using BaseType = typename Impl::BaseType;

    using Impl::get;
    using Impl::at;

    template<typename T>
    using TypeConv = typename Impl::template TypeConv<T>;

    // Use the constructors.
    using Impl::Impl;

    // Scalar init.
    //constexpr ArrayBase( const BaseType& a ) {
    //    const unsigned s = size();
    //    for (unsigned i = 0; i < s; ++i) {
    //        get(i) = a;
    //    }
    //}

    // Overload the operators [] and () to use the implemations at function.
    template<typename ...Args>
    decltype((*(Impl*)(0)).at( *(Args*)(0)... ))
    constexpr inline operator[]( const Args&... args ) {
        return at(args...);
    }
    template<typename ...Args> decltype((*(const Impl*)(0)).at( *(Args*)(0)... ))
    constexpr inline operator[]( const Args&... args ) const {
        return at(args...);
    }
    template<typename ...Args> decltype((*(Impl*)(0)).at( *(Args*)(0)... ))
    constexpr inline operator()( const Args&... args ) {
        return at(args...);
    }
    template<typename ...Args> decltype((*(const Impl*)(0)).at( *(Args*)(0)... ))
    constexpr inline operator()( const Args&... args ) const {
        return at(args...);
    }


    // Element-wise operators.
    #define OPRT(OPRTR) \
        template<typename Impl2 = Impl, \
            typename R = decltype (*(BaseType*)(0) OPRTR *(typename Impl2::BaseType*)(0)), \
            typename = std::enable_if_t<hasCompatibleSize<Impl, Impl2>>> \
        constexpr TypeConv<R> operator \
        OPRTR( const ArrayBase<Impl2>& other ) const { \
            TypeConv<R> a; \
            for (unsigned i = 0; i < length(); ++i) \
                a.at(i) = at(i) OPRTR other.at(i); \
            return a; \
        } \
        template<typename B, typename R = decltype (\
            *(BaseType*)(0) OPRTR *(B*)(0))> \
        constexpr TypeConv<R> operator OPRTR( const B& other ) const { \
            TypeConv<R> a; \
            for (unsigned i = 0; i < length(); ++i) \
                a.at(i) = at(i) OPRTR other; \
            return a; \
        } \
        /* Use the operators defined in the implementation Impl. */ \
        template<typename Impl2 = Impl, typename U> \
        decltype((*(const Impl2*)(0)).operator OPRTR( *(U*)(0) ))\
        operator OPRTR( const U& arg ) const { \
            return Impl2::operator OPRTR(arg); \
        }
    OPRT(+)
    OPRT(-)
    OPRT(*)
    OPRT(/)
    OPRT(%)
    OPRT(&)
    OPRT(|)
    OPRT(^)
    OPRT(<<)
    OPRT(>>)
    #undef OPRT


    // Element-wise assignment operators.
    #define OP_EQ(OPRTR) \
        template<typename Impl2 = Impl, \
            typename = decltype (*(BaseType*)(0) OPRTR *(typename Impl2::BaseType*)(0)), \
            typename = std::enable_if_t<hasCompatibleSize<Impl, Impl2>>> \
        constexpr ArrayBase& operator OPRTR( const ArrayBase<Impl2>& other ) { \
            for (unsigned i = 0; i < length(); ++i) \
                at(i) OPRTR other.at(i); \
            return *this; \
        } \
        template<typename B, typename = decltype ( \
            *(BaseType*)(0) OPRTR *(B*)(0))> \
        constexpr  ArrayBase& operator OPRTR( const B& other ) { \
            for (unsigned i = 0; i < length(); ++i) \
                at(i) OPRTR other; \
            return *this; \
        } \
        /* Use the operators defined in the implementation. */ \
        template<typename Impl2 = Impl, typename U> \
        constexpr decltype((*(const Impl2*)(0)).operator OPRTR( *(U*)(0) ))\
        operator OPRTR( const U& arg ) const { \
            return Impl2::operator OPRTR(arg); \
        }
    OP_EQ(=)
    OP_EQ(+=)
    OP_EQ(-=)
    OP_EQ(*=)
    OP_EQ(/=)
    OP_EQ(%=)
    OP_EQ(&=)
    OP_EQ(|=)
    OP_EQ(^=)
    OP_EQ(<<=)
    OP_EQ(>>=)
    #undef OP_EQ


    // Element-wise bool operators.
    #define BL_OPRT(OPRTR) \
        template<typename Impl2 = Impl, \
            typename R = decltype (*(BaseType*)(0) OPRTR *(typename Impl2::BaseType*)(0)), \
            typename = std::enable_if_t<hasCompatibleSize<Impl, Impl2>>> \
        constexpr TypeConv<R> operator \
        OPRTR( const ArrayBase<Impl2>& other ) const { \
            TypeConv<R> b; \
            for (unsigned i = 0; i < length(); ++i) \
                b.at(i) = at(i) OPRTR other.at(i); \
            return b; \
        } \
        template<typename B, typename R = decltype ( \
            *(BaseType*)(0) OPRTR *(B*)(0))> \
        constexpr TypeConv<R> operator OPRTR( const B& other ) const { \
            TypeConv<R> b; \
            for (unsigned i = 0; i < length(); ++i) \
                b.at(i) = at(i) OPRTR other; \
            return b; \
        } \
        /* Use the operators defined in the implementation. */ \
        template<typename Impl2 = Impl, typename U> \
        constexpr decltype((*(const Impl2*)(0)).operator \
        OPRTR( *(U*)(0) )) \
        operator OPRTR( const U& arg ) const { \
            return Impl2::operator OPRTR(arg); \
        }
    BL_OPRT(==)
    BL_OPRT(!=)
    BL_OPRT(>)
    BL_OPRT(<)
    BL_OPRT(>=)
    BL_OPRT(<=)
    BL_OPRT(&&)
    BL_OPRT(||)
    #undef BL_OPRT


    // Unary operators.
    #define UN_OPRT(OPRTR) \
        template<typename T = BaseType, typename R = decltype (OPRTR (*(T*)(0)))> \
        constexpr ArrayBase operator OPRTR() const { \
            ArrayBase a; \
            for (unsigned i = 0; i < length(); ++i) \
                a.at(i) = OPRTR this->at(i); \
            return a; \
        }
    UN_OPRT(-)
    UN_OPRT(+)
    UN_OPRT(!)
    UN_OPRT(~)
    UN_OPRT(++)
    UN_OPRT(--)
    #undef UN_OPRT

    // Increment and decrement operators.
    template<typename T = BaseType, typename = decltype (++(*(T*)(0)))>
    constexpr ArrayBase& operator++() {
        for (unsigned i = 0; i < length(); ++i) { ++at(i); }
        return *this;
    }
    template<typename T = BaseType, typename = decltype ((*(T*)(0))++)>
    constexpr ArrayBase operator++( int ) {
        for (unsigned i = 0; i < length(); ++i) { at(i)++; }
        return *this;
    }
    template<typename T = BaseType, typename = decltype (--(*(T*)(0)))>
    constexpr ArrayBase& operator--() {
        for (unsigned i = 0; i < length(); ++i) { --at(i); }
        return *this;
    }
    template<typename T = BaseType, typename = decltype ((*(T*)(0))--)>
    constexpr ArrayBase operator--( int ) {
        for (unsigned i = 0; i < length(); ++i) { at(i)--; }
        return *this;
    }


    // Any and all functions (you get what I mean).
    /** Any function */
    template<typename T = BaseType, typename R = decltype (*(T*)(0) || *(T*)(0))>
    constexpr std::enable_if_t<!std::is_fundamental<T>::value, R> any() const {
        for (unsigned i = 0; i < length(); ++i) {
            if (this->at(i).any()) return true;
        }
        return false;
    }
    template<typename T = BaseType, typename R = decltype (*(T*)(0) || *(T*)(0))>
    constexpr std::enable_if_t<std::is_fundamental<T>::value, R> any() const {
        for (unsigned i = 0; i < length(); ++i)
            if (this->at(i)) return true;
        return false;
    }

    /** All function */
    template<typename T = BaseType, typename R = decltype (*(T*)(0) && *(T*)(0))>
    constexpr std::enable_if_t<!std::is_fundamental<T>::value, R> all() const {
        for (unsigned i = 0; i < length(); ++i) {
            if (!this->at(i).all()) return false;
        }
        return true;
    }
    template<typename T = BaseType, typename R = decltype (*(T*)(0) && *(T*)(0))>
    constexpr std::enable_if_t<std::is_fundamental<T>::value, R> all() const {
        for (unsigned i = 0; i < length(); ++i)
            if (!this->at(i)) return false;
        return true;
    }

    /**
        * Calculate the sum of the array.
        */
    template<typename T = BaseType, typename R = decltype (*(T*)(0) + *(T*)(0))>
    constexpr R sum() const {
        T total = get(0);
        const unsigned s = size();
        for (unsigned i = 1; i < s; ++i) {
            total += get(i);
        }
        return total;
    }

    template<typename T = BaseType, typename R = decltype (*(T*)(0) * *(T*)(0))>
    constexpr R product( T start ) const {
        const unsigned s = size();
        for (unsigned i = 0; i < s; ++i) {
            start *= get(i);
        }
        return start;
    }



    // Apply a function to the whole array.
    template<typename T> TypeConv<T>
    map( const std::function<T(BaseType)>& function ) const {
        TypeConv<T> a;
        const unsigned s = size();
        for (unsigned i = 0; i < s; ++i)
            a.get(i) = function(get(i));
        return a;
    }

    // Convert the type of the array.
    template<typename T>
    TypeConv<T> asType() const {
        TypeConv<T> a;
        const unsigned s = size();
        for (unsigned i = 0; i < s; ++i)
            a.get(i) = static_cast<T>(get(i));
        return a;
    }


//        // Scalar convert operator.
//        template<typename T = BaseType,
//                 typename = std::enable_if_t<(!is_constexpr(size) ||
//                    size() == 1) && std::is_same<T, BaseType>::value>>
//        operator BaseType&() {
//            return get(0);
//        }

    template<typename ArrayPointer, int step = 1>
    struct base_iterator {
        base_iterator( ArrayPointer pointer, int i = 0 )
            : pointer(pointer), i(i) {}
        base_iterator& operator++() { i += step; return *this; }
        base_iterator& operator--() { i -= step; return *this; }
        base_iterator operator++( int ) { return base_iterator(this, pointer++); }
        base_iterator operator--( int ) { return base_iterator(this, pointer--); }
        base_iterator operator+=( unsigned i ) { i += i; return *this; }
        base_iterator operator-=( unsigned i ) { i -= i; return *this; }
        base_iterator operator+( unsigned i ) const {
            base_iterator n(pointer);
            return n += i;
        }
        base_iterator operator-( unsigned i ) const {
            base_iterator n(pointer);
            return n -= i;
        }
        bool operator==( const base_iterator& other ) const {
            return i == other.i && pointer == other.pointer;
        }
        bool operator!=( const base_iterator& other ) const {
            return !(operator==(other));
        }
        decltype((*(ArrayPointer)(0)).at(0)) operator*() const {
            return pointer->at(i);
        }
    private:
        ArrayPointer pointer;
        unsigned i;
    };

    typedef base_iterator<ArrayBase*> iterator;
    typedef base_iterator<const ArrayBase*> const_iterator;

    inline iterator begin() { return iterator(this, 0); }
    inline iterator end() { return iterator(this, length()); }
    inline const_iterator begin() const { return const_iterator(this, 0); }
    inline const_iterator end() const { return const_iterator(this, length()); }



    /**
        * TODO: check if this actually works :O.
        * @brief fill
        * @param value
        */
    ArrayBase fill( const BaseType& value ) {
        const unsigned s = size();
        for (unsigned i = 0; i < s; ++i) {
            get(i) = value;
        }
        return *this;
    }
};



//////// Stack array definitions ////////

template<typename BaseType, unsigned Head, unsigned ...Tail>
struct __StackArray;

template<typename Type, unsigned head, unsigned ...Tail>
struct TypeGetter {
    using type = ArrayBase<__StackArray<Type, Tail...>>;
};
template<typename Type, unsigned head>
struct TypeGetter<Type, head> {
    using type = Type;
};

template<typename T, unsigned Head, unsigned ...Tail>
struct CArray {
    using type = typename CArray<T, Tail...>::type[Head];
};
template<typename T, unsigned Head>
struct CArray<T, Head> {
    using type = T[Head];
};


// Stack array constructors.



template<unsigned... Unsigneds>
constexpr unsigned uintProduct() {
    unsigned p = 1;
    unsigned arr[] = {Unsigneds...};
    for (unsigned a : arr)
        p *= a;
    return p;
}

/**
    * The stack array Array implementation.
    */
template<typename T, unsigned Head, unsigned ...Tail>
struct __StackArray {
    using BaseType = T;
    static constexpr unsigned length() { return Head; }
    static constexpr unsigned depth() { return sizeof...(Tail) + 1; }
    static constexpr unsigned size() {
        return Head * uintProduct<Tail...>();
    }
    static constexpr ArrayBase<__StackArray<unsigned, depth()>> shape() {
        return {{Head, Tail...}};
    }
    template<typename U>
    using TypeConv = ArrayBase<__StackArray<U, Head, Tail...>>;

    // Constructors
    constexpr __StackArray() {}

    // C-array init.
    constexpr __StackArray( const typename
                        CArray<BaseType, Head, Tail...>::type& a ) {
        unsigned s = size();
        for (unsigned i = 0; i < s; ++i) {
            get(i) = ((const BaseType*) a)[i];
        }
    }

    // Scalar init.
    template<typename U = T, typename = std::enable_if_t<
                    std::is_same<U, T>::value && Head != 1>>
    constexpr __StackArray( const BaseType& a ) {
        for (unsigned i = 0; i < length(); ++i) {
            this->at(i) = a;
        }
    }

    using Type = typename TypeGetter<BaseType, Head, Tail...>::type;

    constexpr inline BaseType& get( const unsigned& i ) {
        return ((BaseType*) this)[i]; }
    constexpr inline const BaseType& get( const unsigned& i ) const {
        return ((BaseType*) this)[i]; }
    constexpr inline Type& at( const int i ) {
        return this->arr[getIndex(i, Head)]; }
    constexpr inline const Type& at( const int i ) const {
        return this->arr[getIndex(i, Head)]; }

    constexpr inline Type& at( const unsigned i ) {
        return this->arr[i]; }
    constexpr inline const Type& at( const unsigned i ) const {
        return this->arr[i]; }


    // Repeat an array along a new axis.
    template<unsigned ...New>
    ArrayBase<__StackArray<BaseType, New..., Head, Tail...>>&& repeat() const {
        ArrayBase<__StackArray<BaseType, New..., Head, Tail...>> a;
        const unsigned s = size(), as = a.size();
        for (unsigned i = 0; i < as; ++i) {
            a.get(i) = get(i % s);
        }
        return a;
    }

    inline BaseType* c_arr() {
        return (BaseType*) this;
    }
    inline const BaseType* c_arr() const {
        return (const BaseType*) this;
    }

    typename TypeGetter<BaseType, Head, Tail...>::type arr[Head];
};

template<typename BaseType, unsigned Head, unsigned ...Tail>
using Array = ArrayBase<__StackArray<BaseType, Head, Tail...>>;




//////// Shared array definitions ////////

template<typename T, int Depth = 1>
struct __HeapArray {
    using BaseType = T;
    static constexpr unsigned depth() {
        return Depth;
    }
    const unsigned& length() const {
        return _shape[0];
    }

    template<typename U = unsigned>
    const std::enable_if_t<Depth == 1 && std::is_same<U, U>::value, U>& size() const {
        return _shape[0];
    }
    template<typename U = unsigned>
    std::enable_if_t<Depth >= 2 && std::is_same<U, U>::value, U> size() const {
        unsigned s = 1;
        for (unsigned i = 0; i < Depth; ++i)
            s *= _shape[i];
        return s;
    }

    constexpr ArrayBase<__StackArray<unsigned, depth()>> shape() const {
        return _shape;
        // TODO: maybe use this?
        // return *reinterpret_cast<const ArrayBase<__StackArray<unsigned, depth()>>*>(_shape);
    }

    const unsigned* c_shape() const {
        return _shape;
    }

    template<typename U>
    using TypeConv = ArrayBase<__HeapArray<U, Depth>>;

    template<typename Impl>
    using IfIndices = std::enable_if_t<Impl::size() == Depth &&
        std::is_convertible<typename Impl::BaseType, int>::value>;


    // Constructors. //
    // Default (empty) constructor).
    __HeapArray() {
        for (unsigned i = 0; i < Depth; ++i)
            _shape[i] = 0;
        _arr = nullptr;
    };
    inline __HeapArray( decltype (nullptr) ) : __HeapArray() {};

    // Copy a raw pointer.
    template<typename Impl = __StackArray<unsigned, Depth>, typename = IfIndices<Impl>>
    __HeapArray( BaseType* ptr, const ArrayBase<Impl>& shape_ )
        : _arr(ptr) {
        for (unsigned i = 0; i < Depth; ++i)
            _shape[i] = shape_.get(i);
    };

    // Use a stack Array shape.
    template<typename Impl = __StackArray<unsigned, Depth>, typename = IfIndices<Impl>>
    __HeapArray( ArrayBase<Impl> shape ) {
        for (unsigned i = 0; i < Depth; ++i) {
            _shape[i] = shape.get(i);
        }
        _arr = new BaseType[size()];
    }

    // Init the array with a argument pack of unsigneds.
    template<typename ...Args, typename = std::enable_if_t<sizeof...(Args) == Depth>,
                typename = decltype( void_f(static_cast<unsigned>(*(Args*)(0))...) )>
    __HeapArray( Args... shape ) {
        unsigned newshape[] = {static_cast<unsigned>(shape)...};
        for (unsigned i = 0; i < Depth; ++i)
            _shape[i] = newshape[i];
        _arr = new BaseType[size()];
    }

    // Delete the copy constructor.
    __HeapArray( const __HeapArray& other ) = delete;

    // Move constructor.
    __HeapArray( __HeapArray&& other ) noexcept {
        _arr = other._arr;
        other._arr = nullptr;
        for (unsigned i = 0; i < Depth; ++i) {
            _shape[i] = other._shape[i];
            // other._shape[i] = 0;
        }
    }

    // Destructor.
    ~__HeapArray() {
        delete [] _arr;
    }


    // Move assignment constructor.
    __HeapArray& operator=( __HeapArray&& other ) noexcept {
        delete [] _arr;
        _arr = other._arr;
        for (unsigned i = 0; i < Depth; ++i) {
            _shape[i] = other._shape[i];
        }
        other._arr = nullptr;
        return *this;
    }

    // Set to null.
    __HeapArray& operator=( decltype(nullptr) ) noexcept {
        reset();
        return *this;
    }

    // TODO These operators don't work :/.
    bool operator==( decltype(nullptr) ) const { return _arr == nullptr; }
    bool operator!=( decltype(nullptr) ) const { return _arr == nullptr; }


    // Get functions.
    inline BaseType& get( unsigned i ) { return _arr[i]; }
    inline const BaseType& get( unsigned i ) const { return _arr[i]; }


    // At functions.
    template<typename Impl = __StackArray<int, Depth>, typename = IfIndices<Impl>>
    BaseType& at( const ArrayBase<Impl>& indices ) {
        unsigned index = getIndex(indices.get(Depth - 1), _shape[Depth - 1]);
        for (unsigned i = 0; i < Depth - 1; ++i) {
            unsigned c = getIndex(indices.get(i), _shape[i]);
            for (unsigned j = i + 1; j < Depth; j++) {
                c *= _shape[j];
            }
            index += c;
        }
        return _arr[index];
    }
    template<typename Impl = __StackArray<int, Depth>, typename = IfIndices<Impl>>
    const BaseType& at( const ArrayBase<Impl>& indices ) const {
        unsigned index = getIndex(indices.get(Depth - 1), _shape[Depth - 1]);
        index = getIndex(index, _shape[Depth - 1]);
        for (unsigned i = 0; i < Depth - 1; ++i) {
            unsigned c = getIndex(indices.get(i), _shape[i]);
            for (unsigned j = i + 1; j < Depth; j++) {
                c *= _shape[j];
            }
            index += c;
        }
        return _arr[index];
    }


    template<typename ...Args,
                typename = std::enable_if_t<sizeof... (Args) == Depth>>
    inline BaseType& at( Args... args ) {
        return at({static_cast<int>(args)...});
    }
    template<typename ...Args,
                typename = std::enable_if_t<sizeof... (Args) == Depth>>
    inline const BaseType& at( Args... args ) const {
        return at({static_cast<int>(args)...});
    }


    /**
        * @brief copy
        * @return
        */
    template<typename R = ArrayBase<__HeapArray>>
    std::enable_if_t<std::is_trivially_copyable<BaseType>::value, R> copy() const {
        R narr({_shape});
        for (unsigned i = 0; i < Depth; ++i)
            narr._shape[i] = _shape[i];
        narr._arr = new BaseType[size()];
        memcpy(narr._arr, _arr, sizeof (BaseType) * size());
        return narr;
    }
    template<typename R = ArrayBase<__HeapArray>>
    std::enable_if_t<!std::is_trivially_copyable<BaseType>::value, R> copy() const {
        R narr({_shape});
        const unsigned s = size();
        for (unsigned i = 0; i < s; ++i)
            narr.get(i) = get(i);
        return narr;
    }

    /**
        * Release the pointer from the HeapArray's grasp, so that it
        * won't delete it when the array get's deconstructed.
        * @return The released pointer.
        */
    BaseType* release() {
        BaseType* ptr = _arr;
        _arr = nullptr;
        // for (unsigned i = 0; i < Depth; ++i) _shape[i] = 0;
        return ptr;
    }


    /**
        * Delete the current array and create a new
        * one in it's place, of the same size.
        */
    void reallocate() {
        __HeapArray::~__HeapArray();
        unsigned s = size();
        if (s != 0) {
            _arr = new BaseType[s];
        }
    }

    /**
        * @brief reset
        * @param init
        */
    template<typename ...Args,
                typename = decltype (__HeapArray(*(Args*)(0)...))>
    void reset( Args&&... args ) {
        // Call the destructor to free the memory, and then
        // call a constructor again.
        __HeapArray::~__HeapArray();
        new (this) __HeapArray(std::forward<Args>(args)...);
    }


    inline BaseType* c_arr() { return _arr; }
    inline const BaseType* c_arr() const { return _arr; }

private:
    BaseType* _arr;
    unsigned _shape[Depth];



    template<typename Impl, typename R>
    friend R&& make_array( const ArrayBase<Impl>& );
};

/**
    * The shared array Array implementation.
    */

template<typename BaseType, unsigned Depth = 1>
using HeapArray = ArrayBase<__HeapArray<BaseType, Depth>>;



template<typename BaseType, unsigned Depth>
struct __make_array_init {
    using type = std::initializer_list<
        typename __make_array_init<BaseType, Depth - 1>::type>;
};

template<typename BaseType>
struct __make_array_init<BaseType, 0> {
    using type = BaseType;
};

template<typename Iterable,
            typename N = decltype((*(Iterable*)(0))[0])>
constexpr N get_at_depth( const Iterable& iter, unsigned depth) {
    if (depth == 0) return iter;
    else return get_at_depth(iter, depth - 1)[0];
}


//    template<typename BaseType, unsigned Depth>
//    HeapArray<BaseType, Depth> make_array(
//            const typename __make_array_init<BaseType, Depth>::type& init ) {

//        HeapArray<BaseType, Depth> arr;
//        for (unsigned i = 0; i < Depth; ++i) {
//            arr._shape[i] = get_at_depth(init, i).size();
//        }

//        arr.reallocate();

//        return arr;
//    }

template<typename Impl,
            typename R = HeapArray<typename Impl::BaseType, Impl::depth()>>
R&& make_array( const ArrayBase<Impl>& init ) {
    R arr;
    for (unsigned i = 0; i < R::depth(); ++i) {
        arr._shape[i] = init.shape()[i];
    }
    arr.reallocate(); // Allocate the memory.
    arr = init; // Copy the array.
    return std::forward<R>(arr);
}



template<typename ...Args>
constexpr inline Array<std::common_type_t<Args...>, sizeof...(Args)>
a_( Args&&... args ) {
    return {{static_cast<std::common_type_t<Args...>>(std::forward<Args>(args))...}};
}





///// Overload std (cmath) functions, that will work on any Array struct: /////

#define UNARY_FUNC(FUNC) \
template<typename Impl, typename R = decltype (\
    std::FUNC(*(typename Impl::BaseType*)(0)))> \
constexpr typename Impl::template TypeConv<R> \
FUNC( const ArrayBase<Impl>& x  ) { \
    typename Impl::template TypeConv<R> a; \
    const unsigned s = a.size(); \
    for (unsigned i = 0; i < s; ++i) a.get(i) = FUNC(x.get(i)); \
    return a; \
}

#define BINARY_FUNC(FUNC) \
template<typename Impl1, typename Impl2 = Impl1, typename R = decltype ( \
    std::FUNC(*(typename Impl1::BaseType*)(0), \
                *(typename Impl2::BaseType*)(0))), \
    typename = std::enable_if_t<hasCompatibleSize<Impl1, Impl2>>> \
constexpr typename Impl1::template TypeConv<R> \
FUNC( const ArrayBase<Impl1>& x, \
        const ArrayBase<Impl2>& y ) { \
    typename Impl1::template TypeConv<R> a; \
    const unsigned s = a.size(); \
    for (unsigned i = 0; i < s; ++i) \
        a.get(i) = FUNC(x.get(i), y.get(i)); \
    return a; \
} \
template<typename Impl, typename U, typename R = decltype ( \
    std::FUNC(*(typename Impl::BaseType*)(0), *(U*)(0)))> \
constexpr typename Impl::template TypeConv<R> \
FUNC( const ArrayBase<Impl>& x, U y ) { \
    typename Impl::template TypeConv<R> a; \
    const unsigned s = a.size(); \
    for (unsigned i = 0; i < s; ++i) \
        a.get(i) = FUNC(x.get(i), y); \
    return a; \
}


namespace std {

    // cmath (math.h) overloads. //

    // Trigonometric functions.
    UNARY_FUNC(cos)
    UNARY_FUNC(sin)
    UNARY_FUNC(tan)
    UNARY_FUNC(acos)
    UNARY_FUNC(asin)
    UNARY_FUNC(atan)
    BINARY_FUNC(atan2)

    // Hyperbolic functions.
    UNARY_FUNC(cosh)
    UNARY_FUNC(sinh)
    UNARY_FUNC(tanh)
    UNARY_FUNC(acosh)
    UNARY_FUNC(asinh)
    UNARY_FUNC(atanh)

    // Exponential and logarithmic functions.
    UNARY_FUNC(exp)
    // BINARY_FUNC(frexp)
    BINARY_FUNC(ldexp)
    UNARY_FUNC(log)
    UNARY_FUNC(log10)
    // BINARY_FUNC(modf)
    UNARY_FUNC(exp2)
    UNARY_FUNC(expm1)
    UNARY_FUNC(ilogb)
    UNARY_FUNC(log1p)
    UNARY_FUNC(log2)
    UNARY_FUNC(logb)
    BINARY_FUNC(scalbn)
    BINARY_FUNC(scalbln)

    // Power functions.
    BINARY_FUNC(pow)
    UNARY_FUNC(sqrt)
    UNARY_FUNC(cbrt)
    BINARY_FUNC(hypot)

    // Error and gamma functions.
    UNARY_FUNC(erf)
    UNARY_FUNC(erfc)
    UNARY_FUNC(tgamma)
    UNARY_FUNC(lgamma)

    // Rounding and remainder functions.
    UNARY_FUNC(ceil)
    UNARY_FUNC(floor)
    BINARY_FUNC(fmod)
    UNARY_FUNC(trunc)
    UNARY_FUNC(round)
    UNARY_FUNC(lround)
    UNARY_FUNC(llround)
    UNARY_FUNC(rint)
    UNARY_FUNC(lrint)
    UNARY_FUNC(llrint)
    UNARY_FUNC(nearbyint)
    UNARY_FUNC(remainder)
    // TERTIARY_FUNC(remquo)

    // Floating-point manipulation functions.
    BINARY_FUNC(copysign)
    BINARY_FUNC(nextafter)
    BINARY_FUNC(nexttoward)

    // Minimum, maximum, difference functions.
    BINARY_FUNC(fdim)
    BINARY_FUNC(fmax)
    BINARY_FUNC(fmin)

    // Other functions.
    UNARY_FUNC(fabs)
    UNARY_FUNC(abs)
    // TERTIARY_FUNC(fma)


    // Algorithm overloads. //
    BINARY_FUNC(min)  // Compare argument not yet supported.
    BINARY_FUNC(max)

}

#undef UNARY_FUNC
#undef BINARY_FUNC
