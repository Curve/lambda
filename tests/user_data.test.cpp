#include <catch2/catch_test_macros.hpp>
#include <lambda/user_data.hpp>

extern "C" void some_c_function_with_user_data(void (*callback)(const char *a, int b, void *user_data), void *user_data)
{
    callback("test", 1337, user_data);
}

TEST_CASE("UserData utilities are tested", "[UserData]")
{
    int test = 0;

    auto user_data = lambda::user_data(test);

    some_c_function_with_user_data(
        [](const char *, int, void *data) {
            //! It is important that the user_data stays alive until used by the callback when simply passing it to the
            //! callback
            auto &[test] = decltype(user_data)::from(data).tuple();
            test = 4;
        },
        user_data);

    auto user_data_2 = lambda::user_data(test);

    some_c_function_with_user_data(
        [](const char *, int, void *data) {
            auto user_data =
                decltype(user_data_2)::from_detached(data); //! We get a `unique_ptr` to the detached user_data so that
                                                            //! we won't have to `delete` it ourselves.
            auto &[test] = user_data->tuple();
            test = 5;
        },
        user_data_2.detach()); //! We `detach` the user_data, so that it lives independent from the current scope.

    REQUIRE(test == 5);
}