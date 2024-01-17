#include <glib/gi18n.h>
#include <locale.h>
#include <stdio.h>
#include "config.h"

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "");
    bindtextdomain(GETTEXT_PACKAGE, LOCALE_DIR);
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    textdomain(GETTEXT_PACKAGE);
    printf(_("gettext"));
    printf("\n");
    return 0;
}
