#pragma once
#include <concepts>
#include <functional>
#include <boost/callable_traits.hpp>

namespace lambda
{
    namespace detail
    {
        template <typename T>
        concept is_function = requires(T t) {
            []<typename R, typename... A>(std::function<R(A...)> &) {
            }(t);
        };

        template <typename Lambda, typename Return, typename Args>
        concept invocable_lambda = not is_function<Lambda> and not std::is_copy_assignable_v<Lambda> and //
                                   requires(Args args) {
                                       []<typename... T>(std::tuple<T...> &)
                                           requires std::same_as<std::invoke_result_t<Lambda, T...>, Return>
                                       {
                                       }(args);
                                   };

        template <typename Lambda>
        concept is_lambda = invocable_lambda<Lambda, boost::callable_traits::return_type_t<Lambda>,
                                             boost::callable_traits::args_t<Lambda>>;

        template <typename... T>
        struct user_data : std::tuple<T...>
        {
            using std::tuple<T...>::tuple;

            // NOLINTNEXTLINE(google-runtime-operator)
            void *operator&()
            {
                return reinterpret_cast<void *>(this);
            }

            static std::tuple<T...> &from(void *data)
            {
                return *reinterpret_cast<user_data *>(data);
            }
        };
    } // namespace detail

    template <typename Return, typename Args, typename Lambda>
        requires detail::invocable_lambda<Lambda, Return, Args>
    auto pointer_to(Lambda &&lambda)
    {
        static auto static_lambda = std::forward<Lambda>(lambda);

        return []<typename... T>(const std::tuple<T...> &) consteval {
            return [](T... args) -> Return {
                return static_lambda(std::forward<T>(args)...);
            };
        }(Args{});
    }

    template <typename Signature, typename Lambda>
        requires detail::invocable_lambda<Lambda, boost::callable_traits::return_type_t<Signature>,
                                          boost::callable_traits::args_t<Signature>>
    auto pointer_to(Lambda &&lambda)
    {
        return pointer_to<boost::callable_traits::return_type_t<Signature>, boost::callable_traits::args_t<Signature>,
                          Lambda>(std::forward<Lambda>(lambda));
    }

    template <typename Lambda>
        requires detail::is_lambda<Lambda>
    auto pointer_to(Lambda &&lambda)
    {
        return pointer_to<Lambda, Lambda>(std::forward<Lambda>(lambda));
    }

    template <typename... T>
    auto user_data(T &&...args)
    {
        return detail::user_data<T...>{std::forward<T>(args)...};
    }
} // namespace lambda