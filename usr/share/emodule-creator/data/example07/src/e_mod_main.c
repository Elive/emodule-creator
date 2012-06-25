#include <e.h>
#include "e_mod_main.h"

/* Local Function Prototypes */
static void _HOHO_conf_new(void);
static void _HOHO_conf_free(void);
static Eina_Bool _HOHO_conf_timer(void *data);
static Eina_Bool _HOHO_configure(void);

/* Local Structures */

/* Local Variables */
static E_Config_DD *conf_edd = NULL;
Config *HOHO_conf = NULL;

/* We set the version and the name, check e_mod_main.h for more details */
EAPI E_Module_Api e_modapi = {E_MODULE_API_VERSION, "HOHO"};

/*
 * Module Functions
 */

/* Function called when the module is initialized */
EAPI void *
e_modapi_init(E_Module *m)
{
   char buf[4096];

   /* Location of message catalogs for localization */
   snprintf(buf, sizeof(buf), "%s/locale", e_module_dir_get(m));
   bindtextdomain(PACKAGE, buf);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

   /* Location of theme to load for this module */
   snprintf(buf, sizeof(buf), "%s/e-module-HOHO.edj", m->dir);


   /* Display this Modules config info in the main Config Panel */

   /* starts with a category, create it if not already exists */
   e_configure_registry_category_add("features", 80, D_("Features"),
                                     NULL, "preferences-applications-add");
   /* add right-side item */
   e_configure_registry_item_add("features/HOHO", 110, D_("HOHOHO"),
                                 NULL, buf, _HOHO_configure);

   conf_edd = E_CONFIG_DD_NEW("Config", Config);
   #undef T
   #undef D
   #define T Config
   #define D conf_edd
   E_CONFIG_VAL(D, T, version, INT);
   E_CONFIG_VAL(D, T, enabled, UCHAR); /* our var from header */

   /* Tell E to find any existing module data. First run ? */
   HOHO_conf = e_config_domain_load("module.HOHO", conf_edd);
   if (HOHO_conf)
     {
        /* Check config version */
        if ((HOHO_conf->version >> 16) < MOD_CONFIG_FILE_EPOCH)
          {
             /* config too old */
             _HOHO_conf_free();
	     ecore_timer_add(1.0, _HOHO_conf_timer,
			     "HOHO Module Configuration data needed "
			     "upgrading. Your old configuration<br> has been"
			     " wiped and a new set of defaults initialized. "
			     "This<br>will happen regularly during "
			     "development, so don't report a<br>bug. "
			     "This simply means the module needs "
			     "new configuration<br>data by default for "
			     "usable functionality that your old<br>"
			     "configuration simply lacks. This new set of "
			     "defaults will fix<br>that by adding it in. "
			     "You can re-configure things now to your<br>"
			     "liking. Sorry for the inconvenience.<br>");
          }

        /* Ardvarks */
        else if (HOHO_conf->version > MOD_CONFIG_FILE_VERSION)
          {
             /* config too new...wtf ? */
             _HOHO_conf_free();
	     ecore_timer_add(1.0, _HOHO_conf_timer,
			     "Your HOHO Module configuration is NEWER "
			     "than the module version. This is "
			     "very<br>strange. This should not happen unless"
			     " you downgraded<br>the module or "
			     "copied the configuration from a place where"
			     "<br>a newer version of the module "
			     "was running. This is bad and<br>as a "
			     "precaution your configuration has been now "
			     "restored to<br>defaults. Sorry for the "
			     "inconvenience.<br>");
          }
     }

   /* if we don't have a config yet, or it got erased above,
    * then create a default one */
   if (!HOHO_conf) _HOHO_conf_new();

   /* create a link from the modules config to the module
    * this is not written */
   HOHO_conf->module = m;

   // Run the launcher at everytime we start e17
   //if (HOHO_conf->enabled == 1)
   //   ecore_timer_add(0, _HOHO_exec, NULL);

   /* Give E the module */
   return m;
}

/*
 * Function to unload the module
 */
EAPI int
e_modapi_shutdown(E_Module *m)
{
   /* Unregister the config dialog from the main panel */
   e_configure_registry_item_del("features/HOHO");

   /* Remove the config panel category if we can. E will tell us.
    category stays if other items using it */
   e_configure_registry_category_del("features");

   /* Terminate external process if any */
   //char stopper[4096];
   //snprintf(stopper, sizeof(stopper), "%s/scripts/launcher.sh terminate", HOHO_conf->module->dir);
   //ecore_exe_run(stopper, NULL);

   /* Tell E the module is now unloaded. Gets removed from shelves, etc. */
   HOHO_conf->module = NULL;

   /* Cleanup the main config structure */
   E_FREE(HOHO_conf);

   /* Clean EET */
   E_CONFIG_DD_FREE(conf_edd);
   return 1;
}

/*
 * Function to Save the modules config
 */
EAPI int
e_modapi_save(E_Module *m)
{
   e_config_domain_save("module.HOHO", conf_edd, HOHO_conf);
   return 1;
}

/* Local Functions */
/* new module needs a new config :), or config too old and we need one anyway */
static void
_HOHO_conf_new(void)
{
   char buf[128];

   HOHO_conf = E_NEW(Config, 1);
   HOHO_conf->version = (MOD_CONFIG_FILE_EPOCH << 16);

#define IFMODCFG(v) if ((HOHO_conf->version & 0xffff) < v) {
#define IFMODCFGEND }

   /* setup defaults */
   IFMODCFG(0x008d);
   HOHO_conf->enabled = 1;
   IFMODCFGEND;

   /* update the version */
   HOHO_conf->version = MOD_CONFIG_FILE_VERSION;

   /* setup limits on the config properties here (if needed) */

   /* save the config to disk */
   e_config_save_queue();
}

/* This is called when we need to cleanup the actual configuration,
 * for example when our configuration is too old */
static void
_HOHO_conf_free(void)
{
   E_FREE(HOHO_conf);
}

/* timer for the config oops dialog (old configuration needs update) */
static Eina_Bool
_HOHO_conf_timer(void *data)
{
   e_util_dialog_internal( D_("HOHOHO Configuration Updated"), data);
   return EINA_FALSE;
}

/* Launcher Configure */
static Eina_Bool
_HOHO_configure(void)
{
   char launcher[4096];

   snprintf(launcher, sizeof(launcher), "%s/scripts/launcher.sh conf", HOHO_conf->module->dir);

   ecore_exe_run(launcher, NULL);

   return ECORE_CALLBACK_CANCEL; // 0
}

