#pragma once

#include <csetjmp>
#include <functional>
#include <iostream>
#include <vector>
#include <cassert>

struct error {
    static constexpr int io_error = 1;
    static constexpr int math_error = 2;
};

std::function<void(int, int)> __CRASH_HANDLER = [](int first, int second) {
    std::cout << "Exception in destructor " << first << ' ' << second << std::endl;
    std::abort();
};

struct ContextInfo {
    ContextInfo* previous_context;
    std::vector<std::function<void()>> destructors;
    jmp_buf env;
    int exception_type = 0;
    void ApplyDestructors();
};

bool __IS_DESTRUCTOR_EXCEPTION = false;

void ContextInfo::ApplyDestructors() {
    __IS_DESTRUCTOR_EXCEPTION = true;
    for (size_t i = destructors.size(); i--; ){ 
        destructors[i]();
    }
}

ContextInfo* __CURRENT_CONTEXT = nullptr;


bool ClearIsDestructorException() {
    __IS_DESTRUCTOR_EXCEPTION = false;
    return true;
}


#define TRY \
        ContextInfo context; \
        context.previous_context = __CURRENT_CONTEXT; \
        __CURRENT_CONTEXT = &context; \
        int __LOCAL_EXCEPTION = setjmp(context.env); \
        if (__LOCAL_EXCEPTION) { \
            __CURRENT_CONTEXT = context.previous_context; \
        } \
        if (__LOCAL_EXCEPTION == 0) \

#define CATCH(exception) \
        else if (__LOCAL_EXCEPTION == exception && ClearIsDestructorException()) \

#define THROW(exception)  \
        if (__IS_DESTRUCTOR_EXCEPTION) { \
            assert(__CURRENT_CONTEXT); \
            __CRASH_HANDLER(__CURRENT_CONTEXT->exception_type, exception); \
        } \
        else if (!__CURRENT_CONTEXT) { \
            std::cout << "No Try Catch" << std::endl; \
            std::abort(); \
        } else { \
        __CURRENT_CONTEXT->exception_type = exception; \
        __CURRENT_CONTEXT->ApplyDestructors(); \
        longjmp(__CURRENT_CONTEXT->env, exception); \
        } \

#define AUTO_OBJECT(Class, name) \
        Class name; \
        if (__CURRENT_CONTEXT) { \
            __CURRENT_CONTEXT->destructors.push_back([&name]() { \
                name.~Class(); \ 
            }); \ 
        } \

#define SET_UNEXPECTED_HANDLER(func) \
        __CRASH_HANDLER = func;