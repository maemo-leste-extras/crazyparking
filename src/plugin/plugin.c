/*
* This file is part of Crazyparking
*
* Copyright (C) 2005 INdT - Instituto Nokia de Tecnologia
* http://www.indt.org/maemo
*
* This software is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License
* as published by the Free Software Foundation; either version 2.1 of
* the License, or (at your option) any later version.
*
* This software is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this software; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
* 02110-1301 USA
*
*/ 


#include <stdio.h>
#include <gtk/gtk.h>
#include <startup_plugin.h>
#include <gconf/gconf.h>
#include <gconf/gconf-client.h>

#define SETTINGS_LEVEL "/apps/osso/crazyparking/level"
#define SETTINGS_SOUND "/apps/osso/crazyparking/sound"

#define MENU_SOUND     15

static GtkWidget *load_plugin               (void);
static void       unload_plugin             (void);
static void       write_config              (void);
static GtkWidget **load_menu                (guint *);
static void       update_menu               (void);
static void       plugin_callback           (GtkWidget   *menu_item,
                                             gpointer     data);                                             
static void       settings_callback         (GtkWidget   *widget,
                                             gpointer     data);
static void       sound_callback            (GtkWidget   *widget,
                                             gpointer     data);

GConfClient *gcc = NULL;
GtkWidget *board_box;
GtkWidget *level_1_item;
GtkWidget *level_2_item;
GtkWidget *level_3_item;
GtkWidget *level_4_item;
GtkWidget *level_5_item;
GtkWidget *level_6_item;
GtkWidget *level_7_item;
GtkWidget *level_8_item;
GtkWidget *level_9_item;
GtkWidget *level_10_item;
GtkWidget *level_11_item;
GtkWidget *level_12_item;
GtkWidget *settings_item;
GtkWidget *settings_menu;
GtkWidget *difficulty_item;
GtkWidget *difficulty_menu;
static GameStartupInfo gs;
GtkWidget *menu_items[2];
static int changed = FALSE;
GSList * group = NULL;
GtkWidget *sound_check = NULL;
GtkWidget *sound_item;

static StartupPluginInfo plugin_info = {
  load_plugin,
  unload_plugin,
  write_config,
  load_menu,
  update_menu,
  NULL
};


typedef enum {
	LEVEL_1 = 1,
	LEVEL_2,
	LEVEL_3,
	LEVEL_4,
	LEVEL_5,
	LEVEL_6,
	LEVEL_7,
	LEVEL_8,
	LEVEL_9,
	LEVEL_10,
	LEVEL_11,
	LEVEL_12
} LEVEL;

STARTUP_INIT_PLUGIN(plugin_info, gs, FALSE, FALSE)

static GtkWidget *
load_plugin (void)
{
  int board, enable_sound;
  GtkWidget *game_hbox;
  GtkWidget *board_hbox, *board_label;
  GtkWidget *sound_hbox, *sound_label;

  g_type_init();
  gcc = gconf_client_get_default();
  board = gconf_client_get_int(gcc, SETTINGS_LEVEL, NULL);
  enable_sound = gconf_client_get_int(gcc, SETTINGS_SOUND, NULL);

  game_hbox = gtk_hbox_new (TRUE, 0);
  g_assert (game_hbox);

  board_hbox = gtk_hbox_new (FALSE, 4);

  board_box = gtk_spin_button_new_with_range (1, 12, 1);
  g_assert (board_box);

  gtk_spin_button_set_value (GTK_SPIN_BUTTON (board_box), board);
  g_signal_connect(G_OBJECT(board_box), "value-changed", G_CALLBACK(settings_callback), NULL);
  gtk_box_pack_end (GTK_BOX (board_hbox), board_box, FALSE, FALSE, 0);

  board_label = gtk_label_new ("Starting level");
  g_assert(board_label);

  gtk_box_pack_end (GTK_BOX (board_hbox), board_label, FALSE, FALSE, 0);

  gtk_box_pack_start (GTK_BOX (game_hbox), board_hbox, FALSE, FALSE, 2);

  sound_hbox = gtk_hbox_new (FALSE, 4);

  sound_check = gtk_check_button_new();
  g_assert (sound_check);

  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(sound_check), enable_sound);
  g_signal_connect (G_OBJECT(sound_check), "clicked",
                    G_CALLBACK(sound_callback), NULL);

  //gtk_box_pack_end (GTK_BOX (sound_hbox), sound_check, FALSE, FALSE, 0);

  sound_label = gtk_label_new ("Sound");
  g_assert (sound_label);

  //gtk_box_pack_end (GTK_BOX (sound_hbox), sound_label, TRUE, TRUE, 0);

  gtk_box_pack_start (GTK_BOX (game_hbox), sound_hbox, FALSE, FALSE, 2);

  printf ("%s : %s : %d\n", __FILE__, __FUNCTION__, __LINE__);

  return game_hbox;
}

