/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include <e.h>
//#include "etk/Etk.h"
#include "e_mod_main.h"
//   If you need any of these feautres, just uncomment it
#define menu x
#define config x

/***************************************************************************/
//                   Headers:  gadcon requirements
/***************************************************************************/
static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style);
static void _gc_shutdown(E_Gadcon_Client *gcc);
static void _gc_orient(E_Gadcon_Client *gcc);
static const char *_gc_label(void);
static Evas_Object *_gc_icon(Evas *evas);
static const char *_gc_id_new(void);
//static Config_Item *_config_item_get(const char *id);
/* and actually define the gadcon class that this module provides (just 1) */
static const E_Gadcon_Client_Class _gadcon_class =
{
   GADCON_CLIENT_CLASS_VERSION,
   "HOHO",  /*name of the module*/
   {
      _gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, _gc_id_new, NULL
   },
   E_GADCON_CLIENT_STYLE_PLAIN
};
/**/
/***************************************************************************/

/***************************************************************************/
//                   Headers:  user headers
/***************************************************************************/
#ifdef menu
static void cb_add_a_lang(void *data, E_Menu *m, E_Menu_Item *mi);
static void cb_remove_a_lang(void *data, E_Menu *m, E_Menu_Item *mi);
static void cb_english_lang(void *data, E_Menu *m, E_Menu_Item *mi);
static void cb_french_lang(void *data, E_Menu *m, E_Menu_Item *mi);
#endif
static void _button_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _menu_cb_post(void *data, E_Menu *m);
/***************************************************************************/

/***************************************************************************/
#ifdef config
static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;
static E_Action *act = NULL;
#endif
/***************************************************************************/

/***************************************************************************/
//                Module specifics
/***************************************************************************/
typedef struct _Instance Instance;
#ifdef config
typedef struct _Config_Item Config_Item;
#endif

struct _Instance
{
   E_Gadcon_Client *gcc;
   Evas_Object     *o_button;
#ifdef menu
   E_Menu          *main_menu;
   E_Menu          *another_menu;
   E_Menu          *sub_menu;
   E_Gadcon_Popup		*popup;
#endif
#ifdef config
   Config_Item     *ci;
#endif
};

#ifdef config
struct _Config_Item
{
   const char *id;
   int lang_selected;
};
#endif


static E_Module *mod_HOHO = NULL;

   static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style)
{
   Evas_Object *o;
   E_Gadcon_Client *gcc;
   Instance *inst;
   char buf[4096];
   //char name;
   inst = E_NEW(Instance, 1);
   char l;

   snprintf (buf, sizeof (buf), "%s/HOHO.edj",
         e_module_dir_get (mod_HOHO));

   o = edje_object_add(gc->evas);
   e_theme_edje_object_set(o, "base/theme/modules/HOHO", "e/modules/HOHO/main");
   edje_object_file_set (o, buf, "e/modules/HOHO/main");

   //evas_object_show (o); // needed ?
   edje_object_signal_emit(o, "e,state,unfocused", "e");

   gcc = e_gadcon_client_new(gc, name, id, style, o);
   gcc->data = inst;

   inst->gcc = gcc;
   inst->o_button = o;
#ifdef menu
   inst->main_menu = NULL;
#endif

   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN,
         _button_cb_mouse_down, inst);
   return gcc;
}

/*this is what happens when you unload the module*/
   static void
_gc_shutdown(E_Gadcon_Client *gcc)
{
   Instance *inst;

   inst = gcc->data;
#ifdef menu
   if (inst->main_menu)
   {
      e_menu_post_deactivate_callback_set(inst->main_menu, NULL, NULL);
      e_object_del(E_OBJECT(inst->main_menu));
      inst->main_menu = NULL;
   }
#endif
   evas_object_del(inst->o_button);
   free(inst);
}

/*when module is loaded it takes of up possition below*/
   static void
_gc_orient(E_Gadcon_Client *gcc)
{
   Instance *inst;
   Evas_Coord mw, mh;

   inst = gcc->data;
   mw = 0, mh = 0;
   edje_object_min_size_get(inst->o_button, &mw, &mh);
   if ((mw < 1) || (mh < 1))
      edje_object_min_size_calc(inst->o_button, &mw, &mh);
   if (mw < 4) mw = 4;
   if (mh < 4) mh = 4;
   e_gadcon_client_aspect_set(gcc, mw, mh);
   e_gadcon_client_min_size_set(gcc, mw, mh);
}

/*when module is loaded it has this name*/
   static const char *
