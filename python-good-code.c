/*
 *      python-good-code.c
 *
 *      Copyright 2013 Francesco OpenCode Apruzzese <opencode(at)e-ware(dot)org>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 3 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

#include <geanyplugin.h>

GeanyPlugin         *geany_plugin;
GeanyData           *geany_data;
GeanyFunctions      *geany_functions;

PLUGIN_VERSION_CHECK(211)

PLUGIN_SET_INFO("Python Good Code",
                "A plugin to pass your code to some tool as pep8, flake8 and others",
                "1.0",
                "Francesco OpenCode Apruzzese <opencode@e-ware.org>");

static GtkWidget *pgc_main_menu_item = NULL;
static gchar *software_path = NULL;

static void item_activate_cb(GtkMenuItem *menuitem, gpointer gdata)
{
    // Init values
    GeanyDocument *doc = NULL;
    GError *error = NULL;
    gchar *command = NULL;

    // Get actual document object
    doc = document_get_current();
    // If the file is a draft call save file dialog
    if (doc->real_path == NULL)
    {
        dialogs_show_save_as();
    }
    // Save the file to validate it
    document_save_file(doc, FALSE);
    if (software_path == NULL) {
        ui_set_statusbar(FALSE, "Could not execute control on the code. Please check your configuration.");
        return;
    }
    // Create a command and launch it!
    command = g_strconcat(software_path, " ", doc->file_name, NULL);
    g_spawn_command_line_async(command, &error);
    // Check error
    if (error != NULL)
    {
        ui_set_statusbar(FALSE, "Could not execute control on the code. Please check your configuration.");
        g_error_free(error);
    }
    // Good!
    ui_set_statusbar(FALSE, "Control on the code executed!");

}

void plugin_init(GeanyData *data)
{

    // Create menu object
    GtkWidget *pgc_menu_item;
    pgc_menu_item = gtk_menu_item_new_with_mnemonic("Python Good Code");
    gtk_widget_show(pgc_menu_item);
    gtk_container_add(GTK_CONTAINER(geany->main_widgets->tools_menu),
        pgc_menu_item);
    // Connect menu to signal
    g_signal_connect(pgc_menu_item, "activate",
        G_CALLBACK(item_activate_cb), NULL);
    /* make the menu item sensitive only when documents are open */
    ui_add_document_sensitive(pgc_menu_item);
    /* keep a pointer to the menu item, so we can remove it when the plugin is unloaded */
    pgc_main_menu_item = pgc_menu_item;
}

static void on_configure_response(GtkDialog *dialog, gint response, gpointer user_data)
{
        /* catch OK or Apply clicked */
        if (response == GTK_RESPONSE_OK || response == GTK_RESPONSE_APPLY)
        {
                /* We only have one pref here, but for more you would use a struct for user_data */
                GtkWidget *entry_software_path = GTK_WIDGET(user_data);
                g_free(software_path);
                software_path = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry_software_path)));
                /**** TODO : Save the current software path in a config file ****/
        }
}

GtkWidget *plugin_configure(GtkDialog *dialog)
{
    GtkWidget *lbl_software_path, *entry_software_path, *vbox;

    /* example configuration dialog */
    vbox = gtk_vbox_new(FALSE, 6);

    /* add a label and a text entry to the dialog */
    lbl_software_path = gtk_label_new(_("Control Software Command:"));
    gtk_misc_set_alignment(GTK_MISC(lbl_software_path), 0, 0.5);
    entry_software_path = gtk_entry_new();
    if (software_path != NULL)
            gtk_entry_set_text(GTK_ENTRY(entry_software_path), software_path);

    gtk_container_add(GTK_CONTAINER(vbox), lbl_software_path);
    gtk_container_add(GTK_CONTAINER(vbox), entry_software_path);

    gtk_widget_show_all(vbox);

    /* Connect a callback for when the user clicks a dialog button */
    g_signal_connect(dialog, "response", G_CALLBACK(on_configure_response), entry_software_path);
    return vbox;
}

void plugin_cleanup(void)
{
    gtk_widget_destroy(pgc_main_menu_item);
}
