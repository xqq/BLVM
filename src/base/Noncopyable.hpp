#ifndef _BASE_NONCOPYABLE_HPP
#define _BASE_NONCOPYABLE_HPP

namespace base {

    class Noncopyable {
    public:
        Noncopyable() = default;
        ~Noncopyable() = default;
    private:
        Noncopyable(const Noncopyable&) = delete;
        Noncopyable& operator=(const Noncopyable&) = delete;
    };

#ifndef DISALLOW_COPY_AND_ASSIGN
    #define DISALLOW_COPY_AND_ASSIGN(TypeName) \
        TypeName(const TypeName&) = delete;    \
        TypeName& operator=(const TypeName&) = delete;
#endif

}

#endif // _BASE_NONCOPYABLE_HPP
