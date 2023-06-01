#pragma once
#include <tuple>
#include <memory>

namespace lambda
{
    namespace detail
    {
        template <typename... T>
        struct user_data : std::tuple<T...>
        {
            using std::tuple<T...>::tuple;

          public:
            [[nodiscard]] auto &tuple()
            {
                return *static_cast<std::tuple<T...> *>(this);
            }

            [[nodiscard]] auto *detach()
            {
                return new user_data(std::move(*this));
            }

            [[nodiscard]] void *ptr()
            {
                return reinterpret_cast<void *>(this);
            }

          public:
            operator void *()
            {
                return ptr();
            }

          public:
            [[nodiscard]] static auto from_detached(void *data)
            {
                return std::unique_ptr<user_data>(reinterpret_cast<user_data *>(data));
            }

            [[nodiscard]] static auto &from(void *data)
            {
                return *reinterpret_cast<user_data *>(data);
            }
        };
    } // namespace detail

    template <typename... T>
    [[nodiscard]] auto user_data(T &&...args)
    {
        return detail::user_data<T...>{std::forward<T>(args)...};
    }
} // namespace lambda