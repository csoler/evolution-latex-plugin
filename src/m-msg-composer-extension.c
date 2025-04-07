/* latex-converter plugin for Gnome Evolution
 * Copyright (C) 2016 Red Hat, Inc. (www.redhat.com)
 * Copyright (C) 2025 Cyril Soler.
 *
 * Authors and maintainers:
 *           2016: Red Hat, Inc. (evolution plugin example code)
 *     April 2025: Cyril Soler <cyril.soler@inria.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>

#include <composer/e-msg-composer.h>

#include "m-msg-composer-extension.h"
#include "latex-converter.h"

#define LATEX_CONVERT_ACTION_NAME "convert-latex-equations-action"

// Plugin entry functions
//
gint e_plugin_lib_enable (EPlugin *ep, gint enable);
//void ee_editor_command_changed (GtkWidget *textbox);
//void ee_editor_immediate_launch_changed (GtkWidget *checkbox);

// Plugin implementation
//
gint e_plugin_lib_enable (EPlugin *ep, gint enable)
{
    return 0;
}

// void ee_editor_command_changed (GtkWidget *textbox)
// {
//     GSettings *settings;
//     const gchar *editor = gtk_entry_get_text (GTK_ENTRY (textbox));
//     printf ("\n\aeditor is : [%s] \n\a", editor);
//
//     /* GSettings access for every key-press. Sucky ? */
//     settings = e_util_ref_settings ("org.gnome.evolution.plugin.latex-equations");
//     g_settings_set_string (settings, "command", editor);
//     g_object_unref (settings);
// }
//
// void ee_editor_immediate_launch_changed (GtkWidget *checkbox)
// {
//     gboolean immediately;
//     GSettings *settings;
//     immediately = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (checkbox));
//
//     printf ("\n\aimmediate launch is : [%d] \n\a", immediately);
//
//     settings = e_util_ref_settings ("org.gnome.evolution.plugin.latex-equations");
//     g_settings_set_boolean (settings, "launch-on-key-press", immediately);
//     g_object_unref (settings);
// }
struct _MMsgComposerExtensionPrivate {
	gint dummy;
};

G_DEFINE_DYNAMIC_TYPE_EXTENDED (MMsgComposerExtension, m_msg_composer_extension, E_TYPE_EXTENSION, 0, G_ADD_PRIVATE_DYNAMIC (MMsgComposerExtension))

GtkWidget *e_plugin_lib_get_configure_widget (EPlugin *epl);

struct ExternalEditorData {
    EMsgComposer *composer;
    GDestroyNotify content_destroy_notify;
    guint cursor_position, cursor_offset;
};
static void show_error_dialog(GtkWindow *parent, const char *message)
{
    GtkWidget *dialog;

    dialog = gtk_message_dialog_new(
        parent,
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_ERROR,
        GTK_BUTTONS_CLOSE,
        "%s", message);

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}
void show_error_dialog_scrollable(GtkWindow *parent, const char *title, const char *message)
{
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        title,
        parent,
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "_Close", GTK_RESPONSE_CLOSE,
        NULL);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    // Create scrolled window
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scrolled_window, 400, 200); // set scroll area size

    // Create text view
    GtkWidget *text_view = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(text_view), FALSE);

    // Set the message text
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_text_buffer_set_text(buffer, message, -1);

    // Add text view to scrolled window
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);
    gtk_box_pack_start(GTK_BOX(content_area), scrolled_window, TRUE, TRUE, 10);

    gtk_widget_show_all(dialog);

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

