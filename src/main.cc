#include <cstdint>
#include <iostream>


typedef void* fcontext_t;

struct transfer_t {
	fcontext_t fctx;
	void* data;
};

extern "C" transfer_t jump_fcontext(const fcontext_t to, void* vp);
extern "C" fcontext_t make_fcontext(void* sp, std::size_t size, void (*fn)(transfer_t));


#if defined(__i386)
# define ACQUIRE_ESP(esp) __asm__ __volatile__("mov %%esp, %0" : "=r"(esp) ::"memory");
#elif defined(__x86_64__)
# define ACQUIRE_ESP(esp) __asm__ __volatile__("mov %%rsp, %0" : "=r"(esp) ::"memory");
#else
# error "Unsupported arch"
#endif



void context_function(transfer_t t) {
	// The frame address is offset by bit from the actual ESP value. E.g.:
	// ESP           = 0xb734cfc4
	// frame address = 0xb734cfec
	// It still shows the same problem though.
	void* esp;
	ACQUIRE_ESP(esp);

	jump_fcontext(t.fctx, esp);
}

int main(int argc, char* argv[]) {
	void* main_esp;
	ACQUIRE_ESP(main_esp);

	const std::size_t STACK_SIZE = 1024 * 1024;
	const auto stack = new char[STACK_SIZE];

	if (stack == 0 || uintptr_t(stack) & 15 != 0) {
		std::cerr << "Bad stack alloc" << std::endl;
		return 1;
	}

	const auto ctx = make_fcontext(stack + STACK_SIZE, STACK_SIZE, context_function);
	const auto t = jump_fcontext(ctx, nullptr);

	std::cout
		<< "MAIN: ESP          = " << main_esp << std::endl
		<< "MAIN: Misalignment = " << (uintptr_t(main_esp) & 15) << std::endl
		<< "FCTX: ESP          = " << t.data << std::endl
		<< "FCTX: Misalignment = " << (uintptr_t(t.data) & 15) << std::endl;

	return 0;
}
