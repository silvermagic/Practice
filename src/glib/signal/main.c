#include <glib/gprintf.h>
#include "bird.h"

GMainLoop *loop;
Bird *sparrow;

void inthandler(int signo) {
    //小鸟飞了
    i_believe_i_can_fly(sparrow, "forest", "south");
    g_printf("main loop end.\n");
    g_main_loop_quit(loop);
}

void HandleBirdFly(Bird *self, gchar *place, gchar *toward, gpointer user_data) {
    g_printf("The %s found a %s fly %s from the %s and catch it.\n",
             (gchar *) user_data,
             get_bird_name(self),
             toward,
             place);
}

int main() {
    loop = g_main_loop_new(NULL, FALSE);
    signal(SIGINT, inthandler);
    g_printf("main loop begin.\n");

    sparrow = g_object_new(BIRD_TYPE, "name", "sparrow", NULL);
    //捕获麻雀飞行事件
    g_signal_connect(sparrow, "fly", G_CALLBACK(HandleBirdFly), "eagle");

    g_main_loop_run(loop);
    if (NULL != loop)
        g_main_loop_unref(loop);
    if (NULL != sparrow)
        g_object_unref(sparrow);
    return 0;
}
