#include <iostream>
#include <type_traits>

namespace ds
{
template <typename T>
struct NullT
{
    //NullT相当于空指针，但是空指针显然也可以有自己的类型
    typedef T ValueType;
};
template <typename T>
struct IsNull //有些返回值不是NullT,而是继承自NullT,所以这里加一个判断
{
    const static bool value = std::is_base_of<NullT<typename T::ValueType>, T>::value;
};

template <typename T, T...>
struct ListV;

template <typename T, T First, T... Items>
struct ListV<T, First, Items...>
{
    typedef ListV<T, Items...> Next;
    const static int value = First;
    typedef T ValueType;
};

template <typename T, T First>
struct ListV<T, First>
{
    typedef NullT<T> Next;
    const static int value = First;
    typedef T ValueType;
};

template <typename List, int Where>
struct Get : Get<typename List::Next, Where - 1>
{
};
template <typename List>
struct Get<List, 0> : List
{
};

template <typename List, bool Null = IsNull<List>::value>
struct Size
{
    const static int value = Size<typename List::Next>::value + 1;
};
template <typename List>
struct Size<List, true>
{
    const static int value = 0;
};

template <typename List, typename List::ValueType Elem>
struct PushFront
{
    typedef List Next;
    const static typename List::ValueType value = Elem;
    typedef typename List::ValueType ValueType;
};

namespace pbimpl
{
template <typename T, typename Iter, T Elem, bool Null = IsNull<Iter>::value>
struct Impl
{
    typedef Impl<T, typename Iter::Next, Elem> Next;
    const static int value = Iter::value;
    typedef T ValueType;
};
template <typename T, typename Iter, T Elem>
struct Impl<T, Iter, Elem, true>
{
    const static T value = Elem;
    typedef NullT<T> Next;
    typedef T ValueType;
};
}
template <typename List, typename List::ValueType Elem>
struct PushBack : pbimpl::Impl<typename List::ValueType, List, Elem>
{
};

namespace mapimpl
{
template <typename T, typename Return, typename Iter, template <T> class Pred, bool Null = IsNull<Iter>::value>
struct Impl : Impl<T, PushBack<Return, Pred<Iter::value>::value>, typename Iter::Next, Pred>
{
};
template <typename T, typename Return, typename Iter, template <T> class Pred>
struct Impl<T, Return, Iter, Pred, true> : Return
{
};
}
template <typename List, template <typename List::ValueType> class Pred>
struct Map : mapimpl::Impl<typename List::ValueType, NullT<typename List::ValueType>, List, Pred>
{
};

namespace filimpl
{
template <typename T, typename Return, typename Iter, template <T> class Pred, bool Null = IsNull<Iter>::value>
struct Impl : Impl<T,
                   typename std::conditional<Pred<Iter::value>::value, PushBack<Return, Iter::value>, Return>::type,
                   typename Iter::Next, Pred>
{
};
template <typename T, typename Return, typename Iter, template <T> class Pred>
struct Impl<T, Return, Iter, Pred, true> : Return
{
};
}
template <typename List, template <typename List::ValueType> class Pred>
struct Filter : filimpl::Impl<typename List::ValueType, NullT<typename List::ValueType>, List, Pred>
{
};

namespace conimpl
{
template <typename L1, typename L2, bool Null = IsNull<L2>::value>
struct Impl : Impl<PushBack<L1, L2::value>, typename L2::Next>
{
};
template <typename L1, typename L2>
struct Impl<L1, L2, true> : L1
{
};
}
template <typename L1, typename L2>
struct Contact : conimpl::Impl<L1, L2>
{
};

namespace qsortimpl
{
template <typename List, bool Null = IsNull<List>::value>
struct Impl
{
    using T = typename List::ValueType;
    const static T pivot = List::value;
    template <T I>
    struct Less
    {
        const static bool value = I < pivot;
    };
    template <T I>
    struct Equal
    {
        const static bool value = I == pivot;
    };
    template <T I>
    struct Greater
    {
        const static bool value = I > pivot;
    };
    using L1 = Filter<List, Less>;
    using L2 = Filter<List, Equal>;
    using L3 = Filter<List, Greater>;
    using Type = Contact<Contact<typename Impl<L1>::Type, L2>, typename Impl<L3>::Type>;
};
template <typename List>
struct Impl<List, true>
{
    using Type = List;
};
}
template <typename List>
struct QSort : qsortimpl::Impl<List>::Type
{
};
}
using namespace std;
using namespace ds;
template <int I>
struct Even
{
    const static bool value = I % 2;
};
template <int I>
struct Add
{
    const static int value = I + 1;
};

int main()
{
    using L0 = ListV<int, 5, 6, -1, 0, 10, -25, 9, 1, 1, 7, 3>;
    using L1 = QSort<L0>;
    cout << Get<L1, 0>::value << endl;
    cout << Get<L1, 1>::value << endl;
    cout << Get<L1, 2>::value << endl;
    cout << Get<L1, 3>::value << endl;
    cout << Get<L1, 4>::value << endl;
}