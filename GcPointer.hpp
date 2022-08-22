#pragma once

#include "common_macros.h"

template<class T>
class GcPointer final {
public:
    GcPointer() noexcept {
        setRawPointerNull();
    }

    GcPointer(const GcPointer& rhs) noexcept {
        this->rawPointer = rhs.rawPointer;
        ensureRawPointerInMemory();
    }

    GcPointer(GcPointer&& rhs) noexcept {
        this->rawPointer = rhs.rawPointer;
        rhs.rawPointer = nullptr;
        ensureRawPointerInMemory();
        rhs.ensureRawPointerInMemory();
    }

    explicit GcPointer(T* rhs) noexcept {
        this->rawPointer = rhs;
        ensureRawPointerInMemory();
    }

    GcPointer& operator=(const GcPointer& rhs) noexcept {
        if (this == &rhs) {
            return *this;
        }
        this->rawPointer = rhs.rawPointer;
        ensureRawPointerInMemory();
        return *this;
    }

    GcPointer& operator=(GcPointer&& rhs) noexcept {
        this->rawPointer = rhs.rawPointer;
        rhs.rawPointer = nullptr;
        ensureRawPointerInMemory();
        rhs.ensureRawPointerInMemory();
        return *this;
    }

    GcPointer& operator=(T* rhs) noexcept {
        this->rawPointer = rhs;
        ensureRawPointerInMemory();
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
        rawPointer = nullptr;
        ensureRawPointerInMemory();
    }

    void ensureRawPointerInMemory() const noexcept {
        __asm__ __volatile__ ("" : : "m"(rawPointer));
    }
};

template<class T, class... Args>
static GcPointer<T> GcNew(const Args&... args)
{
    return GcPointer<T>{new T { COMMON_MACROS_helper_forward<Args>(args)... }};
}
