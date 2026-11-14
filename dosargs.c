/* dosargs.c -- the child `dos`'s Exec test runs on Windows.
 *
 * Not a shim: nothing here backs an `external`, which is why it is not named
 * *ffi.c. ./build.sh compiles it to dosargs.exe on Windows only.
 *
 * Windows has no argument vector. Exec builds one command line in dosffi.c and
 * the child takes it apart again, so whether an argument survives depends on
 * the two agreeing -- and no program Windows ships reports its arguments as
 * CommandLineToArgvW splits them (`cmd` reparses its own line). This one
 * does: it splits its command line the way any Windows program's runtime
 * does, and compares the result with the list below, which dos.a24's test
 * passes in the same order.
 *
 * Exits 0 when every argument arrived exactly; 200 when the COUNT differs;
 * 100 + N when argument N (1-based) differs. A nonzero answer therefore says
 * which argument the quoting lost.
 *
 * The list must match `Exec Passes Its Arguments To The Program` in dos.a24
 * item for item. This file is UTF-8, as the units are.
 */

#include <windows.h>
#include <shellapi.h>
#include <string.h>

static const char *const expected[] = {
    "plain",
    "a b",                  /* a space                                   */
    "c;d",                  /* a semicolon, which a shell would split on */
    "",                     /* empty                                     */
    "tab\tinside",          /* a tab                                     */
    "say \"hi\"",           /* embedded quotes                           */
    "\"",                   /* a lone quote                              */
    "C:\\dir\\",            /* a trailing backslash, unquoted            */
    "C:\\a dir\\",          /* a trailing backslash inside quotes        */
    "back\\\\\"slash",      /* backslashes before a quote                */
    "\\\\server\\share",    /* leading backslashes                       */
    "%PATH%",               /* what cmd would expand                     */
    "a&b|c<d>e^f",          /* cmd's metacharacters                      */
    "caf\xc3\xa9 \xe2\x9c\x93",   /* 'café ✓', which needs the wide call */
};

int main (void)
{
    int       argc = 0;
    wchar_t **argv = CommandLineToArgvW (GetCommandLineW (), &argc);
    int       want = (int) (sizeof expected / sizeof expected[0]);

    if (argv == NULL || argc - 1 != want) return 200;

    for (int i = 0; i < want; i++)
    {
        char got[1024];

        if (!WideCharToMultiByte (CP_UTF8, 0, argv[i + 1], -1, got,
                                  (int) sizeof got, NULL, NULL))
            return 100 + i + 1;

        if (strcmp (got, expected[i]) != 0) return 100 + i + 1;
    }

    return 0;
}
