#include <catch2/catch_test_macros.hpp>
#include <lambda_ptr/lambda_ptr.hpp>
#include <functional>

extern "C" void some_c_function(void (*callback)(const char *a, int b))
{
    callback("test", 1337);
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

    some_c_function(lambda_ptr::pointer_to<void, const char *, int>([&](auto a, auto b) {
        REQUIRE(strcmp(a, "test") == 0);
        REQUIRE(b == 1337);
        test = 2;
    }));

    REQUIRE(test == 2);

    static_assert(lambda_ptr::detail::CaptureLambda<decltype([&] {})>);
    static_assert(lambda_ptr::detail::CaptureLambda<decltype([=] {})>);

    static_assert(not lambda_ptr::detail::CaptureLambda<decltype([] {})>);
    static_assert(not lambda_ptr::detail::CaptureLambda<std::function<void(const char *, int)>>);
}