static void
unload_plugin (void)
{
  //free(gcc);
  //free(board_box);
}

static void
write_config (void)
{
  int value;

  value = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON(board_box));
  if (value < 1) value = 1;
  else if (value > 12) value = 12;
  gconf_client_set_int(gcc, SETTINGS_LEVEL, value, NULL);

  gconf_client_set_int(gcc, SETTINGS_SOUND,
        gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(sound_check)), NULL);
}

static GtkWidget **load_menu (guint *nitems)
{

    int enable_sound;
    //number of entries in osso-games-startup main menu for this game
    *nitems = 1;

    settings_item = gtk_menu_item_new_with_label ("Settings");
    settings_menu = gtk_menu_new ();    
    menu_items[0] = settings_item;
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (settings_item), settings_menu);

    //difficulty settings
    difficulty_menu = gtk_menu_new ();
    difficulty_item = gtk_menu_item_new_with_label ("Difficulty");
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (difficulty_item), difficulty_menu);
    gtk_menu_append (GTK_MENU (settings_menu), difficulty_item);

    level_1_item = gtk_radio_menu_item_new_with_label (group, "Level 1");
    gtk_menu_append (GTK_MENU (difficulty_menu), level_1_item);
    group = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(level_1_item));

    level_2_item = gtk_radio_menu_item_new_with_label (group, "Level 2");
    gtk_menu_append (GTK_MENU (difficulty_menu), level_2_item);
    group = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(level_2_item));

    level_3_item = gtk_radio_menu_item_new_with_label (group, "Level 3");
    gtk_menu_append (GTK_MENU (difficulty_menu), level_3_item);
    group = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(level_3_item));

    level_4_item = gtk_radio_menu_item_new_with_label (group, "Level 4");
    gtk_menu_append (GTK_MENU (difficulty_menu), level_4_item);
    group = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(level_4_item));

    level_5_item = gtk_radio_menu_item_new_with_label (group, "Level 5");
    gtk_menu_append (GTK_MENU (difficulty_menu), level_5_item);
    group = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(level_5_item));

    level_6_item = gtk_radio_menu_item_new_with_label (group, "Level 6");
    gtk_menu_append (GTK_MENU (difficulty_menu), level_6_item);
    group = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(level_6_item));

    level_7_item = gtk_radio_menu_item_new_with_label (group, "Level 7");
    gtk_menu_append (GTK_MENU (difficulty_menu), level_7_item);
    group = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(level_7_item));

    level_8_item = gtk_radio_menu_item_new_with_label (group, "Level 8");
    gtk_menu_append (GTK_MENU (difficulty_menu), level_8_item);
    group = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(level_8_item));

    level_9_item = gtk_radio_menu_item_new_with_label (group, "Level 9");
    gtk_menu_append (GTK_MENU (difficulty_menu), level_9_item);
    group = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(level_9_item));

    level_10_item = gtk_radio_menu_item_new_with_label (group, "Level 10");
    gtk_menu_append (GTK_MENU (difficulty_menu), level_10_item);
    group = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(level_10_item));

    level_11_item = gtk_radio_menu_item_new_with_label (group, "Level 11");
    gtk_menu_append (GTK_MENU (difficulty_menu), level_11_item);
    group = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(level_11_item));

    level_12_item = gtk_radio_menu_item_new_with_label (group, "Level 12");
    gtk_menu_append (GTK_MENU (difficulty_menu), level_12_item);
    group = gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM(level_12_item));

    g_signal_connect (G_OBJECT (level_1_item), "toggled",
                            G_CALLBACK (plugin_callback),
                            (gpointer) LEVEL_1);

    g_signal_connect (G_OBJECT (level_2_item), "toggled",
                            G_CALLBACK (plugin_callback),
                            (gpointer) LEVEL_2);
  
    g_signal_connect (G_OBJECT (level_3_item), "toggled",
                            G_CALLBACK (plugin_callback),
                            (gpointer) LEVEL_3);

    g_signal_connect (G_OBJECT (level_4_item), "toggled",
                            G_CALLBACK (plugin_callback),
                            (gpointer) LEVEL_4);

    g_signal_connect (G_OBJECT (level_5_item), "toggled",
                            G_CALLBACK (plugin_callback),
                            (gpointer) LEVEL_5);
  
    g_signal_connect (G_OBJECT (level_6_item), "toggled",
                            G_CALLBACK (plugin_callback),
                            (gpointer) LEVEL_6);

    g_signal_connect (G_OBJECT (level_7_item), "toggled",
                            G_CALLBACK (plugin_callback),
                            (gpointer) LEVEL_7);

    g_signal_connect (G_OBJECT (level_8_item), "toggled",
                            G_CALLBACK (plugin_callback),
                            (gpointer) LEVEL_8);
  
    g_signal_connect (G_OBJECT (level_9_item), "toggled",
                            G_CALLBACK (plugin_callback),
                            (gpointer) LEVEL_9);

    g_signal_connect (G_OBJECT (level_10_item), "toggled",
                            G_CALLBACK (plugin_callback),
                            (gpointer) LEVEL_10);

    g_signal_connect (G_OBJECT (level_11_item), "toggled",
                            G_CALLBACK (plugin_callback),
                            (gpointer) LEVEL_11);
  
    g_signal_connect (G_OBJECT (level_12_item), "toggled",
                            G_CALLBACK (plugin_callback),
                            (gpointer) LEVEL_12);

    gtk_menu_append (GTK_MENU (settings_menu), gtk_menu_item_new());
    
  //sound settings
  sound_item = gtk_check_menu_item_new_with_label("Sound");
  gtk_menu_append (GTK_MENU (settings_menu), sound_item);
  g_signal_connect (G_OBJECT (sound_item),
                    "toggled",
                    G_CALLBACK (plugin_callback),
                    (gpointer) MENU_SOUND);
  gtk_check_menu_item_set_state (GTK_CHECK_MENU_ITEM(sound_item),
		  gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (sound_check)));
    
    return menu_items;    
}

