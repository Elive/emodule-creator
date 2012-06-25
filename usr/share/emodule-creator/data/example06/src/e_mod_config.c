#include <e.h>
#include "e_mod_main.h"

struct _E_Config_Dialog_Data 
{
   int enabled;
   int autodetect;
   int startup_delay;
};

/* Local Function Prototypes */
static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void _fill_data(E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);

extern int HOHO_launcher(void);

/* External Functions */
/* Function for calling our personal dialog menu */
EAPI E_Config_Dialog *
e_int_config_HOHO_module(E_Container *con) 
{
   E_Config_Dialog *cfd = NULL;
   E_Config_Dialog_View *v = NULL;
   char buf[4096];

   /* is this config dialog already visible ? */
   if (e_config_dialog_find("HOHO", "_e_module_HOHO_cfg_dlg")) return NULL;

   v = E_NEW(E_Config_Dialog_View, 1);
   if (!v) return NULL;

   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.create_widgets = _basic_create;
   v->basic.apply_cfdata = _basic_apply;

   /* Icon in the theme */
   snprintf(buf, sizeof(buf), "%s/e-module-HOHO.edj", HOHO_conf->module->dir);

   /* create new config dialog */
   cfd = e_config_dialog_new(con, D_("HOHOHO Module"), "HOHO", 
                             "features/HOHO", buf, 0, v, NULL);

   e_dialog_resizable_set(cfd->dia, 1);
   HOHO_conf->cfd = cfd;
   return cfd;
}

/* Local Functions */
static void *
_create_data(E_Config_Dialog *cfd) 
{
   E_Config_Dialog_Data *cfdata = NULL;

   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   _fill_data(cfdata);
   return cfdata;
}

static void 
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata) 
{
   HOHO_conf->cfd = NULL;
   E_FREE(cfdata);
}

static void 
_fill_data(E_Config_Dialog_Data *cfdata) 
{
   /* load a temp copy of the config variables */
   cfdata->enabled = HOHO_conf->enabled;
   cfdata->autodetect = HOHO_conf->autodetect;
   cfdata->startup_delay = HOHO_conf->startup_delay;
}

static Evas_Object *
_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata) 
{
   Evas_Object *o = NULL, *of = NULL, *ow = NULL;

   o = e_widget_list_add(evas, 0, 0);

   /*  
   // FIXME: do you want to add a Description widget ?
   ow = e_widget_textblock_add(evas);
   e_widget_size_min_set(ow, 140 * e_scale, 50 * e_scale);
   e_widget_textblock_markup_set(ow, D_("This feature enables some launchers for different actions like<br>hardware events of media inserted, for example to play a<br>DVD or a Audio CD when inserted, it also automount devices"));
   e_widget_list_object_append (o, ow, 1, 0, 0.5);
   */


   of = e_widget_framelist_add(evas, D_("HOHOHO Configurations"), 0);
   e_widget_framelist_content_align_set(of, 0.0, 0.0);

   ow = e_widget_check_add(evas, D_("Enable HOHOHO"), 
                           &(cfdata->enabled));
   e_widget_framelist_object_append(of, ow);

   ow = e_widget_label_add (evas, D_("Small Description (no textblock)"));
   e_widget_framelist_object_append (of, ow);

   ow = e_widget_check_add(evas, D_("Only run it if is autodetected"), 
                           &(cfdata->autodetect));
   e_widget_framelist_object_append(of, ow);


   ow = e_widget_label_add (evas, D_("Delay in seconds to enable it after to login"));
   e_widget_framelist_object_append (of, ow);

   ow = e_widget_slider_add(evas, 1, 0, ("%1.0f"), 1, 120, 1, 0, NULL,
                           &(cfdata->startup_delay), 200);
   e_widget_framelist_object_append(of, ow);


   e_widget_list_object_append(o, of, 1, 1, 0.5);

   return o;
}


static int 
_basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata) 
{
   HOHO_conf->enabled = cfdata->enabled;
   HOHO_conf->autodetect = cfdata->autodetect;
   HOHO_conf->startup_delay = cfdata->startup_delay;
   e_config_save_queue();

   if(HOHO_conf->enabled == 1)
      ecore_timer_add(HOHO_conf->startup_delay, HOHO_launcher, NULL);
   else
   {
      /* Terminate external process if any */
      char stopper[4096];
      snprintf(stopper, sizeof(stopper), "%s/scripts/launcher.sh terminate", HOHO_conf->module->dir);
      ecore_exe_run(stopper, NULL);
   }

   return 1;
}
