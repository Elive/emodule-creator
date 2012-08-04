#include <e.h>
#include "e_mod_main.h"

/* gadcon requirements */
static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style);
static void _gc_shutdown(E_Gadcon_Client *gcc);
static void _gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient);
static const char *_gc_label(const E_Gadcon_Client_Class *client_class);
static Evas_Object *_gc_icon(const E_Gadcon_Client_Class *client_class, Evas *evas);
static const char *_gc_id_new(const E_Gadcon_Client_Class *client_class);
static Config_Item *_conf_item_get(const char *id);

/* and actually define the gadcon class that this module provides (just 1) */
static const E_Gadcon_Client_Class _gadcon_class =
{
   GADCON_CLIENT_CLASS_VERSION,
   "Skeletor",
   {
      _gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, _gc_id_new, NULL, NULL
   },
   E_GADCON_CLIENT_STYLE_PLAIN
};
/**/
/***************************************************************************/

/***************************************************************************/
static void _skeletor_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _skeletor_menu_cb_post(void *data, E_Menu *m);
static void _skeletor_menu_cb_cfg(void *data, E_Menu *menu, E_Menu_Item *mi);
/***************************************************************************/

/***************************************************************************/
/***************************************************************************/

/***************************************************************************/
/**/
/* actual module specifics */

typedef struct _Instance Instance;

struct _Instance
{
   E_Gadcon_Client *gcc;
   Evas_Object     *logo;
   E_Gadcon_Popup  *popup;
   E_Menu          *menu;
   Config_Item     *cfg;
};


Config *skeletor_config = NULL;
static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;
static E_Action *act;
static Eina_List *skeletor_instances = NULL;
/*
 * This function is called when you add the Module to a Shelf or Gadgets, it
 * this is where you want to add functions to do things.
 */
static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style)
{
   Evas_Object *o;
   E_Gadcon_Client *gcc;
   Instance *inst;
   char buf[PATH_MAX];

   inst = E_NEW(Instance, 1);

   /*get plugin path*/
   snprintf(buf, sizeof(buf), "%s/skeletor.edj",
           e_module_dir_get(skeletor_config->module));

   o = edje_object_add(gc->evas);
   e_theme_edje_object_set(o, "base/theme/modules/skeletor",
           "modules/skeletor/main");
   edje_object_file_set(o,buf,"modules/skeletor/main");
   edje_object_signal_emit(o, "e,state,unfocused", "e");
   evas_object_show (o);

   gcc = e_gadcon_client_new(gc, name, id, style, o);
   gcc->data = inst;

   inst->gcc = gcc;
   inst->logo = o;

   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN,
	 _skeletor_cb_mouse_down, inst);

   skeletor_config->instances =
       eina_list_append(skeletor_config->instances, inst);

   return gcc;
}

/*
 * This function is called when you remove the Module from a Shelf or Gadgets,
 * what this function really does is clean up, it removes everything the module
 * displays
 */
static void
_gc_shutdown(E_Gadcon_Client *gcc)
{
   Instance *inst;

   inst = gcc->data;
   if (inst->menu)
     {
	e_menu_post_deactivate_callback_set(inst->menu, NULL, NULL);
	e_object_del(E_OBJECT(inst->menu));
	inst->menu = NULL;
     }
   evas_object_del(inst->logo);
   free(inst);
}

static void
_gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient __UNUSED__)
{
    Instance *inst;
    Evas_Coord mw, mh;

    inst = gcc->data;
    mw = 0, mh = 0;
    edje_object_size_min_get(inst->logo, &mw, &mh);
    if ((mw < 1) || (mh < 1))
        edje_object_size_min_calc(inst->logo, &mw, &mh);
    if (mw < 4) mw = 4;
    if (mh < 4) mh = 4;
    e_gadcon_client_aspect_set(gcc, mw, mh);
    e_gadcon_client_min_size_set(gcc, mw, mh);
}

/*
 * This function sets the Gadcon name of the module, not to confuse this with
 * E_Module_Api
 */
static const char *
_gc_label(const E_Gadcon_Client_Class *client_class __UNUSED__)
{
   return ("Skeletor");
}

/*
 * This functions sets the Gadcon icon, the icon you see when you go to add
 * the module to a Shelf or Gadgets.
 */
static Evas_Object *
_gc_icon(const E_Gadcon_Client_Class *client_class __UNUSED__, Evas *evas)
{
   Evas_Object *o;
   char buf[PATH_MAX];

   o = edje_object_add(evas);
   snprintf(buf, sizeof(buf), "%s/e-module-skeletor.edj",
           e_module_dir_get(skeletor_config->module));
   edje_object_file_set(o, buf, "icon");
   return o;
}

/*
 * This function sets the id for the module, so it can be unique from other
 * modules
 */
static const char *
_gc_id_new(const E_Gadcon_Client_Class *client_class __UNUSED__)
{
    static char buf[PATH_MAX];

    snprintf(buf, sizeof(buf), "%s.%d", _gadcon_class.name,
            eina_list_count(skeletor_config->instances) + 1);
    return buf;
}

