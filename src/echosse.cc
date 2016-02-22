//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <emmintrin.h>


typedef void* fcontext_t;

struct transfer_t {
    fcontext_t fctx;
    void* data;
};

extern "C" transfer_t jump_fcontext(const fcontext_t to, void* vp);
extern "C" fcontext_t make_fcontext(void* sp, std::size_t size, void (*fn)(transfer_t));


void echoSSE(int i) {
    __m128i xmm;
    xmm = _mm_set_epi32(i, i + 1, i + 2, i + 3);
    uint32_t v32[4];
    memcpy(& v32, & xmm, 16);
    std::cout << v32[0];
    std::cout << v32[1];
    std::cout << v32[2];
    std::cout << v32[3];
}

void echo(transfer_t t) {
    for (;;) {
        const auto i = (int)t.data;
        std::cout << i;
        echoSSE(i);
        std::cout << " ";
        t = jump_fcontext(t.fctx, nullptr);
    }
}

int main(int argc, char* argv[]) {
    const std::size_t STACK_SIZE = 1024 * 1024;
    const auto stack = new char[STACK_SIZE];
    transfer_t t = {
        make_fcontext(stack + STACK_SIZE, STACK_SIZE, echo),
        nullptr
    };

    for (int i = 0; i < 10; ++i) {
        t = jump_fcontext(t.fctx, (void*)i);
    }

    std::cout << "\nDone" << std::endl;
    return EXIT_SUCCESS;
}
