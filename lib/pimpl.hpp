#ifndef NANOM_PIMPL
#define NANOM_PIMPL
/* Based on GotW #101
 */

#include <memory>

using namespace std;

template<typename T>
class Pimpl {
    private:
        unique_ptr<T> m;
    public:
        Pimpl();
        template<typename ...Args>
        Pimpl( Args&& ... );

        ~Pimpl();
        T* operator->() const;
        T& operator*() const;
};

#endif

