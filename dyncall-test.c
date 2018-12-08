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

typedef int (*printf_ptr)(
   const char *format,
   ...
);

int main(void) {
    DCCallVM *vm;
    void *native_result = (void *)0xcafebabe;
    int native_int;

    DLLib *pLib = (void *)0x31337;
    void *msvcrt_printf = (void *)42;

    char msg[]=">> Hello, world!\n";

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

    printf("About to dcFree\n");
    dcFree(vm);
    printf("after dcFree\n");

    vm = dcNewCallVM(8192);
    //dcMode(vm, DC_CALL_C_X64_WIN64);  // No output with this in Cygwin
    //dcMode(vm, DC_CALL_C_X86_WIN32_STD);  //ditto
    //dcMode(vm, DC_CALL_C_ELLIPSIS); //Ditto
    //dcMode(vm, DC_CALL_C_ELLIPSIS_VARARGS); //Ditto
    //dcMode(vm, DC_CALL_C_X86_CDECL);    //ditto
    //dcMode(vm, DC_CALL_C_X86_WIN32_FAST_MS); //ditto
    //dcMode(vm, DC_CALL_C_X86_WIN32_FAST_GNU); //ditto
    //dcMode(vm, DC_CALL_C_X86_WIN32_THIS_MS);  //ditto
    //dcMode(vm, DC_CALL_C_X86_WIN32_FAST_GNU); //ditto
    //dcMode(vm, DC_CALL_SYS_DEFAULT);    //ditto

    // OF INTEREST: with no dcMode() call, both the dcCall dynamic and the
    // direct-call dynamic produce no output.  However, when run under cmd,
    // both produce the desired output!
    dcReset(vm);

    dcArgPointer(vm, msg);

    printf("\nAbout to dcCall dynamic symbol\n");
    native_result = (void *)0xdeadbeef;
        // so we know if it doesn't get assigned
    native_result = dcCallPointer(vm, msvcrt_printf);
    printf("After dcCall; result %p\n", native_result);

    printf("About to dcFree\n");
    dcFree(vm);
    printf("after dcFree\n");

    printf("\nAbout to directly call dynamic symbol\n");
    native_result = (void *)0xdeadbee;
        // so we know if it doesn't get assigned
    native_int = (*((printf_ptr)msvcrt_printf))(msg);
    printf("After direct call; result %x\n", native_int);


    return 0;
}
// vi: set fo-=ro:
