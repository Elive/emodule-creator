#include <e.h>
#include "e_mod_main.h"

/***************************************************************************/

static Evas *evas = NULL;
static void _skeletor_gadconless_init(E_Module *m);
static void _skeletor_gadconless_shutdown();
static void cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event);
static void cb_mouse_up(void *data, Evas *e, Evas_Object *obj, void *event);
static void cb_mouse_move(void *data, Evas *e, Evas_Object *obj, void *event);
/***************************************************************************/

/***************************************************************************/
/***************************************************************************/

/***************************************************************************/
/**/
/* actual module specifics */

typedef struct _Instance Instance;

struct _Instance
{
   Evas_Object     *logo;
   E_Menu          *menu;
};

Config *skeletor_config = NULL;

/*
 * This function is called when you add the Module to a Shelf or Gadgets, it
 * this is where you want to add functions to do things.
 */

EAPI E_Module_Api e_modapi =
{
   E_MODULE_API_VERSION,
   "Skeletor"
};

/*
 * This function is called when you remove the Module from a Shelf or Gadgets,
 * what this function really does is clean up, it removes everything the module
 * displays
 */
EAPI void *
e_modapi_init(E_Module *m)
{
    if(!skeletor_config)
        skeletor_config = E_NEW(Config, 1);

   skeletor_config->module = m;
   _skeletor_gadconless_init(skeletor_config->module);
   return m;
}

/*
 * This function sets the Gadcon name of the module, not to confuse this with
 * E_Module_Api
 */
EAPI int
e_modapi_shutdown(E_Module *m __UNUSED__)
{
    if(skeletor_config)
        skeletor_config = NULL;

   _skeletor_gadconless_shutdown();
    E_FREE(skeletor_config);
   return 1;
}

/*
 * e_modapi_save is used to save and store configuration info on local
 * storage
 */
EAPI int
e_modapi_save(E_Module *m __UNUSED__)
{
   return 1;
}

/****************************************************************************/
/* Skeletor Functions Custom Functions                                      */
/****************************************************************************/

static void
cb_mouse_move(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
    Instance *inst;
    Evas_Event_Mouse_Move *ev;

    inst = data;
    ev = event_info;

    if (ev->buttons == 1)
    {
        evas_object_move(inst->logo,ev->cur.canvas.x,ev->cur.canvas.y);
    }
}

static void
cb_mouse_down(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
    Instance *inst;
    Evas_Event_Mouse_Down *ev;

    inst = data;
    ev = event_info;

    /*if (ev->button == 1)
    {
	    Evas_Coord x, y, w, h;
        evas_object_geometry_get(inst->o_button,&x, &y, &w, &h);
    }*/
}

/*
 * This function is a call-back it removes the menu after the use clicks on
 * a item in the menu or click of the menu, basically it removes the menu
 * from being displayed.
 */
static void
_skeletor_gadconless_shutdown()
{
    Instance *inst;
    if (!inst) return;
    evas_object_del(inst->logo);
    free(inst);
}

static void
_skeletor_gadconless_init(E_Module *m __UNUSED__)
{
    Instance *inst;
    E_Zone *zone;
    Evas_Coord w, h;
    Evas_Object *o;
    char buf[PATH_MAX];

    inst = E_NEW(Instance, 1);

    zone = e_util_container_zone_number_get(0, 0); /* get the first zone */
    evas = zone->container->bg_evas;

    snprintf(buf, sizeof(buf), "%s/skeletor.edj",
                e_module_dir_get(skeletor_config->module));

    o = edje_object_add(evas);
    e_theme_edje_object_set(o, "base/theme/modules/skeletor",
           "modules/skeletor/main");
    edje_object_file_set(o,buf,"modules/skeletor/main");
    edje_object_signal_emit(o, "e,state,unfocused", "e");

    w = 128;
    h = 128;
    evas_object_resize(o, w, h);
    evas_object_move(o,
            ((zone->x + zone->w - w) / 2),
		    ((zone->y + zone->h - h) / 2));
    evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN,
				  cb_mouse_down, inst);
    evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_MOVE,
				  cb_mouse_move, inst);
    inst->logo = o;

    evas_object_show(o);
}

