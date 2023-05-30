#pragma once
#include <concepts>
#include <functional>
#include <boost/callable_traits.hpp>

namespace lambda_ptr
{
    namespace detail
    {
        template <typename T> using pointer_if_not = std::conditional_t<std::is_pointer_v<T>, T, std::add_pointer_t<T>>;

        template <typename T>
        concept assignable = requires(T a, T b) { a = b; };

        template <typename T>
        concept is_function = requires(T t) { []<typename R, typename... A>(std::function<R(A...)> &) {}(t); };

        template <typename T>
        concept capture_lambda = !is_function<T> && !assignable<T> && requires(T t) { &T::operator(); };

        template <typename T, typename... A>
        concept generic_lambda = !is_function<T> && !assignable<T> && std::invocable<T, A...>;

        template <typename Signature, typename Lambda>
        concept signature_lambda = requires(pointer_if_not<Signature> signature) {
            []<typename R, typename... A>(R(*)(A...))
                requires generic_lambda<Lambda, A...>
            {
            }
            (signature);
        };
    } // namespace detail

    template <typename Lambda>
        requires detail::capture_lambda<Lambda>
    auto pointer_to(Lambda &&lambda)
    {
        static auto static_lambda = std::forward<Lambda>(lambda);
        using args_t = boost::callable_traits::args_t<Lambda>;

        return []<typename... T>(std::tuple<T...>) consteval {
            return [](T... args) { return static_lambda(std::forward<T>(args)...); };
        }(args_t{});
    }

    template <typename R, typename... T, typename Lambda>
        requires detail::generic_lambda<Lambda, T...>
    auto pointer_to(Lambda &&lambda)
    {
        static auto static_lambda = std::forward<Lambda>(lambda);
        return [](T... args) -> R { return static_lambda(std::forward<T>(args)...); };
    }

    template <typename Signature, typename Lambda>
        requires detail::signature_lambda<Signature, Lambda>
    auto pointer_to(Lambda &&lambda)
    {
        static auto static_lambda = std::forward<Lambda>(lambda);
        return []<typename R, typename... T>(R (*)(T...)) consteval {
            return [](T... args) -> R { return static_lambda(std::forward<T>(args)...); };
        }(detail::pointer_if_not<Signature>{});
    }
} // namespace lambda_ptr