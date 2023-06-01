<hr>

<div align="center"> 
    <img src="assets/logo.png" height=312/>
</div>

<div align="center"> 

### lambda

A utility for working with C-Libraries from C++

</div>

---

<div align="center"> 
    
### Basic Examples

</div>

> Using lambdas with capture as a function pointer

```cpp
extern "C" void some_c_function(void (*callback)(int a))
{
    // ...
}

std::string some_string;

some_c_function(lambda::pointer_to([&](int)
{
    some_string = "Captured by reference!";
}));
```

<br/>

> A utility for passing `void*` user data
```cpp
extern "C" void some_c_function(void (*callback)(int a, void* user_data), void* user_data)
{
    // ...
}

std::string some_string;
auto user_data = lambda::user_data(some_string);

some_c_function([](int a, void* data) {
    auto &[some_string] = decltype(user_data)::from(data).tuple();
    some_string = "Easy user-data!";
}, user_data);
```

> For more examples, and some useful comments, you might want to take a look at the [tests](tests/)!

---

<div align="center"> 
    
### Installation

</div>

> **Note**  
> This library requires a C++20 capable compiler.

- FetchContent

    ```cmake
    include(FetchContent)
    FetchContent_Declare(lambda GIT_REPOSITORY "https://github.com/Curve/lambda")

    FetchContent_MakeAvailable(lambda)
    target_link_libraries(<YourLibrary> lambda::ptr)
    ```

- Git Submodule

    ```bash
    git submodule add "https://github.com/Curve/lambda"
    ```
    ```cmake
    # Somewhere in your CMakeLists.txt
    add_subdirectory("<path_to_lambda>")
    target_link_libraries(<YourLibrary> lambda::ptr)
    ```

---

<div align="center"> 
    
### Windows

</div>

**Q:** Does this Library work on windows?  
**A:** Yes

**Q:** Does this library work with MSVC?  
**A:** Kind of, beware that lambda captures on MSVC are broken¹ and MSVC Versions below 19.36 are affected by a bug that causes compilation errors.

>¹: This bug has already been reported and will hopefully be fixed with a new MSVC update.

<table>
<tr>
<th>❌ Broken</th>
<th>✅ Working</th>
</tr>
<tr>
<td>

```cpp
some_c_function(lambda::pointer_to([&]{/*...*/}));
```

</td>
<td>

```cpp
auto fn_ptr = lambda::pointer_to([&]{/*...*/});
some_c_function(fn_ptr);
```

</td>
</tr>
</table>


**Q:** Are there alternatives to MSVC?  
**A:** Yes! I'd recommend to just use clang-cl