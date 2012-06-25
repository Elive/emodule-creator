#include <e.h>
#include "e_mod_main.h"

/* Local Function Prototypes */
static void _HOHO_conf_new(void);
static void _HOHO_conf_free(void);
static int _HOHO_conf_timer(void *data);
static int _HOHO_del_cb(void *data, int type, void *event);
static void _HOHO_cb_menu_post(void *data, E_Menu *menu);
static void _HOHO_cb_menu_configure(void *data, E_Menu *mn, E_Menu_Item *mi);
extern int HOHO_launcher(void);

/* Local Structures */
typedef struct _Handy Handy;
struct _Handy
{
   /* Our handler for the checker command */
   Ecore_Event_Handler  *handler;
   Ecore_Exe            *gen_exe;
};

typedef struct _Instance Instance;
struct _Instance 
{
   /* evas_object used to display */
   Evas_Object          *o_HOHO;

   /* popup anyone ? */
   E_Menu               *menu;
};

struct _E_Config_Dialog_Data 
{
   int enabled;
   int autodetect;
   int startup_delay;
};

/* Local Variables */
static int uuid = 0;
static Eina_List *instances = NULL;
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
                                 NULL, buf, e_int_config_HOHO_module);

   conf_edd = E_CONFIG_DD_NEW("Config", Config);
   #undef T
   #undef D
   #define T Config
   #define D conf_edd
   E_CONFIG_VAL(D, T, version, INT);
   E_CONFIG_VAL(D, T, enabled, UCHAR); /* our var from header */
   E_CONFIG_VAL(D, T, autodetect, UCHAR); /* our var from header */
   E_CONFIG_VAL(D, T, startup_delay, INT); /* our var from header */

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
   if (HOHO_conf->enabled == 1)
      ecore_timer_add(HOHO_conf->startup_delay, HOHO_launcher, NULL);

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

   /* Kill the config dialog */
   if (HOHO_conf->cfd) e_object_del(E_OBJECT(HOHO_conf->cfd));
   HOHO_conf->cfd = NULL;

   /* Terminate external process if any */
   char stopper[4096];
   snprintf(stopper, sizeof(stopper), "%s/scripts/launcher.sh terminate", HOHO_conf->module->dir);
   ecore_exe_run(stopper, NULL);

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
   HOHO_conf->autodetect = 1;
   HOHO_conf->startup_delay = 10;
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
static int 
_HOHO_conf_timer(void *data) 
{
   e_util_dialog_show( D_("HOHOHO Configuration Updated"), data);
   return 0;
}

/* popup menu closing, cleanup */
static void 
_HOHO_cb_menu_post(void *data, E_Menu *menu) 
{
   Instance *inst = NULL;

   if (!(inst = data)) return;
   if (!inst->menu) return;
   e_object_del(E_OBJECT(inst->menu));
   inst->menu = NULL;
}

/* call configure from popup */
static void 
_HOHO_cb_menu_configure(void *data, E_Menu *mn, E_Menu_Item *mi) 
{
   if (!HOHO_conf) return;
   if (HOHO_conf->cfd) return;
   e_int_config_HOHO_module(mn->zone->container);
}

/* Launcher Exec feature */
static int
_HOHO_exec()
{
   char launcher[4096];
   snprintf(launcher, sizeof(launcher), "%s/scripts/launcher.sh run", HOHO_conf->module->dir);

   ecore_exe_run(launcher, NULL);
}

/* Launcher autodetect */
/* function called when the HOHO checker command has finished */
static int
_HOHO_del_cb(void *data, int type, void *event)
{
   Ecore_Exe_Event_Del *ev;
   Handy *hand = NULL;

   ev = event;
   hand = data;

   if (hand)
   {
      ecore_event_handler_del(hand->handler);
      hand->handler = NULL;
      hand->gen_exe = NULL;
   }

   if (ev->exit_code == 0)
   {
      // HOHO returned:  OK (0)
      _HOHO_exec();
      E_FREE(hand);
      return ECORE_CALLBACK_CANCEL; // 0
   }
   // HOHO returned:  error (1?)
   E_FREE(hand);
   return ECORE_CALLBACK_CANCEL; // 0
}


/* Launch the launcher */
extern int
HOHO_launcher(void)
{
   // Check if we have the systray module enabled
   E_Module *m;
   m = e_module_find("systray");
   if (! ((m) && (e_module_enabled_get(m))))
   {
      e_util_dialog_show( D_("Systray Not Found"), D_("HOHOHO depends of the systray module, you need<br>to enable that module first and have it visually on your desktop"));
      return ECORE_CALLBACK_CANCEL; // 0
   } // FIXME: Do you need this check ?


   if (HOHO_conf->autodetect == 1)
     {
       // Run the autodetection and if exit-code is correct, he will run _HOHO_exec
       Handy *hand = NULL;
       hand = calloc(1, sizeof(hand));

       hand->handler = ecore_event_handler_add(ECORE_EXE_EVENT_DEL, _HOHO_del_cb, hand);

       char checker[4096];
       snprintf(checker, sizeof(checker), "%s/scripts/launcher.sh autodetect", HOHO_conf->module->dir);

       hand->gen_exe = ecore_exe_run(checker, hand);
       return ECORE_CALLBACK_CANCEL; // 0
     }
   else
     {
       // Run it directly, no autodetection required
       _HOHO_exec();
       return ECORE_CALLBACK_CANCEL; // 0
     }

   return ECORE_CALLBACK_CANCEL; // 0
}
