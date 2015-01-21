#include "spidermonkey.h"

static JSClass global_class = {
    "global",
    JSCLASS_GLOBAL_FLAGS,
    // [SpiderMonkey 38] Following Stubs are removed. Remove those lines.
    JS_PropertyStub,
    JS_DeletePropertyStub,
    JS_PropertyStub,
    JS_StrictPropertyStub,
    JS_EnumerateStub,
    JS_ResolveStub,
    JS_ConvertStub
};

int myGoGo(JSContext *cx, unsigned argc, JS::Value *vp) {
    printf("GOGO\n");

    return 1;
}

JSContext* JS_NewContext(JSRuntime *rt) {
    return JS_NewContext(rt, 8192);
}

JSRuntime* JS_NewRuntime() {
    return JS_NewRuntime(8L * 1024 * 1024, JS_NO_HELPER_THREADS);
}

void reportError(JSContext *cx, const char *message, JSErrorReport *report) {
    fprintf(stderr, "E(%d) W(%d)", JSREPORT_IS_EXCEPTION(report->flags), JSREPORT_IS_WARNING(report->flags));
    fprintf(stderr, "%s:%u:%s\n", report->filename ? report->filename : "[no filename]", (unsigned int) report->lineno, message);
}

const char* foo(JSContext *cx, const char *script) {
    JS_SetErrorReporter(cx, reportError);

    JS::RootedObject global(cx, JS_NewGlobalObject(cx, &global_class, nullptr));
    JS::RootedValue rval(cx);

    JSAutoCompartment ac(cx, global);

    JS_InitStandardClasses(cx, global);
    JS_DefineFunction(cx, global, "gogo", myGoGo, 0, 0);

    const char *filename = "noname";
    int lineno = 1;
    JSScript *compiledScript = JS_CompileScript(cx, global, script, strlen(script), filename, lineno);

    if (!compiledScript) {
        JS_ReportError(cx, "compile error :(");

        return "";
    }

    if (!JS_ExecuteScript(cx, global, compiledScript, rval.address())) {
        JS_ReportError(cx, "execute error :(");

        return "";
    }

    JSString *str = rval.toString();

    return JS_EncodeString(cx, str);
}
