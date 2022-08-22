#pragma once

#include <utility>

template<class T>
class GcPointer final {
public:
    GcPointer() noexcept {
        setRawPointerNull();
    }

    GcPointer(const GcPointer& rhs) noexcept {
        __asm__ __volatile__ (
            "mov\t%1, %0"
            : "=m"(rawPointer)
            : "r"(rhs.rawPointer)
        );
    }

    explicit GcPointer(T* rhs) noexcept {
        __asm__ __volatile__ (
            "mov\t%1, %0"
            : "=m"(rawPointer)
            : "r"(rhs)
        );
    }

    GcPointer& operator=(const GcPointer& rhs) noexcept {
        __asm__ __volatile__ (
            "mov\t%1, %0"
            : "=m"(rawPointer)
            : "r"(rhs.rawPointer)
        );
        return *this;
    }

    GcPointer& operator=(T* rhs) noexcept {
        __asm__ __volatile__ (
            "mov\t%1, %0"
            : "=m"(rawPointer)
            : "r"(rhs)
        );
        return *this;
    }

    ~GcPointer() noexcept {
        setRawPointerNull();
    }

    T* getRawPointer() const noexcept {
        return this->rawPointer;
    }

    T* operator->() const noexcept {
        return this->rawPointer;
    }
private:
    T* rawPointer;

    void setRawPointerNull() noexcept {
        if (sizeof(rawPointer) == 4) {
            __asm__ __volatile__ (
                "movl\t%1, %0"
                : "=m"(rawPointer)
                : "ri"(0)
            );
        } else if (sizeof(rawPointer) == 8) {
            __asm__ __volatile__ (
                "movq\t%1, %0"
                : "=m"(rawPointer)
                : "ri"(0LL)
            );
        } else {
            __builtin_abort();
        }
    }
};

template<class T, class... Args>
static GcPointer<T> GcNew(const Args&... args)
{
    T* p = new T { std::forward<Args>(args)... };
    return GcPointer<T>{p};
}
