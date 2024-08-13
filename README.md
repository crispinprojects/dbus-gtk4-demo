# D-Bus Getting Started (GTK4, C)

This small sample project demonstrates how to get started with D-Bus using GTK4 and C. 

D-Bus is a message passing system that allows different processes which are running concurrently on the same computer to communicate with each other. This is known as interprocess communication (IPC).  D-Bus is an integral part of the Linux operating system. 

## D-Bus Connection

The function [g_application_get_dbus_connection](https://docs.gtk.org/gio/method.Application.get_dbus_connection.html) gets the GDBusConnection being used by an application which has a unique connection name assigned by the message bus. The code below shows how to make a D-Bus connection and get the unique name.

```
//Establish a GDBusConnection used by the application
GDBusConnection *conn= g_application_get_dbus_connection (g_application_get_default());
//Get the unique name of connection as assigned by the message bus
const gchar* dbus_name =g_dbus_connection_get_unique_name(conn);
g_print("dbus_name = %s\n", dbus_name);
```

The unique connection name starts with the colon character ":".

## Messages

With D-Bus, messages are transferred between processes. A message has a header (which identifies it) and a message body containing data. A D-Bus service is an application that provides one or more interfaces for other applications to use. An interface is a collection of methods, signals, and properties that a service exposes. All D-Bus services should provide an interface called "Introspectable" and a method called "Introspect" to get information about the D-Bus service as an XML string. The function [g_dbus_message_new_method_call](https://docs.gtk.org/gio/ctor.DBusMessage.new_method_call.html) creates a new GDBusMessage for a method call. The sample project uses the call_message shown below. Notice that it uses the unique application D-Bus connection name obtained above, the aplication ID path, the "Introspectable" D-Bus interface name and the "Introspect" method name.

```
GDBusMessage* call_message = g_dbus_message_new_method_call (
    dbus_name,
    "/org/gtk/example",     
    "org.freedesktop.DBus.Introspectable",
    "Introspect");
```

## Synchronously Sending a Message

The function [g_dbus_connection_send_message_with_reply_sync](https://docs.gtk.org/gio/method.DBusConnection.send_message_with_reply_sync.html) synchronously sends the call message (defined above) to the connection and blocks the calling thread until a reply is received or the timeout is reached. The g_dbus_connection_send_message_with_reply() is the asynchronous version of this method and is dicussed in the next section. The code below demonstrates using the application connection and the call_message to obtain a reply_message. The [g_dbus_message_get_body](https://docs.gtk.org/gio/method.DBusMessage.get_body.html) gets the body of a message as a GVariant data type. The function g_variant_get_child_value is used to read the first child item out of the GVariant container instance and the function g_variant_get_string is used to convert this to a string for printing purposes. Finally the message objects and variant object are unreferenced.

```
GError* error = NULL;   

GDBusMessage* reply_message = g_dbus_connection_send_message_with_reply_sync
(conn,
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

    // get first child and print
    GVariant* var_child = g_variant_get_child_value(variant, 0);
    const gchar* var_str = g_variant_get_string(var_child, NULL);    
    g_print("var_str = %s\n",var_str);

    // cleanup
    g_variant_unref(var_child);
    g_object_unref(call_message);
    g_object_unref(reply_message);
```

## Asynchronously Sending a Message

The function [g_dbus_connection_send_message_with_reply](https://docs.gtk.org/gio/method.DBusConnection.send_message_with_reply.html) asynchronously sends message to a connection. With asynchronous programming, operations can be started, run in the background and then a call back invoked when they are finished. This means that a callback function has to be defined and has the form shown below.

```
static void callbk_async(GObject *source_object,  GAsyncResult *res,  gpointer  user_data)
{  
  g_print("async function called\n"); 
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
```

The GDBusMessage reply_message is now contained inside the asynchronous callback function which gets the body of a message as a GVariant data type and prints this out. The code details are demonstrated in the sample project.

My reading around GTK4 suggests that asynchronous operations are preferred over synchronous operations.

## Compile DBus Sample Project

Download the source code for the D-Bus demo and use the MAKEFILE to compile and run it as shown below

```
make
./demo
```

![](dbus-demo.png)

Running the application and pressing either the synchronous or asynchronous D-Bus buttons  generates XML output on the console which typically looks like that below.

```
dbus_name = :1.114
async callback function invoked
var_str = <!DOCTYPE node PUBLIC "-//freedesktop//DTD D-BUS Object Introspection 1.0//EN"
                      "http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd">
<!-- GDBus 2.80.3 -->
<node>
  <interface name="org.freedesktop.DBus.Properties">
    <method name="Get">
      <arg type="s" name="interface_name" direction="in"/>
      <arg type="s" name="property_name" direction="in"/>
      <arg type="v" name="value" direction="out"/>
    </method>
    <method name="GetAll">
      <arg type="s" name="interface_name" direction="in"/>
      <arg type="a{sv}" name="properties" direction="out"/>
    </method>
    <method name="Set">
      <arg type="s" name="interface_name" direction="in"/>
      <arg type="s" name="property_name" direction="in"/>
      <arg type="v" name="value" direction="in"/>
    </method>
    <signal name="PropertiesChanged">
      <arg type="s" name="interface_name"/>
      <arg type="a{sv}" name="changed_properties"/>
      <arg type="as" name="invalidated_properties"/>
    </signal>
  </interface>
  <interface name="org.freedesktop.DBus.Introspectable">
    <method name="Introspect">
      <arg type="s" name="xml_data" direction="out"/>
    </method>
  </interface>
  <interface name="org.freedesktop.DBus.Peer">
    <method name="Ping"/>
    <method name="GetMachineId">
      <arg type="s" name="machine_uuid" direction="out"/>
    </method>
  </interface>
  <interface name="org.gtk.Actions">
    <method name="List">
      <arg type="as" name="list" direction="out">
      </arg>
    </method>
    <method name="Describe">
      <arg type="s" name="action_name" direction="in">
      </arg>
      <arg type="(bgav)" name="description" direction="out">
      </arg>
    </method>
    <method name="DescribeAll">
      <arg type="a{s(bgav)}" name="descriptions" direction="out">
      </arg>
    </method>
    <method name="Activate">
      <arg type="s" name="action_name" direction="in">
      </arg>
      <arg type="av" name="parameter" direction="in">
      </arg>
      <arg type="a{sv}" name="platform_data" direction="in">
      </arg>
    </method>
    <method name="SetState">
      <arg type="s" name="action_name" direction="in">
      </arg>
      <arg type="v" name="value" direction="in">
      </arg>
      <arg type="a{sv}" name="platform_data" direction="in">
      </arg>
    </method>
    <signal name="Changed">
      <arg type="as" name="removals">
      </arg>
      <arg type="a{sb}" name="enable_changes">
      </arg>
      <arg type="a{sv}" name="state_changes">
      </arg>
      <arg type="a{s(bgav)}" name="additions">
      </arg>
    </signal>
  </interface>
  <interface name="org.gtk.Application">
    <method name="Activate">
      <arg type="a{sv}" name="platform-data" direction="in">
      </arg>
    </method>
    <method name="Open">
      <arg type="as" name="uris" direction="in">
      </arg>
      <arg type="s" name="hint" direction="in">
      </arg>
      <arg type="a{sv}" name="platform-data" direction="in">
      </arg>
    </method>
    <method name="CommandLine">
      <arg type="o" name="path" direction="in">
      </arg>
      <arg type="aay" name="arguments" direction="in">
      </arg>
      <arg type="a{sv}" name="platform-data" direction="in">
      </arg>
      <arg type="i" name="exit-status" direction="out">
      </arg>
    </method>
    <property type="b" name="Busy" access="read">
    </property>
  </interface>
  <interface name="org.freedesktop.Application">
    <method name="Activate">
      <arg type="a{sv}" name="platform-data" direction="in">
      </arg>
    </method>
    <method name="Open">
      <arg type="as" name="uris" direction="in">
      </arg>
      <arg type="a{sv}" name="platform-data" direction="in">
      </arg>
    </method>
    <method name="ActivateAction">
      <arg type="s" name="action-name" direction="in">
      </arg>
      <arg type="av" name="parameter" direction="in">
      </arg>
      <arg type="a{sv}" name="platform-data" direction="in">
      </arg>
    </method>
  </interface>
  <node name="window"/>
</node>
```

## D-Feet

Now install D-Feet which is the GNOME D-Bus browser, viewer and degugger. 

With Fedora you install it using the terminal command below.

```
dnf search d-feet
sudo dnf install d-feet
```

D-feet allows the D-Bus peers represented by the unique connection names on both the system and session bus to be inspected. Now search for the D-Bus demo application which will be located in the "Session Bus" section. It will be something like :1.(number) with ./demo and an object path name of org.gtk.example which in this case is the application ID. See screenshot below.

![](d-feet2.png)

## Summary

This demo application shows how to get the GDBusConnection being used by an application and its unique name and how to synchronously and asynchronously send a message and get a reply. D-Bus services self-register with the bus, making themselves discoverable to other applications.

## License

GTK is released under the terms of the [GNU Lesser General Public License version 2.1](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html). Consequently, this GTK4 D-Bus sample code application is licensed under the same LGPL v2.1 license.

## Acknowledgements

* [GTK](https://www.gtk.org/)

* GTK is a free and open-source project maintained by GNOME and an active community of contributors. GTK is released under the terms of the [GNU Lesser General Public License version 2.1](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html).

* [GTK4 API](https://docs.gtk.org/gtk4/index.html)

* [GObject API](https://docs.gtk.org/gobject/index.html)

* [Glib API](https://docs.gtk.org/glib/index.html)

* [Gio API](https://docs.gtk.org/gio/index.html)

* [Geany](https://www.geany.org/) is a lightweight source-code editor (version 2 now uses GTK3). [GPL v2 license](https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt)

* [Fedora](https://fedoraproject.org/)

* [Ubuntu](https://ubuntu.com/download/desktop)

* [Debian](https://www.debian.org/)
