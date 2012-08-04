#include <e.h>
#include "e_mod_main.h"

static void _skeletor_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _skeletor_menu_cb_post(void *data, E_Menu *m);
static void _skeletor_menu_cb_cfg(void *data, E_Menu *menu, E_Menu_Item *mi);
/***************************************************************************/
/* actual module specifics */

typedef struct _Instance Instance;

struct _Instance
{
   E_Gadcon_Client *gcc;
   Config_Item     *cfg;
};

Config *skeletor_config = NULL;

static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;

static Config_Item *
_conf_item_get(const char *id)
{
    Config_Item *ci;

    ci = E_NEW(Config_Item, 1);
    ci->id = eina_stringshare_add(id);
    skeletor_config->items = eina_list_append(skeletor_config->items, ci);
    e_config_save_queue();

    return ci;
}

/* menu item callback(s) */
static void 
_e_mod_run_cb(void *data __UNUSED__, E_Menu *m, E_Menu_Item *mi __UNUSED__)
{
   e_configure_registry_call("elive/skeletor", m->zone->container, NULL);
}

/* menu item add hook */
static void
_e_mod_menu_add(void *data __UNUSED__, E_Menu *m)
{
   E_Menu_Item *mi;

   mi = e_menu_item_new(m);
   e_menu_item_label_set(mi, _("Elive"));
   e_util_menu_item_theme_icon_set(mi, "preferences-desktop-shelf");
   e_menu_item_callback_set(mi, _e_mod_run_cb, NULL);
}

static Eina_Bool
_skeletor_exec()
{
    char buf[PATH_MAX];
    snprintf(buf, sizeof(buf), "%s/scripts/launcher.sh run",
            e_module_dir_get(skeletor_config->module));
    ecore_exe_run(buf,NULL);
}

static Eina_Bool
_skeletor_del_cb(void *data, int type, void *event)
{
    Ecore_Exe_Event_Del *ev;

    ev = event;
    skeletor_config = data;

    if(skeletor_config)
        ecore_event_handler_del(skeletor_config->eeh);

    if(ev->exit_code == 0)
    {
        _skeletor_exec();
        ecore_event_handler_del(skeletor_config->eeh);
        return ECORE_CALLBACK_CANCEL;
    }

    return ECORE_CALLBACK_CANCEL;
}

Eina_Bool
_skeletor_launcher(void *data __UNUSED__)
{
    E_Module *m;

    m = e_module_find("systray");
    if(!(m) && (e_module_enabled_get(m)))
    {
        e_util_dialog_internal("Systray Not Found",
                "Skeletor depends of the systray module, \
                you need<br>to enable that module first and have \
                it visually on your desktop");
        return ECORE_CALLBACK_CANCEL;
    }

    m = e_module_find("notification");
    if((m) && (e_module_enabled_get(m)))
    {
         ecore_exe_run("notify-send -i xterm \"Autorun\" \
                \"E Module running command like wild cherries \"",NULL);
    }
    else
    {
        e_util_dialog_internal("Notification Not Found",
                "Skeletor depends of the notification module, \
                you need<br>to enable that module first and have \
                it visually on your desktop");
        return ECORE_CALLBACK_CANCEL;
    }

    if(skeletor_config->set == 1)
    {
        char buf[PATH_MAX];
        skeletor_config->eeh =
            ecore_event_handler_add(ECORE_EXE_EVENT_DEL,
                    _skeletor_del_cb,skeletor_config);

        snprintf(buf, sizeof(buf), "%s/scripts/launcher.sh autodetect",
                e_module_dir_get(skeletor_config->module));

        ecore_exe_run(buf, NULL);

        return ECORE_CALLBACK_CANCEL;
    }
    else
    {
        _skeletor_exec();
        return ECORE_CALLBACK_CANCEL;
    }

    _skeletor_exec();
    return ECORE_CALLBACK_CANCEL;
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
    /* Note-to-self, if you add a catagory, and you did not add any item
     * to it , IT WILL NOT BE DISPLAYED!!! */
    e_configure_registry_category_add("elive", 32, "Elive", NULL,
            NULL);

    e_configure_registry_item_add("elive/skeletor", 10,
            "Skeletor Configuration", NULL,
            "preferences-system-screen-resolution",e_int_config_skeletor_module);


    conf_item_edd = E_CONFIG_DD_NEW("Config_Item", Config_Item);
#undef T
#undef D
#define T Config_Item
#define D conf_item_edd
    E_CONFIG_VAL(D, T, id, STR);

    conf_edd = E_CONFIG_DD_NEW("Config", Config);
#undef T
#undef D
#define T Config
#define D conf_edd
    E_CONFIG_LIST(D, T, items, conf_item_edd);
    E_CONFIG_VAL(D, T, set, INT);

    skeletor_config = e_config_domain_load("module.skeletor", conf_edd);


    if(!skeletor_config)
        skeletor_config = E_NEW(Config, 1);
    
    skeletor_config->maug =
        e_int_menus_menu_augmentation_add_sorted("config/1", _("Elive"),
                _e_mod_menu_add, NULL, NULL, NULL);

    skeletor_config->module = m;
    e_module_delayed_set(m, 1);

    ecore_timer_add(0, _skeletor_launcher,
            skeletor_config->module);

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
    char buf[PATH_MAX];
    
    snprintf(buf, sizeof(buf), "%s/scripts/launcher.sh terminate",
            e_module_dir_get(skeletor_config->module));
    ecore_exe_run(buf, NULL);

    if(skeletor_config->maug)
        e_int_menus_menu_augmentation_del("config/1",skeletor_config->maug);
    
    while((skeletor_config->cfd = e_config_dialog_get("E", "elive/skeletor")))
        e_object_del(E_OBJECT(skeletor_config->cfd));

    e_configure_registry_item_del("elive/skeletor");
    e_configure_registry_category_del("elive");

    if(skeletor_config->cfd)
        e_object_del(E_OBJECT(skeletor_config->cfd));
    E_FREE(skeletor_config->cfd);

    if(skeletor_config->eeh)
        ecore_event_handler_del(skeletor_config->eeh);
    skeletor_config->eeh == NULL;

    if(skeletor_config)
    {
        EINA_LIST_FREE(skeletor_config->items, ci)
        {
            eina_stringshare_del(ci->id);
            free(ci);
        }
        skeletor_config->module = NULL;
        E_FREE(skeletor_config);
    }
    E_CONFIG_DD_FREE(conf_edd);
    E_CONFIG_DD_FREE(conf_item_edd);

    return 1;
}

/*
 * e_modapi_save is used to save and store configuration info on local
 * storage
 */
EAPI int
e_modapi_save(E_Module *m __UNUSED__)
{
    e_config_domain_save("module.skeletor", conf_edd, skeletor_config);
    return 1;
}

/**/
/***************************************************************************/