_gc_label(void)
{
   return ("HOHO"); /*this name shows up in Shelf contents*/
}

/*this is the icon for the module*/
   static Evas_Object *
_gc_icon(Evas *evas)
{
   Evas_Object *o;
   char buf[4096];

   o = edje_object_add(evas);
   snprintf(buf, sizeof(buf), "%s/e-module-HOHO.edj",
         e_module_dir_get(mod_HOHO));
   edje_object_file_set(o, buf, "icon");
   return o;
}

   static const char *
_gc_id_new(void)
{
   return _gadcon_class.name;
}

/***************************************************************************/

/***************************************************************************/
/*modules callback*/

   static void
_button_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Instance *inst;
   Evas_Event_Mouse_Down *ev;
   inst = data;
   ev = event_info;

   if (ev->button == 1)
   {
      E_Menu *mn;
      E_Menu *sm;
      E_Menu_Item *mi;

      Evas_Coord x, y, w, h;
      int cx, cy, cw, ch;
      evas_object_geometry_get(inst->o_button, &x, &y, &w, &h);
      e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon,
            &cx, &cy, &cw, &ch);
      x += cx;
      y += cy;
#ifdef menu
      if (!inst->main_menu)
         inst->main_menu = e_menu_new(); /*e_int_menus_main_new();*/
      e_menu_title_set((inst->main_menu), "text");

      mi = e_menu_item_new( inst->main_menu);
      e_menu_item_label_set(mi, ("Add A Language"));
      e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");
      e_menu_item_callback_set(mi, cb_add_a_lang, inst);

      mi = e_menu_item_new( inst->main_menu);
      e_menu_item_label_set(mi, ("Remove A Language"));
      e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");
      e_menu_item_callback_set(mi, cb_remove_a_lang, inst);

      mi = e_menu_item_new( inst->main_menu);
      e_menu_item_label_set(mi, ("Translate"));
      e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");

      mi = e_menu_item_new( inst->main_menu);
      e_menu_item_separator_set(mi, 1); /*menu seperator*/

      if (!inst->sub_menu)
         inst->sub_menu = e_menu_new();/*e_int_menus_main_new();*/
      e_menu_title_set((inst->main_menu), "text");

      mi = e_menu_item_new( inst->main_menu);
      e_menu_item_label_set(mi, ("Language Select"));
      e_menu_item_submenu_set(mi, inst->sub_menu);
      e_util_menu_item_edje_icon_set(mi, "enlightenment/intl");

      /*default language*/
      mi = e_menu_item_new( inst->sub_menu);
      e_menu_item_label_set(mi, (e_intl_language_get()));
      e_menu_item_radio_set(mi, 1);
      e_menu_item_radio_group_set(mi, 1);
      e_menu_item_toggle_set(mi, 1);
      e_menu_item_callback_set(mi, cb_french_lang, inst);
      e_util_menu_item_edje_icon_set(mi, "enlightenment/e");

      mi = e_menu_item_new( inst->sub_menu);
      e_menu_item_separator_set(mi, 1);

      mi = e_menu_item_new( inst->sub_menu);
      e_menu_item_label_set(mi, ("Spanish"));
      e_menu_item_radio_set(mi, 1);
      e_menu_item_radio_group_set(mi, 1);
      // if (val == 1) e_menu_item_toggle_set(mi, 1);
      e_menu_item_callback_set(mi, cb_english_lang, inst);
      e_util_menu_item_edje_icon_set(mi, "enlightenment/e");

      if (inst->main_menu)
      {
         int dir;

         e_menu_post_deactivate_callback_set(inst->main_menu,
               _menu_cb_post,
               inst);
         dir = E_MENU_POP_DIRECTION_AUTO;
         switch (inst->gcc->gadcon->orient)
         {
            case E_GADCON_ORIENT_TOP:
               dir = E_MENU_POP_DIRECTION_DOWN;
               break;
            case E_GADCON_ORIENT_BOTTOM:
               dir = E_MENU_POP_DIRECTION_UP;
               break;
            case E_GADCON_ORIENT_LEFT:
               dir = E_MENU_POP_DIRECTION_RIGHT;
               break;
            case E_GADCON_ORIENT_RIGHT:
               dir = E_MENU_POP_DIRECTION_LEFT;
               break;
            case E_GADCON_ORIENT_CORNER_TL:
               dir = E_MENU_POP_DIRECTION_DOWN;
               break;
            case E_GADCON_ORIENT_CORNER_TR:
               dir = E_MENU_POP_DIRECTION_DOWN;
               break;
            case E_GADCON_ORIENT_CORNER_BL:
               dir = E_MENU_POP_DIRECTION_UP;
               break;
            case E_GADCON_ORIENT_CORNER_BR:
               dir = E_MENU_POP_DIRECTION_UP;
               break;
            case E_GADCON_ORIENT_CORNER_LT:
               dir = E_MENU_POP_DIRECTION_RIGHT;
               break;
            case E_GADCON_ORIENT_CORNER_RT:
               dir = E_MENU_POP_DIRECTION_LEFT;
               break;
            case E_GADCON_ORIENT_CORNER_LB:
               dir = E_MENU_POP_DIRECTION_RIGHT;
               break;
            case E_GADCON_ORIENT_CORNER_RB:
               dir = E_MENU_POP_DIRECTION_LEFT;
               break;
            case E_GADCON_ORIENT_FLOAT:
            case E_GADCON_ORIENT_HORIZ:
            case E_GADCON_ORIENT_VERT:
            default:
               dir = E_MENU_POP_DIRECTION_AUTO;
               break;
         }
         e_menu_activate_mouse(inst->main_menu,
               e_util_zone_current_get(e_manager_current_get()),
               x, y, w, h,
               dir, ev->timestamp);
         edje_object_signal_emit(inst->o_button, "e,state,focused", "e");
      }
   }