static void launch_editor_content_ready_cb (GObject *source_object, GAsyncResult *result, gpointer user_data)
{
    struct ExternalEditorData *eed = user_data;
    EContentEditor *cnt_editor;
    EContentEditorContentHash *content_hash;
    GError *error = NULL;

    g_return_if_fail (E_IS_CONTENT_EDITOR (source_object));
    g_return_if_fail (eed != NULL);

    cnt_editor = E_CONTENT_EDITOR (source_object);

    content_hash = e_content_editor_get_content_finish (cnt_editor, result, &error);

    if (!content_hash)
    {
        g_warning ("%s: Faild to get content: %s", G_STRFUNC, error ? error->message : "Unknown error");
        return;
    }

    gchar *content = content_hash ? e_content_editor_util_steal_content_data (content_hash, E_CONTENT_EDITOR_GET_TO_SEND_PLAIN, &(eed->content_destroy_notify)) : NULL;

    g_print ("Got the following message content:\n%s\n", (char*)content);

    // now convert the message

    gchar *converted_text = NULL;
    int error_code=0;
    gchar *error_details = NULL;

    if(!convertText(content,&converted_text,&error_code,&error_details))
    {
        int len = strlen(error_details)+1000;
        char *error_message = (char*)malloc(len+1);
        snprintf(error_message,len,"Error %d while converting. Details:\n%s\n",error_code,error_details);

        free(error_details);

        show_error_dialog_scrollable(&eed->composer->parent,"LaTeX conversion error",error_message);
        free(error_message);
        return;
    }

    e_msg_composer_set_body_text(eed->composer, converted_text,true);

    free(eed);
    free(converted_text);

}

static bool executableExists(const char *command)
{
    struct stat st;
    return stat(command, &st) == 0 && (st.st_mode & S_IXUSR);
}
static bool checkExists(EMsgComposer *composer,const char *path,const char *package_name)
{
    if(executableExists(path))
        return true;

    int len = strlen(path) + strlen(package_name) + 100;

    char tmp[len];
    sprintf(tmp,"Executable %s not found!\nTry to install package named %s.",path,package_name);

    show_error_dialog(GTK_WINDOW(&composer->parent), tmp);

    return false;
}
static void action_msg_composer_cb (GtkAction *action, MMsgComposerExtension *msg_composer_ext)
{
    EMsgComposer *composer;
    g_return_if_fail (M_IS_MSG_COMPOSER_EXTENSION (msg_composer_ext));
    composer = E_MSG_COMPOSER (e_extension_get_extensible (E_EXTENSION (msg_composer_ext)));

    // perform various checks

    if(!checkExists(composer,"/usr/bin/latex","texlive-latex-base")) return;
    if(!checkExists(composer,"/usr/bin/dvips","texlive-binaries"))   return;
    if(!checkExists(composer,"/usr/bin/ps2pdf","ghostscript"))       return;
    if(!checkExists(composer,"/usr/bin/pdftocairo","poppler-utils")) return;
    if(!checkExists(composer,"/usr/bin/base64","coreutils"))         return;

    //
    EHTMLEditor *editor = e_msg_composer_get_editor(composer);
    EContentEditor *cnt_editor = e_html_editor_get_content_editor(editor);
    EContentEditorMode mode = e_html_editor_get_mode(editor);

    if(mode != E_CONTENT_EDITOR_MODE_HTML && mode != E_CONTENT_EDITOR_MODE_MARKDOWN_HTML)
    {
        show_error_dialog(&composer->parent,"Cannot convert equations in plain text mode.\nPlease use HTML editor instead.");
        return;
    }

    struct ExternalEditorData *eed = (struct ExternalEditorData*)malloc(sizeof(struct ExternalEditorData));
    eed->composer = composer;

    // Start async request of editor data

    e_content_editor_get_content (cnt_editor, E_CONTENT_EDITOR_GET_TO_SEND_PLAIN, NULL, NULL, launch_editor_content_ready_cb, eed);

    g_print ("New 2 action: %s: for composer '%s'\n", G_STRFUNC, gtk_window_get_title (GTK_WINDOW (composer)));
}
#ifdef TODO
static void update_actions_cb(EHTMLEditor *editor, GtkActionEntry *entries)
{
    GtkUIManager *ui_manager = e_html_editor_get_ui_manager (editor);
    EContentEditorMode mode = e_html_editor_get_mode(editor);


    GtkAction *convert_action =	e_lookup_action(ui_manager,LATEX_CONVERT_ACTION_NAME);

        fprintf(stderr,"Got action %p!",(void*)convert_action);

    if(!convert_action)
    {
        fprintf(stderr,"Cannot find action!");
        return;
    }

    if(mode == E_CONTENT_EDITOR_MODE_HTML || mode == E_CONTENT_EDITOR_MODE_MARKDOWN_HTML)
        gtk_action_set_sensitive(convert_action, TRUE);
    else
        gtk_action_set_sensitive(convert_action, FALSE);
}
#endif

