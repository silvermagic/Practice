#ifndef BIRD_H_INCLUDED
#define BIRD_H_INCLUDED

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define BIRD_TYPE            (bird_get_type ())
#define BIRD(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), BIRD_TYPE, Bird))
#define BIRD_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), BIRD_TYPE, BirdClass))
#define IS_BIRD(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BIRD_TYPE))
#define IS_BIRD_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), BIRD_TYPE))
#define BIRD_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), BIRD_TYPE, BirdClass))

typedef struct _Bird Bird;
typedef struct _BirdClass BirdClass;
typedef struct _BirdPrivate BirdPrivate;

struct _Bird {
    GObject parent;

    /*< private >*/
    BirdPrivate *priv;
    /* Do not add fields to this struct */
};

struct _BirdClass {
    GObjectClass parent_class;

    /* signals */
    void (*fly)(Bird *self, gchar *place, gchar *toward, gpointer user_data);

    /*< private >*/
};

GType bird_get_type(void);

//public api
const gchar *get_bird_name(Bird * self);
void i_believe_i_can_fly(Bird * self, gchar * place, gchar * toward);

G_END_DECLS

#endif // BIRD_H_INCLUDED
