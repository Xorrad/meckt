#pragma once

template <typename T>
using SharedPtr = std::shared_ptr<T>;

template <typename T>
using UniquePtr = std::unique_ptr<T>;

template <typename T, typename ...Args>
SharedPtr<T> MakeShared(Args&& ...args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template <typename T, typename ...Args>
UniquePtr<T> MakeUnique(Args&& ...args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename Derived, typename Base>
inline bool IsInstance(const SharedPtr<Base>& ptr) {
    return dynamic_cast<Derived*>(ptr.get());
}
