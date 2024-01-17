#include "bird.h"

#define BIRD_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), BIRD_TYPE, Bird))

enum {
    PROP_0,
    PROP_NAME,
};

struct _BirdPrivate {
    gchar *name;
};

/** Bird构造函数 */
static void bird_init(Bird *bird);

static void bird_class_init(BirdClass *klass);

/** Bird析构函数 */
static void bird_dispose(GObject *gobject);

static void bird_finalize(GObject *gobject);

/** Bird成员属性操作 */
static void bird_get_property(GObject *gobject, guint prop_id, GValue *value, GParamSpec *pspec);

static void bird_set_property(GObject *gobject, guint prop_id, const GValue *value, GParamSpec *pspec);

/** 定义Bird类型 */
G_DEFINE_TYPE(Bird, bird, G_TYPE_OBJECT
)


/** ------------------------------------------------------------------ */
/**                   class     implement                              */
/** ------------------------------------------------------------------ */
static void bird_init(Bird * bird) {
    BirdPrivate *priv = (BirdPrivate *) BIRD_GET_PRIVATE(bird);
    bird->priv = priv;

    priv->name = g_strdup("bird");
}

static void bird_class_init(BirdClass *klass) {
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->dispose = bird_dispose;
    gobject_class->finalize = bird_finalize;
    gobject_class->get_property = bird_get_property;
    gobject_class->set_property = bird_set_property;

    g_object_class_install_property(gobject_class, PROP_NAME,
                                    g_param_spec_string("name", "bird name", "bird name", NULL,
                                                        G_PARAM_READWRITE)); // https://developer.gnome.org/gobject/stable/gobject-Standard-Parameter-and-Value-Types.html#g-value-set-string

    g_type_class_add_private(klass, sizeof(BirdPrivate));
}

static void bird_dispose(GObject *gobject) {
    Bird * bird = BIRD(gobject);
    BirdPrivate *priv = bird->priv;

    /* Chain up to the parent class */
    if (G_OBJECT_CLASS(bird_parent_class)->dispose)
        G_OBJECT_CLASS(bird_parent_class)->dispose(gobject);
}

static void bird_finalize(GObject *gobject) {
    Bird * bird = BIRD(gobject);
    BirdPrivate *priv = bird->priv;

    if (priv->name)
        g_free(priv->name);

    /* Chain up to the parent class */
    if (G_OBJECT_CLASS(bird_parent_class)->finalize)
        G_OBJECT_CLASS(bird_parent_class)->finalize(gobject);
}

static void bird_get_property(GObject *gobject, guint prop_id, GValue *value, GParamSpec *pspec) {
    Bird * bird = BIRD(gobject);
    BirdPrivate *priv = bird->priv;

    switch (prop_id) {
        case PROP_NAME:
            g_value_set_string(value, priv->name);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, pspec);
            break;
    }
}

static void bird_set_property(GObject *gobject, guint prop_id, const GValue *value, GParamSpec *pspec) {
    Bird * bird = BIRD(gobject);
    BirdPrivate *priv = bird->priv;

    switch (prop_id) {
        case PROP_NAME: {
            if (NULL != priv->name)
                g_free(priv->name);
            priv->name = g_value_dup_string(value);
        }
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, pspec);
            break;
    }
}

/** ------------------------------------------------------------------ */
/**                     public    api                                  */
/** ------------------------------------------------------------------ */
const gchar *get_bird_name(Bird * self) {
    if (IS_BIRD(self))
        return self->priv->name;

    return "";
}
