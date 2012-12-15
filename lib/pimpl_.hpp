#ifndef NANOM_PIMPL_
#define NANOM_PIMPL_

#include <utility>

#ifndef NANOM_PIMPL
    #include "pimpl.hpp"
#endif

using namespace std;

template<typename T>
Pimpl<T>::Pimpl() : m{ new T{} } {
}

template<typename T>
template<typename ...Args>
Pimpl<T>::Pimpl( Args&& ...args )
        : m{ new T{ forward<Args>(args)... } } {
}

template<typename T>
Pimpl<T>::~Pimpl() { }

template<typename T>
T* Pimpl<T>::operator->() const { return m.get(); }

template<typename T>
T& Pimpl<T>::operator*() const { return *m.get(); }


#endif

