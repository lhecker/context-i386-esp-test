#include <cstdint>
#include <emmintrin.h>
#include <iostream>


typedef void* fcontext_t;

struct transfer_t {
	fcontext_t fctx;
	void* data;
};

extern "C" transfer_t jump_fcontext(const fcontext_t to, void* vp);
extern "C" fcontext_t make_fcontext(void* sp, std::size_t size, void (*fn)(transfer_t));


void context_function(transfer_t t) {
	volatile float __attribute__ ((vector_size(16))) xmm;
	jump_fcontext(t.fctx, (void*)&xmm);
}

int main(int argc, char* argv[]) {
	const std::size_t STACK_SIZE = 1024 * 1024;
	const auto stack = new char[STACK_SIZE];
	const auto ctx = make_fcontext(stack + STACK_SIZE, STACK_SIZE, context_function);
	const auto t = jump_fcontext(ctx, nullptr);

	std::cout
		<< "Address      = " << t.data << std::endl
		<< "Misalignment = " << (uintptr_t(t.data) & 15) << std::endl;

	return 0;
}
