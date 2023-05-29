#pragma once
#include <concepts>
#include <functional>
#include <boost/callable_traits.hpp>

namespace lambda_ptr
{
    namespace detail
    {
        template <typename T>
        concept Assignable = requires(T a, T b) { a = b; };

        template <typename T>
        concept IsFunction = requires(T t) { []<typename R, typename... A>(std::function<R(A...)> &) {}(t); };

        template <typename T>
        concept CaptureLambda = !IsFunction<T> && !Assignable<T> && requires(T t) { &T::operator(); };

        template <typename T, typename... A>
        concept GenericCaptureLambda = !IsFunction<T> && !Assignable<T> && std::invocable<T, A...>;
    } // namespace detail

    template <typename Lambda>
        requires detail::CaptureLambda<Lambda>
    auto pointer_to(Lambda &&lambda)
    {
        static auto static_lambda = std::forward<Lambda>(lambda);
        using args_t = boost::callable_traits::args_t<Lambda>;

        return []<typename... T>(std::tuple<T...>) consteval {
            return [](T... args) { return static_lambda(std::forward<T>(args)...); };
        }(args_t{});
    }

    template <typename R, typename... T, typename Lambda>
        requires detail::GenericCaptureLambda<Lambda, T...>
    auto pointer_to(Lambda &&lambda)
    {
        static auto static_lambda = std::forward<Lambda>(lambda);
        return [](T... args) -> R { return static_lambda(std::forward<T>(args)...); };
    }
} // namespace lambda_ptr