#pragma once

template <typename T>
using Opt = std::optional<T>;

template <typename T>
using Ref = std::reference_wrapper<T>;

template <typename T>
using SharedPtr = std::shared_ptr<T>;

template <typename T>
using UniquePtr = std::unique_ptr<T>;

template <typename T, typename ...Args>
inline SharedPtr<T> MakeShared(Args&& ...args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template <typename T, typename ...Args>
inline UniquePtr<T> MakeUnique(Args&& ...args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename Derived, typename Base>
inline bool IsInstance(const SharedPtr<Base>& ptr) {
    if (!ptr)
        return false;
    return std::dynamic_pointer_cast<Derived>(ptr) != nullptr;
}

template <typename Derived, typename Base>
inline bool IsInstance(const UniquePtr<Base>& ptr) {
    if (!ptr)
        return false;
    return dynamic_cast<const Derived*>(ptr.get()) != nullptr;
}

template <typename Derived, typename Base>
inline SharedPtr<Derived> CastSharedPtr(const SharedPtr<Base>& ptr) {
    return std::dynamic_pointer_cast<Derived>(ptr);
}

template <typename Derived, typename Base>
inline Ref<Derived> CastRef(const Base& ptr) {
	return std::dynamic_pointer_cast<Derived>(ptr);
}

template <typename Derived, typename Base>
Derived& CastRef(Base& ref) {
    return dynamic_cast<Derived&>(ref);
}