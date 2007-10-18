#include <stdio.h>
#include <glib.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>

int
main (int argc, char **argv)
{
  DBusConnection *connection;
  DBusError error;
  DBusMessage *message;
  DBusMessage *reply;
  int reply_timeout;
  char **service_list;
  int service_list_len;
  int i;
  
  dbus_error_init (&error);

  connection = dbus_bus_get (DBUS_BUS_SYSTEM,
                               &error);
  if (connection == NULL)
    {
      fprintf(stderr, "Failed to open connection to bus: %s\n",
                  error.message);
      dbus_error_free (&error);
      exit (1);
    }

  /* Construct the message */
  message = dbus_message_new_method_call ("org.frugalware.UpdNotifier",       /*service*/
                                           "/org/frugalware/UpdNotifier",     /*path*/
                                            "org.frugalware.UpdNotifier",      /*interface*/
                                            "PerformUpdate"); 

  /* Call ListServices method */
  reply_timeout = -1;   /*don't timeout*/
  reply = dbus_connection_send_with_reply_and_block (connection,
                                                                                           message, reply_timeout, 
                                                                                           &error);

  if (dbus_error_is_set (&error))
    {
      fprintf (stderr, "Error: %s\n",
                   error.message);
       exit (1);
    }

/* Extract the data from the reply */
 if (!dbus_message_get_args (reply, &error, 
                               DBUS_TYPE_STRING, &service_list,
                               DBUS_TYPE_INVALID))
   { 
      fprintf (stderr, "Failed to complete ListServices call: %s\n",
                  error.message);
      exit (1);
    }
  dbus_message_unref (reply);
  dbus_message_unref (message);

  /* Print the results */
 
  printf ("Services on the message bus:\n");
  i = 0;
  printf (service_list);
  
  return 0;
}
