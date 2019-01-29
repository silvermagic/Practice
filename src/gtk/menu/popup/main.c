#include <gtk/gtk.h>

gint on_button_press_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    g_return_val_if_fail(widget != NULL, FALSE);
    g_return_val_if_fail(GTK_IS_MENU(data), FALSE);
    g_return_val_if_fail(event != NULL, FALSE);

    // 响应点击事件
    if (event->type == GDK_BUTTON_PRESS)
    {
        GdkEventButton *mouse = (GdkEventButton *) event;
        if (mouse->button == GDK_BUTTON_SECONDARY)
        {
            gtk_widget_show_all(GTK_WIDGET(data));
            // 显示弹出菜单
            gtk_menu_popup_at_widget(GTK_MENU(data), widget, GDK_GRAVITY_SOUTH_WEST, GDK_GRAVITY_NORTH_WEST, NULL);
            return TRUE;
        }
    }
    return FALSE;
}

int main( int argc, char *argv[])
{
    GtkWidget *window;

    // 初始化
    gtk_init(&argc, &argv);

    // 创建窗口（显示在最顶层）
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 250);

    // 设置窗口标签
    GtkWidget *label = gtk_label_new("test");
    gtk_container_add(GTK_CONTAINER(window), label);

    // 创建菜单
    GtkWidget *menu = gtk_menu_new();
    GtkWidget *pasteMi = gtk_menu_item_new_with_label("Paste");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), pasteMi);

    // 关联事件
    g_signal_connect(window, "button-press-event", G_CALLBACK(on_button_press_event), menu);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // 显示窗口
    gtk_widget_show_all(window);

    // 窗口循环
    gtk_main();

    return 0;
}
