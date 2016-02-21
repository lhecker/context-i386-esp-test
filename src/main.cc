#include <cstdint>
#include <iostream>


typedef void* fcontext_t;

struct transfer_t {
	fcontext_t fctx;
	void* data;
};

extern "C" transfer_t jump_fcontext(const fcontext_t to, void* vp);
extern "C" fcontext_t make_fcontext(void* sp, std::size_t size, void (*fn)(transfer_t));


void context_function(transfer_t t) {
	// The frame address is offset by bit from the actual ESP value. E.g.:
	// ESP           = 0xb734cfc4
	// frame address = 0xb734cfec
	// It still shows the same problem though.
	const auto esp = __builtin_frame_address(0);
	__asm__ __volatile__("":::"memory");

	jump_fcontext(t.fctx, esp);
}

int main(int argc, char* argv[]) {
	const std::size_t STACK_SIZE = 1024 * 1024;
	const auto stack = new char[STACK_SIZE];

	if (stack == 0 || uintptr_t(stack) & 15 != 0) {
		std::cerr << "Bad stack alloc" << std::endl;
		return 1;
	}

	const auto ctx = make_fcontext(stack + STACK_SIZE, STACK_SIZE, context_function);
	const auto t = jump_fcontext(ctx, nullptr);

	std::cout
		<< "Frame Address = " << t.data << std::endl
		<< "Misalignment  = " << (uintptr_t(t.data) & 15) << std::endl;

	return 0;
}
