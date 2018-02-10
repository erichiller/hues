
:: parameters ::
:: https://ss64.com/nt/syntax-args.html
:: IF STATEMENTS ::
:: https://ss64.com/nt/if.html



# Setting up the Windows Compiler _cl_

https://docs.microsoft.com/en-us/cpp/build/reference/cl-environment-variables


[Compiler Command Line Syntax](https://msdn.microsoft.com/en-us/library/610ecb4h.aspx)
This also contains the **ordering** of arguments that are passed to `cl`  
Essentially it is:  
```batch
CL [option...] file... [option | file]... [lib...] [@command-file] [/link link-opt...]  
```

## Specific directives (of use)

| Directive     | Purpose       | Link
| ---           | ---           | ---
| `\D option`   | Set `#define`s or macros| https://docs.microsoft.com/en-us/cpp/build/reference/d-preprocessor-definitions

