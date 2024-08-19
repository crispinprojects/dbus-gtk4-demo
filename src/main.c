/***************************************************************************
 *   Author Alan Crispin                                                   *
 *   crispinalan@gmail.com                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation.                                         *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

//====================================================================
// GTK4 D-Bus Demo
// Author: Alan Crispin <crispinalan@gmail.com> 
// Date: August 2024
// use make file to compile
//====================================================================


#include <gtk/gtk.h>


static void callbk_async(GObject *source_object,  GAsyncResult *res,  gpointer  user_data);
static void callbk_btn_async(GtkButton *button, gpointer  user_data);
static void callbk_btn_sync(GtkButton *button, gpointer  user_data);
static void callbk_notification(GtkButton *button, gpointer  user_data);


//======================================================================
static void callbk_async(GObject *source_object,  GAsyncResult *res,  gpointer  user_data)
{  
  g_print("async callback function invoked\n");
  
  GDBusMessage *reply_message;
  GError *error = NULL;
  
  GDBusConnection *conn = G_DBUS_CONNECTION (source_object);
	
  reply_message = g_dbus_connection_send_message_with_reply_finish (conn, res, &error);
	
	
 if(reply_message == NULL)
    {
        g_print("g_dbus_connection_send_message_with_reply_sync failed\n");     
    }

    if(g_dbus_message_get_message_type(reply_message) == G_DBUS_MESSAGE_TYPE_ERROR)
    {
        g_print("Error occured\n");

        g_dbus_message_to_gerror(reply_message, &error);
        g_printerr("Error invoking g_dbus_connection_send_message_with_reply_sync: %s\n", error->message);

        g_error_free(error);
        
    }


    GVariant* variant = g_dbus_message_get_body(reply_message);

    // get first child
    GVariant* var_child = g_variant_get_child_value(variant, 0);

    const gchar* var_str = g_variant_get_string(var_child, NULL);
    
    g_print("var_str = %s\n",var_str);
   
    //cleanup
    g_variant_unref(var_child);    
    g_object_unref(reply_message);	
	
}

//======================================================================

static void callbk_btn_async(GtkButton *button, gpointer  user_data)
{	
	g_print("Asynchronous D-Bus connection\n");
	//D-Bus connection
	GDBusConnection *conn= g_application_get_dbus_connection (g_application_get_default());
	//Get the unique name of connection as assigned by the message bus
	const gchar* dbus_name =g_dbus_connection_get_unique_name(conn);
	g_print("dbus_name = %s\n", dbus_name);
	
    GDBusMessage* call_message = g_dbus_message_new_method_call (
	dbus_name,
	"/org/gtk/example",     
	"org.freedesktop.DBus.Introspectable",
	"Introspect");
                                                   
                                                       
    if(call_message == NULL)
    {
        g_print("g_dbus_message_new_method_call failed\n");
    }
        
   g_dbus_connection_send_message_with_reply (
	conn,
	call_message,
	G_DBUS_SEND_MESSAGE_FLAGS_NONE,
	-1,
	NULL,
	NULL,
	callbk_async,
	NULL
	);
   
   // cleanup
    g_object_unref(call_message);		
	
}
//=====================================================================

static void callbk_btn_sync(GtkButton *button, gpointer  user_data)
{	
	g_print("Synchronous D-Bus connection\n");
	//D-Bus connection
	GDBusConnection *conn= g_application_get_dbus_connection (g_application_get_default());
	//Get the unique name of connection as assigned by the message bus
	const gchar* dbus_name =g_dbus_connection_get_unique_name(conn);
	g_print("dbus_name = %s\n", dbus_name);
	
	
	GError* error = NULL;
    
    GDBusMessage* call_message = g_dbus_message_new_method_call (
	dbus_name,
	"/org/gtk/example",     
	"org.freedesktop.DBus.Introspectable",
	"Introspect");
                                                       
    if(call_message == NULL)
    {
        g_print("g_dbus_message_new_method_call failed\n");
    }

    GDBusMessage* reply_message = g_dbus_connection_send_message_with_reply_sync(conn,
                                                                          call_message,
                                                                          G_DBUS_SEND_MESSAGE_FLAGS_NONE,
                                                                          -1,
                                                                          NULL,
                                                                          NULL,
                                                                          &error);
    if(reply_message == NULL)
    {
        g_print("g_dbus_connection_send_message_with_reply_sync failed\n");     
    }

    if(g_dbus_message_get_message_type(reply_message) == G_DBUS_MESSAGE_TYPE_ERROR)
    {
        g_print("Error occured\n");

        g_dbus_message_to_gerror(reply_message, &error);
        g_printerr("Error invoking g_dbus_connection_send_message_with_reply_sync: %s\n", error->message);

        g_error_free(error);
        
    }


    GVariant* variant = g_dbus_message_get_body(reply_message);

    // get first child
    GVariant* var_child = g_variant_get_child_value(variant, 0);

    const gchar* var_str = g_variant_get_string(var_child, NULL);
    
    g_print("var_str = %s\n",var_str);
  
    //cleanup
    g_variant_unref(var_child);
    g_object_unref(call_message);
    g_object_unref(reply_message);	
}
//=====================================================================
static void notification_sent (GObject *source_object,  GAsyncResult *result, gpointer user_data)
{
  g_print("notification sent called\n");
  GVariant *val;
  GError *error = NULL;
 
  val = g_dbus_connection_call_finish (G_DBUS_CONNECTION (source_object), result, &error);
  if (val)
    {
      g_print("Notification sent: Connection call finished \n");
      g_variant_unref (val);
    }
  else
    {
      g_print("Unable to send notifications using org.freedesktop.Notifications\n"); 
      //process error here 
      //g_warning ("Unable to send notifications using org.freedesktop.Notifications: %s",
                     //error->message);
      g_error_free (error);    

    }

}

//=====================================================================
static void callbk_notification(GtkButton *button, gpointer  user_data)
{
	g_print(" Call org.freedesktop.Notifications.Notify\n");
	
	const char* title;
	const char* body;
	title= "D-Bus Notification";
	body = "Hello World Message";
	
	//ACTIONS
	g_auto(GVariantBuilder) actions_builder = G_VARIANT_BUILDER_INIT (G_VARIANT_TYPE_ARRAY);	
	g_variant_builder_add(&actions_builder, "s", "");
	g_variant_builder_add(&actions_builder, "s", "Quit");
	//GVariant *notify_actions = g_variant_builder_end(&actions_builder); //dont do this
	
	//HINTS		
	g_auto(GVariantBuilder) hints_builder = G_VARIANT_BUILDER_INIT (G_VARIANT_TYPE_DICTIONARY);	
	g_variant_builder_add(&hints_builder, "{sv}", "urgency", g_variant_new_int32(1));	
	g_autoptr(GVariant) notify_hints = g_variant_builder_end(&hints_builder);
	
	g_autoptr(GVariant) parameters = g_variant_new("(susssas@a{sv}i)",
	"app_name",
	-1,
	"",
	title,
	body,
	&actions_builder,	
	notify_hints,
	-1);
	
	g_autoptr(GDBusConnection) conn= g_application_get_dbus_connection (g_application_get_default());
	
	g_dbus_connection_call (conn,
	"org.freedesktop.Notifications",
	"/org/freedesktop/Notifications",
	"org.freedesktop.Notifications",
	"Notify",
	parameters,
	G_VARIANT_TYPE ("(u)"),
	G_DBUS_CALL_FLAGS_NONE,
	-1, 
	NULL,
	notification_sent, 
	NULL);	
}

//=====================================================================

static void activate (GtkApplication* app, gpointer user_data)
{
	GtkWidget *window;
	GtkWidget *box;
	GtkWidget *button_sync;
	GtkWidget *button_async;
	GtkWidget *button_notification;
		
	window = gtk_application_window_new (app);
	gtk_window_set_title (GTK_WINDOW (window), "D-Bus GTK4 Demo");
	gtk_window_set_default_size (GTK_WINDOW (window),500, 200);
	box =gtk_box_new(GTK_ORIENTATION_VERTICAL,1);  
    gtk_window_set_child (GTK_WINDOW (window), box);	
	
	button_sync = gtk_button_new_with_label ("Synchronous D-Bus Connection");
	g_signal_connect (GTK_BUTTON (button_sync),"clicked", G_CALLBACK (callbk_btn_sync), G_OBJECT (window));
	gtk_box_append(GTK_BOX(box), button_sync);
	
	button_async = gtk_button_new_with_label ("Asynchronous D-Bus Connection");
	g_signal_connect (GTK_BUTTON (button_async),"clicked", G_CALLBACK (callbk_btn_async), G_OBJECT (window));
	gtk_box_append(GTK_BOX(box), button_async);
	
	button_notification = gtk_button_new_with_label ("D-Bus Notification");
	g_signal_connect (GTK_BUTTON (button_notification),"clicked", G_CALLBACK (callbk_notification), G_OBJECT (window));
	gtk_box_append(GTK_BOX(box), button_notification);
	
	gtk_window_present(GTK_WINDOW (window));
	     
}
//======================================================================
int main (int argc, char **argv)
{
  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