static GtkActionEntry msg_composer_entries[] = {
    { LATEX_CONVERT_ACTION_NAME,
      "ooo-math",
	  N_("M_y Message Composer Action..."),
      "<Shift><Control>l",
      N_("Convert LaTeX equations"),
	  G_CALLBACK (action_msg_composer_cb) }
};

static void m_msg_composer_extension_add_ui (MMsgComposerExtension *msg_composer_ext, EMsgComposer *composer)
{
	const gchar *ui_def =
		"<menubar name='main-menu'>\n"
		"  <placeholder name='pre-edit-menu'>\n"
		"    <menu action='file-menu'>\n"
		"      <placeholder name='external-editor-holder'>\n"
        "        <menuitem action='convert-latex-equations-action'/>\n"
		"      </placeholder>\n"
		"    </menu>\n"
		"  </placeholder>\n"
		"</menubar>\n"
		"\n"
		"<toolbar name='main-toolbar'>\n"
        "  <toolitem action='convert-latex-equations-action'/>\n"
		"</toolbar>\n";

    gtk_icon_theme_add_resource_path(gtk_icon_theme_get_default(),"/usr/share/icons/breeze/actions/22/");

	EHTMLEditor *html_editor;
	GtkActionGroup *action_group;
	GtkUIManager *ui_manager;
	GError *error = NULL;

	g_return_if_fail (M_IS_MSG_COMPOSER_EXTENSION (msg_composer_ext));
	g_return_if_fail (E_IS_MSG_COMPOSER (composer));

	html_editor = e_msg_composer_get_editor (composer);
	ui_manager = e_html_editor_get_ui_manager (html_editor);
	action_group = e_html_editor_get_action_group (html_editor, "core");

	/* Add actions to the action group. */
	e_action_group_add_actions_localized (
		action_group, GETTEXT_PACKAGE,
		msg_composer_entries, G_N_ELEMENTS (msg_composer_entries), msg_composer_ext);

	gtk_ui_manager_add_ui_from_string (ui_manager, ui_def, -1, &error);

	if (error) {
		g_warning ("%s: Failed to add ui definition: %s", G_STRFUNC, error->message);
		g_error_free (error);
	}
#ifdef TODO
    /* Decide whether we want this option to be visible or not */
    /* this actually doesn't work since the callback is never called. I don't know why. */
    g_signal_connect ( html_editor, "update-actions", G_CALLBACK (update_actions_cb), html_editor);
#endif

    gtk_ui_manager_ensure_update (ui_manager);
}

static void m_msg_composer_extension_constructed (GObject *object)
{
	EExtension *extension;
	EExtensible *extensible;

	/* Chain up to parent's method. */
	G_OBJECT_CLASS (m_msg_composer_extension_parent_class)->constructed (object);

	extension = E_EXTENSION (object);
	extensible = e_extension_get_extensible (extension);

	m_msg_composer_extension_add_ui (M_MSG_COMPOSER_EXTENSION (object), E_MSG_COMPOSER (extensible));
}

static void m_msg_composer_extension_class_init (MMsgComposerExtensionClass *class)
{
	GObjectClass *object_class;
	EExtensionClass *extension_class;

	object_class = G_OBJECT_CLASS (class);
	object_class->constructed = m_msg_composer_extension_constructed;

	/* Set the type to extend, it's supposed to implement the EExtensible interface */
	extension_class = E_EXTENSION_CLASS (class);
	extension_class->extensible_type = E_TYPE_MSG_COMPOSER;
}

static void m_msg_composer_extension_class_finalize (MMsgComposerExtensionClass *class)
{
}

static void m_msg_composer_extension_init (MMsgComposerExtension *msg_composer_ext)
{
	msg_composer_ext->priv = m_msg_composer_extension_get_instance_private (msg_composer_ext);
}

