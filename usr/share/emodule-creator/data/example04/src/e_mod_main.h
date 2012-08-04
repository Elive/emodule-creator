#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#ifdef ENABLE_NLS
# include <libintl.h>
# define _(string) dgettext(PACKAGE, string)
#else
# define bindtextdomain(domain,dir)
# define bind_textdomain_codeset(domain, codeset)
# define _(string) (string)
#endif

EAPI extern E_Module_Api e_modapi;

EAPI void  *e_modapi_init       (E_Module *m);
EAPI int    e_modapi_shutdown   (E_Module *m);
EAPI int    e_modapi_save       (E_Module *m);

typedef struct _Config Config;
typedef struct _Config_Item Config_Item;
struct _Config
{
    Eina_List *items;
    Eina_List *instances;
    E_Config_Dialog *cfd;
    E_Module *module;
    int set;
};

struct _Config_Item
{
    const char *id;
};

E_Config_Dialog * e_int_config_skeletor_module(E_Container *con, const char *params);

extern Config *skeletor_config;
/**
 * @addtogroup Optional_Gadgets
 * @{
 *
 * @defgroup Module_Skeletor Skeletor
 *
 * This is an updated clone of the Skel module.
 *
 * @}
 */

#endif
