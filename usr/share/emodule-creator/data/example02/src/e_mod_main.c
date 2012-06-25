#include <e.h>
#include "e_mod_main.h"

typedef struct _Draggie Draggie;

struct _Draggie
{
   Evas_Object *obj;
   struct {
      struct {
    Evas_Coord x, y;
      } obj;
      Evas_Coord x, y;
      int count;
   } down;
};

/* internal calls */
static Evas_Object *theme_obj_new(Evas *e, const char *group);

static void cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event);
static void cb_mouse_up(void *data, Evas *evas, Evas_Object *obj, void *event);
static void cb_mouse_move(void *data, Evas *evas, Evas_Object *obj, void *event);

/* draggie state */
static E_Module *mod = NULL;
static Evas *evas = NULL;
static Draggie *draggie = NULL;

/* called from the module core */
void
_e_mod_HOHO_init(E_Module *m)
{
   E_Zone *zone;
   Draggie *d;
   Evas_Coord w, h;
   Evas_Object *o;
   
   mod = m; /* save the module handle */
   zone = e_util_container_zone_number_get(0, 0); /* get the first zone */
   evas = zone->container->bg_evas;
   
   d = calloc(1, sizeof(Draggie));
   if (!d) return;
   draggie = d;
   
   o = theme_obj_new(evas, "e/modules/HOHO/main");
   w = 128;
   h = 128;
   evas_object_resize(o, w, h);
   evas_object_move(o,
		    ((zone->x + zone->w - w) / 2),
		    ((zone->y + zone->h - h) / 2));
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN,
				  cb_mouse_down, draggie);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_UP,
				  cb_mouse_up, draggie);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_MOVE,
				  cb_mouse_move, draggie);
   d->obj = o;
   
   evas_object_show(o);
}

void
_e_mod_HOHO_shutdown(void)
{
   if (!draggie) return;
   evas_object_del(draggie->obj);
   free(draggie);
}

/* internal calls */
static Evas_Object *
theme_obj_new(Evas *e, const char *group)
{
   Evas_Object *o;

   o = edje_object_add(e);
   if (!e_theme_edje_object_set(o, "base/theme/modules/HOHO", group))
     {
	char buf[PATH_MAX];
	
	snprintf(buf, sizeof(buf), "%s/HOHO.edj", e_module_dir_get(mod));
	edje_object_file_set(o, buf, group);
     }
   return o;
}

static void
cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event)
{
   Evas_Event_Mouse_Down *ev = event;
   draggie->down.x = ev->canvas.x;
   draggie->down.y = ev->canvas.y;
   draggie->down.count++;
   if (draggie->down.count == 1)
     evas_object_geometry_get(draggie->obj,
			      &(draggie->down.obj.x),
			      &(draggie->down.obj.y), NULL, NULL);
}

static void
cb_mouse_up(void *data, Evas *evas, Evas_Object *obj, void *event)
{
   Evas_Event_Mouse_Up *ev = event;
   draggie->down.count--;
}

static void
cb_mouse_move(void *data, Evas *evas, Evas_Object *obj, void *event)
{
   Evas_Event_Mouse_Move *ev = event;
   if (draggie->down.count > 0)
     evas_object_move(draggie->obj,
		      draggie->down.obj.x + ev->cur.canvas.x - 
		      draggie->down.x,
		      draggie->down.obj.y + ev->cur.canvas.y -
		      draggie->down.y);
}

/* this is needed to advertise a label for the module IN the code (not just
 * the .desktop file) but more specifically the api version it was compiled
 * for so E can skip modules that are compiled for an incorrect API version
 * safely) */
EAPI E_Module_Api e_modapi = 
{
   E_MODULE_API_VERSION, "HOHO"
};

/* called first thing when E inits the module */
EAPI void *
e_modapi_init(E_Module *m) 
{
   bindtextdomain(PACKAGE, LOCALEDIR);
   bind_textdomain_codeset(PACKAGE, "UTF-8");
   
   _e_mod_HOHO_init(m);
   
   return m; /* return NULL on failure, anything else on success. the pointer
	      * returned will be set as m->data for convenience tracking */
}

/* called on module shutdown - should clean up EVERYTHING or we leak */
EAPI int
e_modapi_shutdown(E_Module *m) 
{
   _e_mod_HOHO_shutdown();
   return 1; /* 1 for success, 0 for failure */
}

/* called by E when it thinks this module shoudl go save any config it has */
EAPI int
e_modapi_save(E_Module *m) 
{
   /* called to save config - none currently */
   return 1; /* 1 for success, 0 for failure */
}
