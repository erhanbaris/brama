# BRAMA JAVASCRIPT ENGINE
Brama is a ecmascript 5 based easy to use interpreter.


Example code:
```c
    t_context* context = brama_init(0);
    brama_compile(context, "var result = Number.parseInt('0xF') == 15;");
    brama_run(context);

    t_get_var_info_ptr var_info = NULL;
    brama_status status = brama_get_var(context, "result", &var_info);
    assert(status == BRAMA_OK);
    assert(var_info->type == CONST_BOOL);
    assert(var_info->bool_ == true);
    brama_destroy_get_var(context, &var_info);

    brama_destroy(context);
```