#endif

#ifdef menu
   if (ev->button == 3)
   {
      Evas_Coord x, y, w, h;
      int cx, cy, cw, ch;

      evas_object_geometry_get(inst->o_button, &x, &y, &w, &h);
      e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon,
            &cx, &cy, &cw, &ch);
      x += cx;
      y += cy;

      if (!inst->another_menu); // FIXME ???

      E_Menu_Item *mi;
      inst->another_menu = e_menu_new();
      e_menu_title_set((inst->another_menu), "text");

      mi = e_menu_item_new( inst->another_menu);
      e_menu_item_label_set(mi, ("Configuration for HOHO module"));
      e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");

      e_gadcon_client_util_menu_items_append(inst->gcc, inst->another_menu, 0);
      e_menu_activate_mouse(inst->another_menu,
            e_util_zone_current_get(e_manager_current_get()),
            x, y, w, h,
            E_MENU_POP_DIRECTION_AUTO, ev->timestamp);
      edje_object_signal_emit(inst->o_button, "e,state,focused", "e");
   }
#endif
}

#ifdef menu
   static void
_menu_cb_post(void *data, E_Menu *m)
{
   Instance *inst;

   inst = data;
   if (!inst->main_menu) return;
   edje_object_signal_emit(inst->o_button, "e,state,unfocused", "e");
   e_object_del(E_OBJECT(inst->main_menu));
   e_object_del(E_OBJECT(inst->sub_menu));
   inst->main_menu = NULL;
   inst->sub_menu = NULL;
   if (!inst->another_menu) return;
   edje_object_signal_emit(inst->o_button, "e,state,unfocused", "e");
   e_object_del(E_OBJECT(inst->another_menu));
   inst->another_menu = NULL;
}
#endif

/***************************************************************************/

/***************************************************************************/
/**/
/* module setup */
EAPI E_Module_Api e_modapi =
{
   E_MODULE_API_VERSION,
   "HOHO" /*another name of the module*/
};

   EAPI void *
e_modapi_init(E_Module *m)
{
   mod_HOHO = m;

   e_gadcon_provider_register(&_gadcon_class);
   return m;
}

   EAPI int
e_modapi_shutdown(E_Module *m)
{
   mod_HOHO = NULL;

   e_gadcon_provider_unregister(&_gadcon_class);
   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   //e_config_domain_save("module.lang", conf_edd, mod_HOHO);
   return 1;
}

/***************************************************************************/

/***************************************************************************/
#ifdef menu
static void cb_add_a_lang(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Ecore_Exe *exe;
   int status;
   exe = ecore_exe_run("thunar &", NULL);
   waitpid(ecore_exe_pid_get(exe), &status, 0);
   ecore_exe_free(exe);
}


static void cb_remove_a_lang(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Ecore_Exe *exe;
   int status;
   exe = ecore_exe_run("thunar &", NULL);
   waitpid(ecore_exe_pid_get(exe), &status, 0);
   ecore_exe_free(exe);
}

static void cb_english_lang(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Instance *inst;
   inst = data;
   e_intl_language_set("fr_FR.UTF-8");
}

static void cb_french_lang(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Instance *inst;
   inst = data;
}
#endif
/***************************************************************************/
