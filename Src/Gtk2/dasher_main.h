#ifndef __dasher_main_h__
#define __dasher_main_h__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS
#define TYPE_DASHER_MAIN            (dasher_main_get_type())
#define DASHER_MAIN(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), TYPE_DASHER_MAIN, DasherMain ))
#define DASHER_MAIN_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_DASHER_MAIN, DasherMainClass ))
#define IS_DASHER_MAIN(obj)	    (G_TYPE_CHECK_INSTANCE_TYPE((obj), TYPE_DASHER_MAIN))
#define IS_DASHER_MAIN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_DASHER_MAIN))
#define DASHER_MAIN_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_DASHER_MAIN, DasherMainClass))

typedef struct _DasherMain DasherMain;
typedef struct _DasherMainClass DasherMainClass;

struct _DasherMain {
  GObject parent;
  gpointer private_data;
};

struct _DasherMainClass {
  GObjectClass parent_class;
};

DasherMain *dasher_main_new(int argc, char **argv);
GType dasher_main_get_type();
G_END_DECLS

#endif