void m_msg_composer_extension_type_register (GTypeModule *type_module)
{
	m_msg_composer_extension_register_type (type_module);
}

void add_command_widget_for(GtkWidget *vbox,const char *command,const char *package)
{
    GtkWidget *hbox_cmd = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *entry_cmd = gtk_entry_new();

    gtk_box_pack_start(GTK_BOX(hbox_cmd), entry_cmd, TRUE, TRUE, 0);

    GtkWidget *icon_image ;

    if(executableExists(command))
    {
        icon_image = gtk_image_new_from_icon_name("starred", GTK_ICON_SIZE_BUTTON);
        gtk_entry_set_text(GTK_ENTRY(entry_cmd), command);
    }
    else
    {
        char text[1001];
        snprintf(text,1000,"Missing package %s",package);
        gtk_entry_set_text(GTK_ENTRY(entry_cmd), text);
        icon_image = gtk_image_new_from_icon_name("dialog-error", GTK_ICON_SIZE_BUTTON);
    }

    gtk_editable_set_editable(GTK_EDITABLE(entry_cmd), FALSE);

    gtk_box_pack_start(GTK_BOX(hbox_cmd), icon_image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox_cmd, FALSE, FALSE, 0);
}

GtkWidget *e_plugin_lib_get_configure_widget (EPlugin *epl)
{
    GtkWidget *vbox;
//    GSettings *settings;
//    gchar *editor;
//    gboolean checked;

    fprintf(stderr,"***** e_plugin_lib_get_configure_widget called!\n");

#ifdef SUSPENDED
    vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
    textbox = gtk_entry_new ();
    label = gtk_label_new (_("Command to be executed to launch the editor: "));
    help = gtk_label_new (_("For XEmacs use “xemacs”\nFor Vim use “gvim -f”"));
//    settings = e_util_ref_settings ("org.gnome.evolution.plugin.latex-equations");

//    editor = g_settings_get_string (settings, "command");

    if (editor) {
        gtk_entry_set_text (GTK_ENTRY (textbox), editor);
        g_free (editor);
    }

    checkbox = gtk_check_button_new_with_mnemonic ( _("_Automatically launch when a new mail is edited"));
//    checked = g_settings_get_boolean (settings, "launch-on-key-press");
//    if (checked)
//        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbox), TRUE);
//    g_object_unref (settings);

    gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (vbox), textbox, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (vbox), help, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (vbox), checkbox, FALSE, FALSE, 0);
#endif

    vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);

    add_command_widget_for(vbox,"/usr/bin/latex","texlive-latex-base");
    add_command_widget_for(vbox,"/usr/bin/dvips","texlive-binaries");
    add_command_widget_for(vbox,"/usr/bin/ps2pdf","ghostscript");
    add_command_widget_for(vbox,"/usr/bin/pdftocairo","poppler-utils");
    add_command_widget_for(vbox,"/usr/bin/base64","coreutils");

//        g_signal_connect(entry_cmd, "changed", G_CALLBACK(on_command_changed), NULL);

        // --- Row 2: Resolution + Entry ---
        GtkWidget *hbox_res = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
        GtkWidget *entry_res = gtk_entry_new();
        gtk_entry_set_text(GTK_ENTRY(entry_res), "16000");
        gtk_entry_set_input_purpose(GTK_ENTRY(entry_res), GTK_INPUT_PURPOSE_NUMBER); // mobile hint
        gtk_entry_set_max_length(GTK_ENTRY(entry_res), 5);
        gtk_editable_set_editable(GTK_EDITABLE(entry_res), FALSE);

        gtk_box_pack_start(GTK_BOX(hbox_res), entry_res, FALSE, FALSE, 0);

        GtkWidget *label_res = gtk_label_new("resolution");
        gtk_box_pack_start(GTK_BOX(hbox_res), label_res, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(vbox), hbox_res, FALSE, FALSE, 0);
#ifdef TODO
    g_signal_connect (
        checkbox, "toggled",
        G_CALLBACK (ee_editor_immediate_launch_changed), checkbox);
#endif
    gtk_widget_show_all (vbox);

    return vbox;
}
