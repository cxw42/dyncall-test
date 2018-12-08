// dyncall-test.c
// cxw42, 2018
// Based on https://github.com/MoarVM/MoarVM/blob/2018.11/src/core/nativecall_dyncall.c:MVM_nativecall_invoke()
// and https://github.com/MoarVM/MoarVM/blob/2018.11/src/core/nativecall.c:MVM_nativecall_build()

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
    void *native_result = (void *)0xcafebabe;

    DLLib *pLib = (void *)0x31337;
    void *msvcrt_printf = (void *)42;

    char msg[]="Hello, world!\n";

    /* Create and set up call VM. */
    vm = dcNewCallVM(8192);
    dcMode(vm, DC_CALL_C_DEFAULT);
        // since nqp/t/nativecall/01 passes '' for the convention,
        // which https://github.com/MoarVM/MoarVM/blob/2018.11/src/core/nativecall_dyncall.c:MVM_nativecall_get_calling_convention() maps to C_DEFAULT.
    dcReset(vm);

    // Call to a specific identifier
    dcArgPointer(vm, msg);

    printf("About to dcCall\n");
    native_result = dcCallPointer(vm, &printf);
    printf("After dcCall; result %p\n", native_result);

    // ///////////////////////////////////////////////////////////
    // Call to a dynamically-loaded function
    pLib = dlLoadLibrary("msvcrt.dll");
    if(!pLib) {
        fprintf(stderr, "Could not load msvcrt\n");
        return 1;
    }

    msvcrt_printf = dlFindSymbol(pLib, "printf");
    if(!msvcrt_printf) {
        fprintf(stderr, "Could not find printf in msvcrt\n");
        return 1;
    }

    dcReset(vm);

    dcArgPointer(vm, msg);

    printf("About to dcCall dynamic symbol\n");
    native_result = (void *)0xdeadbeef;
        // so we know if it doesn't get assigned
    native_result = dcCallPointer(vm, msvcrt_printf);
    printf("After dcCall; result %p\n", native_result);


    printf("About to dcFree\n");
    dcFree(vm);
    printf("after dcFree\n");

    return 0;
}