static void plugin_callback (GtkWidget *menu_item, gpointer  data)
{
    if (MENU_SOUND == (int) data && !changed){
        changed = TRUE;
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (sound_check), 
                                      gtk_check_menu_item_get_active (
                                      GTK_CHECK_MENU_ITEM(sound_item)));
    } else if (!changed) {
        changed = TRUE;
        gtk_spin_button_set_value (GTK_SPIN_BUTTON (board_box), (int) data);
    }
    changed = FALSE;
}

static void settings_callback (GtkWidget *widget, gpointer   data)
{
  if (!changed) {
    changed = TRUE;
    gint active = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
    if (active == LEVEL_1) {
        gtk_check_menu_item_set_state(GTK_CHECK_MENU_ITEM(level_1_item), TRUE);
    } else if (active == LEVEL_2) {
        gtk_check_menu_item_set_state(GTK_CHECK_MENU_ITEM(level_2_item), TRUE);
    } else if (active == LEVEL_3) {
        gtk_check_menu_item_set_state(GTK_CHECK_MENU_ITEM(level_3_item), TRUE);
    } else if (active == LEVEL_4) {
        gtk_check_menu_item_set_state(GTK_CHECK_MENU_ITEM(level_4_item), TRUE);
    } else if (active == LEVEL_5) {
        gtk_check_menu_item_set_state(GTK_CHECK_MENU_ITEM(level_5_item), TRUE);
    } else if (active == LEVEL_6) {
        gtk_check_menu_item_set_state(GTK_CHECK_MENU_ITEM(level_6_item), TRUE);
    } else if (active == LEVEL_7) {
        gtk_check_menu_item_set_state(GTK_CHECK_MENU_ITEM(level_7_item), TRUE);
    } else if (active == LEVEL_8) {
        gtk_check_menu_item_set_state(GTK_CHECK_MENU_ITEM(level_8_item), TRUE);
    } else if (active == LEVEL_9) {
        gtk_check_menu_item_set_state(GTK_CHECK_MENU_ITEM(level_9_item), TRUE);
    } else if (active == LEVEL_10) {
        gtk_check_menu_item_set_state(GTK_CHECK_MENU_ITEM(level_10_item), TRUE);
    } else if (active == LEVEL_11) {
        gtk_check_menu_item_set_state(GTK_CHECK_MENU_ITEM(level_11_item), TRUE);
    } else if (active == LEVEL_12) {
        gtk_check_menu_item_set_state(GTK_CHECK_MENU_ITEM(level_12_item), TRUE);
    }
    changed = FALSE;
  } 
}

static void 
sound_callback (GtkWidget *widget, 
                gpointer   data)
{
  if (!changed) {
    changed = TRUE;
    gtk_check_menu_item_set_state (GTK_CHECK_MENU_ITEM(sound_item), 
                                   gtk_toggle_button_get_active(
                                                   GTK_TOGGLE_BUTTON (widget)));
  }
  changed = FALSE;           
}

static void update_menu (void)
{
  settings_callback(board_box, NULL);
  sound_callback(sound_check, NULL);
}
