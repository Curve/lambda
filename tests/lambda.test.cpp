#include <catch2/catch_test_macros.hpp>
#include <lambda/lambda.hpp>
#include <functional>

extern "C" void some_c_function(void (*callback)(const char *a, int b))
{
    callback("test", 1337);
}

TEST_CASE("Lambda utilities are tested", "[Lambda]")
{
    int test = 0;

    auto fn_ptr_1 = lambda::pointer_to([&](const char *a, int b) {
        REQUIRE(strcmp(a, "test") == 0);
        REQUIRE(b == 1337);
        test = 1;
    });

    some_c_function(fn_ptr_1);

    REQUIRE(test == 1);

    auto fn_ptr_2 = lambda::pointer_to<void(const char *, int)>([&](auto a, auto b) {
        REQUIRE(strcmp(a, "test") == 0);
        REQUIRE(b == 1337);
        test = 2;
    });

    some_c_function(fn_ptr_2);

    REQUIRE(test == 2);

    auto fn_ptr_3 = lambda::pointer_to<void (*)(const char *, int)>([&](auto a, auto b) {
        REQUIRE(strcmp(a, "test") == 0);
        REQUIRE(b == 1337);
        test = 3;
    });

    some_c_function(fn_ptr_3);

    REQUIRE(test == 3);

    static_assert(lambda::detail::is_capture_lambda<decltype([&] {})>);
    static_assert(lambda::detail::is_capture_lambda<decltype([=] {})>);

    static_assert(not lambda::detail::is_capture_lambda<decltype([] {})>);
    static_assert(not lambda::detail::is_capture_lambda<std::function<void(const char *, int)>>);
}