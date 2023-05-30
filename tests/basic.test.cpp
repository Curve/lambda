#include <catch2/catch_test_macros.hpp>
#include <lambda_ptr/lambda_ptr.hpp>
#include <functional>

extern "C" void some_c_function(void (*callback)(const char *a, int b))
{
    callback("test", 1337);
}

extern "C" void some_c_function_with_user_data(void (*callback)(const char *a, int b, void *user_data), void *user_data)
{
    callback("test", 1337, user_data);
}

TEST_CASE("Basic usage is tested", "[Basic]")
{
    int test = 0;

    some_c_function(lambda_ptr::pointer_to([&](const char *a, int b) {
        REQUIRE(strcmp(a, "test") == 0);
        REQUIRE(b == 1337);
        test = 1;
    }));

    REQUIRE(test == 1);

    some_c_function(lambda_ptr::pointer_to<void(const char *, int)>([&](auto a, auto b) {
        REQUIRE(strcmp(a, "test") == 0);
        REQUIRE(b == 1337);
        test = 2;
    }));

    REQUIRE(test == 2);

    some_c_function(lambda_ptr::pointer_to<void (*)(const char *, int)>([&](auto a, auto b) {
        REQUIRE(strcmp(a, "test") == 0);
        REQUIRE(b == 1337);
        test = 3;
    }));

    REQUIRE(test == 3);

    auto user_data = lambda_ptr::user_data(test);

    some_c_function_with_user_data(
        [](const char *, int, void *data) {
            auto &[test] = decltype(user_data)::from(data);
            test = 4;
        },
        &user_data);

    REQUIRE(test == 4);

    static_assert(lambda_ptr::detail::is_lambda<decltype([&] {})>);
    static_assert(lambda_ptr::detail::is_lambda<decltype([=] {})>);

    static_assert(not lambda_ptr::detail::is_lambda<decltype([] {})>);
    static_assert(not lambda_ptr::detail::is_lambda<std::function<void(const char *, int)>>);
}