static Config_Item *
_conf_item_get(const char *id)
{
    Config_Item *ci;

    GADCON_CLIENT_CONFIG_GET(Config_Item, skeletor_config->items, _gadcon_class, id);

    ci = E_NEW(Config_Item, 1);
    ci->id = eina_stringshare_add(id);

    skeletor_config->items = eina_list_append(skeletor_config->items, ci);
    e_config_save_queue();

    return ci;
}

static void
_skeletor_menu_cb_cfg(void *data, E_Menu *menu __UNUSED__, E_Menu_Item *mi __UNUSED__)
{
    Instance *inst = data;
    E_Container *con;

    if(inst->popup)
    {
        e_object_del(E_OBJECT(inst->popup));
        inst->popup = NULL;
    }
    con = e_container_current_get(e_manager_current_get());
    e_int_config_skeletor_module(con, NULL);
}

static void
_skeletor_cb_mouse_down(void *data, Evas *e __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info)
{
    Instance *inst;
    Evas_Event_Mouse_Down *ev;

    inst = data;
    ev = event_info;
    if((ev->button == 1) && !(inst->menu))
    {
	    int x, y;

        inst->menu = e_int_menus_main_new();

        e_menu_post_deactivate_callback_set(inst->menu,
                _skeletor_menu_cb_post, inst);

        e_gadcon_locked_set(inst->gcc->gadcon,1);
        e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon, &x, &y, NULL, NULL);

        e_menu_activate_mouse(inst->menu,
                e_util_zone_current_get(e_manager_current_get())
                , x + ev->output.x, y + ev->output.y,
                1, 1, E_MENU_POP_DIRECTION_AUTO, ev->timestamp);

        edje_object_signal_emit(inst->logo, "e,state,focused", "e");
    }
    else if ((ev->button == 3) && (!inst->menu))
    {
        E_Zone *zone;
        E_Menu *m;
        E_Menu_Item *mi;
        int x, y;

        zone = e_util_zone_current_get(e_manager_current_get());

        m = e_menu_new();
        mi = e_menu_item_new(m);
        e_menu_item_label_set(mi, "Settings");
        e_util_menu_item_theme_icon_set(mi, "configure");
        e_menu_item_callback_set(mi, _skeletor_menu_cb_cfg, inst);
        m = e_gadcon_client_util_menu_items_append(inst->gcc, m, 0);
        e_menu_post_deactivate_callback_set(m, _skeletor_menu_cb_post, inst);
        inst->menu = m;

        e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon, &x, &y, NULL, NULL);
        e_menu_activate_mouse(m, zone, x + ev->output.x, y + ev->output.y,
                              1, 1, E_MENU_POP_DIRECTION_AUTO, ev->timestamp);

        evas_event_feed_mouse_up(inst->gcc->gadcon->evas, ev->button,
                                 EVAS_BUTTON_NONE, ev->timestamp, NULL);
     }
}

/*
 * This function is a call-back it removes the menu after the use clicks on
 * a item in the menu or click of the menu, basically it removes the menu
 * from being displayed.
 */
static void
_skeletor_menu_cb_post(void *data, E_Menu *m __UNUSED__)
{
    Instance *inst;

    inst = data;
    if(inst->menu)
    {
        edje_object_signal_emit(inst->logo, "e,state,unfocused", "e");
        e_object_del(E_OBJECT(inst->menu));
        inst->menu = NULL;
    }
}

/* This is needed to advertise a label for the module IN the code (not just
 * the .desktop file) but more specifically the api version it was compiled
 * for so E can skip modules that are compiled for an incorrect API version
 * safely) */
EAPI E_Module_Api e_modapi =
{
   E_MODULE_API_VERSION,
   "Skeletor"
};

/*
 * This is the first function called by e17 when you click the enable button
 */
EAPI void *
e_modapi_init(E_Module *m)
{
    conf_item_edd = E_CONFIG_DD_NEW("Config_Item", Config_Item);
#undef T
#undef D
#define T Config_Item
#define D conf_item_edd
    E_CONFIG_VAL(D, T, id, STR);

    conf_edd = E_CONFIG_DD_NEW("Skeletor_Config", Config);
#undef T
#undef D
#define T Config
#define D conf_edd
    E_CONFIG_LIST(D, T, items, conf_item_edd);
    E_CONFIG_VAL(D, T, set, INT);

    skeletor_config = e_config_domain_load("module.skeletor", conf_edd);
    if(!skeletor_config)
        skeletor_config = E_NEW(Config, 1);

    skeletor_config->module = m;
    e_gadcon_provider_register(&_gadcon_class);
    return m;
}

/*
 * This function is called by e17 when you disable the module, in e_modapi_shutdown
 * you should try to free all resources used while the module was enabled.
 */
EAPI int
e_modapi_shutdown(E_Module *m __UNUSED__)
{
    Config_Item *ci;

    e_gadcon_provider_unregister(&_gadcon_class);

    if(skeletor_config)
    {
        skeletor_config->module = NULL;
        skeletor_config->instances = NULL;

        EINA_LIST_FREE(skeletor_config->items, ci)
        {
            eina_stringshare_del(ci->id);
            free(ci);
        }
        E_FREE(skeletor_config);
        skeletor_config = NULL;
   }
   E_CONFIG_DD_FREE(conf_edd);
   E_CONFIG_DD_FREE(conf_item_edd);
   conf_edd = NULL;
   conf_item_edd = NULL;

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

/**/
/***************************************************************************/

