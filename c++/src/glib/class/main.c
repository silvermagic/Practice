#include <glib/gprintf.h>
#include "bird.h"

GMainLoop *loop;

void inthandler(int signo) {
    g_printf("main loop end.\n");
    g_main_loop_quit(loop);
}

int main() {
    loop = g_main_loop_new(NULL, FALSE);
    signal(SIGINT, inthandler);
    g_printf("main loop begin.\n");

    Bird *bird = g_object_new(BIRD_TYPE, NULL);
    Bird *peacock = g_object_new(BIRD_TYPE, "name", "peacock", NULL);
    g_printf("there is a %s and a %s.\n", get_bird_name(bird), get_bird_name(peacock));

    g_main_loop_run(loop);
    if (NULL != loop)
        g_main_loop_unref(loop);
    if (NULL != bird)
        g_object_unref(bird);
    if (NULL != peacock)
        g_object_unref(peacock);
    return 0;
}
