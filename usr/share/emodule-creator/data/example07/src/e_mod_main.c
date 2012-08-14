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
    //e_configure_registry_category_add("elive", 32, "Elive", NULL,
    //        NULL);

    e_configure_registry_item_add("preferences/skeletor", 10,
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
    
    skeletor_config->module = m;
    e_module_delayed_set(m, 1);

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
    
    while((skeletor_config->cfd = e_config_dialog_get("E", "preferences/skeletor")))
        e_object_del(E_OBJECT(skeletor_config->cfd));

    e_configure_registry_item_del("preferences/skeletor");

    if(skeletor_config->cfd)
        e_object_del(E_OBJECT(skeletor_config->cfd));
    E_FREE(skeletor_config->cfd);

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

