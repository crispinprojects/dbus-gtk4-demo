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

static void activate (GtkApplication* app, gpointer user_data)
{
	GtkWidget *window;
	GtkWidget *box;
	GtkWidget *button_sync;
	GtkWidget *button_async;
		
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
