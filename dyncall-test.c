// dyncall-test.c
// cxw42, 2018
// Based on https://github.com/MoarVM/MoarVM/blob/2018.11/src/core/nativecall_dyncall.c:MVM_nativecall_invoke()
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <setjmp.h>
#include <stddef.h>

#include <dynload.h>
#include <dyncall.h>
#include <dyncall_callback.h>

int main(void) {
    DCCallVM *vm;
    void *native_result;

    char msg[]="Hello, world!\n";
    /* Create and set up call VM. */
    vm = dcNewCallVM(8192);
    dcMode(vm, DC_CALL_C_DEFAULT /*body->convention*/);
        // TODO trace nqp and see which it uses
    dcReset(vm);

    dcArgPointer(vm, msg);

    printf("About to dcCall\n");
    native_result = dcCallPointer(vm, &printf);
    printf("After dcCall; result %p\n", native_result);
    return 0;
}
