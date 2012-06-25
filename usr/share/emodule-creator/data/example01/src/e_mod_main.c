#include <e.h>
#include "e_mod_main.h"

/***************************************************************************/
/**/
/* gadcon requirements */
static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style);
static void _gc_shutdown(E_Gadcon_Client *gcc);
static void _gc_orient(E_Gadcon_Client *gcc);
static char *_gc_label(void);
static Evas_Object *_gc_icon(Evas *evas);
static const char *_gc_id_new(void);
/* and actually define the gadcon class that this module provides (just 1) */
static const E_Gadcon_Client_Class _gadcon_class =
{
   GADCON_CLIENT_CLASS_VERSION,
   "HOHO", /* name of the module */
   {
      _gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, _gc_id_new, NULL
   },
   E_GADCON_CLIENT_STYLE_PLAIN
};
/**/
/***************************************************************************/

/***************************************************************************/
static void _button_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _menu_cb_post(void *data, E_Menu *m);
/***************************************************************************/

/***************************************************************************/
/***************************************************************************/

/***************************************************************************/
/**/
/* actual module specifics */

typedef struct _Instance Instance;
//typedef struct _Config_Item Config_Item;

struct _Instance
{
   E_Gadcon_Client *gcc;
   Evas_Object     *o_button;
   E_Menu          *main_menu;
};

/* 
struct _Config_Item 
{
   const char *id;
   int lang_selected;
};
*/

static E_Module *HOHO = NULL;

   static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style)
{
   Evas_Object *o;
   E_Gadcon_Client *gcc;
   Instance *inst;

   char buf[4096];
   inst = E_NEW(Instance, 1);

   snprintf (buf, sizeof (buf), "%s/HOHO.edj",
    e_module_dir_get (HOHO));

   o = edje_object_add(gc->evas);
   e_theme_edje_object_set(o, "base/theme/modules/HOHO", "e/modules/HOHO/main");
   edje_object_file_set (o, buf, "e/modules/HOHO/main");
   evas_object_show (o); // FIXME ? remove ?
   edje_object_signal_emit(o, "e,state,unfocused", "e");

   gcc = e_gadcon_client_new(gc, name, id, style, o);
   gcc->data = inst;

   inst->gcc = gcc;
   inst->o_button = o;
   inst->main_menu = NULL;

   e_gadcon_client_util_menu_attach(gcc);

   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN,
	 _button_cb_mouse_down, inst);
   return gcc;
}

/* this is what happens when you unload the module */
static void
_gc_shutdown(E_Gadcon_Client *gcc)
{
   Instance *inst;

   inst = gcc->data;
   if (inst->main_menu)
     {
	e_menu_post_deactivate_callback_set(inst->main_menu, NULL, NULL);
	e_object_del(E_OBJECT(inst->main_menu));
	inst->main_menu = NULL;
     }
   evas_object_del(inst->o_button);
   free(inst);
}

   static void
_gc_orient(E_Gadcon_Client *gcc)
{
   Instance *inst;

   inst = gcc->data;
   e_gadcon_client_aspect_set(gcc, 16, 16);
   e_gadcon_client_min_size_set(gcc, 16, 16);

}

   static char *
_gc_label(void)
{
   return ("HOHO");
}

   static Evas_Object *
_gc_icon(Evas *evas)
{
   Evas_Object *o;
   char buf[4096];

   o = edje_object_add(evas);
   snprintf(buf, sizeof(buf), "%s/e-module-HOHO.edj",
	 e_module_dir_get(HOHO));
   edje_object_file_set(o, buf, "icon");
   return o;
}

   static const char *
_gc_id_new(void)
{
   return _gadcon_class.name;
}

/**/
/***************************************************************************/

/***************************************************************************/
/*static Config_Item *
_config_item_get(const char *id) 
{
 //  Eina_List   *l;
   Config_Item *ci;
 //  char buf[128];
//   ci->lang_selected = 0;
}*/
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
	Evas_Coord x, y, w, h;
	int cx, cy, cw, ch;

	evas_object_geometry_get(inst->o_button, &x, &y, &w, &h); 
	e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon,
	      &cx, &cy, &cw, &ch);
	x += cx;
	y += cy;
	if (!inst->main_menu)
      printf("Showing menu now because is not showing up\n");
	  //inst->main_menu = e_int_menus_main_new();
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
}

   static void
_menu_cb_post(void *data, E_Menu *m)
{
   Instance *inst;

   inst = data;
   if (!inst->main_menu) return;
   edje_object_signal_emit(inst->o_button, "e,state,unfocused", "e");
   e_object_del(E_OBJECT(inst->main_menu));
   inst->main_menu = NULL;
}
/**/
/***************************************************************************/

/***************************************************************************/
/**/
/* module setup */
EAPI E_Module_Api e_modapi = 
{
   E_MODULE_API_VERSION,
   "HOHO"
};

   EAPI void *
e_modapi_init(E_Module *m)
{
   HOHO = m;

   e_gadcon_provider_register(&_gadcon_class);
   return m;
}

   EAPI int
e_modapi_shutdown(E_Module *m)
{
   HOHO = NULL;

   e_gadcon_provider_unregister(&_gadcon_class);
   return 1;
}

   EAPI int
e_modapi_save(E_Module *m)
{
   // e_config_domain_save("module.(HOHO ?)", conf_edd, HOHO);
   return 1;
}

/**/
/***************************************************************************/

