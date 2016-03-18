#include <glib.h>
#include <gio/gio.h>
#include "utils.h" 

gchar* mountCommand;
gchar* alwaysMounts;
gchar* openCommand;
gchar* confFile;
gchar* rulesDir;
gchar* pidFile;
gchar* commandOnInputDevice;




void init()
{
	gchar* pid= g_strdup_printf("%d",getpid());
	
	
	alwaysMounts=GetKey(confFile,"Main" ,"alwaysMounts");
	mountCommand=GetKey(confFile,"Main" ,"mountCommand");
	openCommand=GetKey(confFile,"Main" ,"openCommand");
	commandOnInputDevice=GetKey(confFile,"Main" ,"commandOnInputDevice");
	pidFile=GetKey(confFile,"Main" ,"pidFile");
	if (pidFile!=NULL && g_strrstr (pidFile, "" )!=NULL) g_file_set_contents (pidFile,pid, -1,NULL);
	
	
}

 
void eventDiskCallback(GFileMonitor     *monitor,
               GFile            *file,
               GFile            *other_file,
               GFileMonitorEvent event_type,
               gpointer          user_data) 
{
	gchar* cmd;
	GError *error = NULL;
	gchar* deviceFile=g_file_get_path (file);
 
	
	if (event_type==G_FILE_MONITOR_EVENT_CREATED) g_print("Device %s is connected. \n", deviceFile);
	if (event_type==G_FILE_MONITOR_EVENT_DELETED) g_print("Device %s is disconnected. \n", deviceFile);
	if (event_type==G_FILE_MONITOR_EVENT_CHANGED) g_print("Device %s is changed. \n", deviceFile);
	
	//Try to mount partition
	if (event_type==G_FILE_MONITOR_EVENT_CREATED && \
		g_strrstr (alwaysMounts, "yes")!=NULL && \
		g_strrstr (deviceFile, "-part")!=NULL)
	{
		
		//Mount device
		cmd=g_strdup_printf(mountCommand, deviceFile);
		g_spawn_command_line_async (cmd, &error);
		if (error!=NULL) { g_print(error->message); g_error_free (error);}
		
		//Execute openCommand on mount point
		gchar* mountPoint=getMountByDevice(getDeviceById(deviceFile)); 
		g_print("Mounted on %s\n", mountPoint);
		
		cmd=g_strdup_printf("%s '%s'",openCommand, mountPoint);
		if(openCommand!=NULL)
		{
			g_spawn_command_line_async (cmd, &error);
			if (error!=NULL) { g_print(error->message); g_error_free (error);}
		}
	}
	
	//Try parsing rules when device is connected
	if (event_type==G_FILE_MONITOR_EVENT_CREATED && \
		g_strrstr (deviceFile, "-part")!=NULL)
	{
		scanConnectedRules(deviceFile);
	}
	
	//Try parsing rules when device is disconnected
	if (event_type==G_FILE_MONITOR_EVENT_DELETED && \
		g_strrstr (deviceFile, "-part")!=NULL)
	{
		scanDisconnectedRules(deviceFile);
	}
	
 
}

void eventInputCallback(GFileMonitor     *monitor,
               GFile            *file,
               GFile            *other_file,
               GFileMonitorEvent event_type,
               gpointer          user_data) 
{
	gchar* cmd;
	GError *error = NULL;
	gchar* deviceFile=g_file_get_path (file);
 
	
	if (event_type==G_FILE_MONITOR_EVENT_CREATED) g_print("Device %s is connected. \n", deviceFile);
	if (event_type==G_FILE_MONITOR_EVENT_DELETED) g_print("Device %s is disconnected. \n", deviceFile);
	if (event_type==G_FILE_MONITOR_EVENT_CHANGED) g_print("Device %s is changed. \n", deviceFile);
	
	
	
	//Try parsing rules when device is connected
	if (event_type==G_FILE_MONITOR_EVENT_CREATED )
	{
		//Command on input device
		cmd=g_strdup_printf(commandOnInputDevice, deviceFile);
		g_spawn_command_line_async (cmd, &error);
		if (error!=NULL) { g_print(error->message); g_error_free (error);}
		
		scanConnectedRules(deviceFile);
	}
	
	//Try parsing rules when device is disconnected
	if (event_type==G_FILE_MONITOR_EVENT_DELETED)
	{
		scanDisconnectedRules(deviceFile);
	}
	
 
}
 
 
 
int main(int argc, char** argv )
{
	GError *error = NULL;
	
	//Parsing commandline
	static GOptionEntry entries[] =
	{
	  { "conf", 'c', 0, G_OPTION_ARG_STRING, &confFile, "Custom config file", "<custom config file>" },
	  { NULL }
	};
	
	
	GOptionContext *context;

	context = g_option_context_new ("- Start gino with a  custom config file");
	g_option_context_add_main_entries (context, entries, NULL);
	//g_option_context_add_group (context, gtk_get_option_group (TRUE));
	if (!g_option_context_parse (context, &argc, &argv, &error))
	{
	  g_print ("Option parsing failed: %s\n", error->message);
	  exit (1);
	}
	if (confFile==NULL) makeConfDirs();
	

	//Gino Init
	init();
	
	//Main glib Loop
	GMainLoop *loop;
    loop = g_main_loop_new ( NULL , FALSE );
 
	//GError *error=NULL;
 
	
	
	//Disk monitor
	GFile *dirDisks= g_file_new_for_path ("/dev/disk/by-id/");
	GFileMonitor *diskMonitor=g_file_monitor_directory (dirDisks,
														G_FILE_MONITOR_NONE,
														NULL,
															&error);
	if (error!=NULL)
	{
		 g_error("I can't make a monitor  : %s\n",error->message);
		 g_error_free (error);
	}
	
	//Generic Input monitor
	GFile *dirInput= g_file_new_for_path ("/dev/input/by-id/");
	GFileMonitor *inputMonitor=g_file_monitor_directory (dirInput,
														G_FILE_MONITOR_NONE,
														NULL,
															&error);
	if (error!=NULL)
	{
		 g_error("I can't make a monitor  : %s\n",error->message);
		 g_error_free (error);
	}
															
 
	//Main events callbacks
	g_signal_connect (diskMonitor, "changed", G_CALLBACK(eventDiskCallback), NULL);
	g_signal_connect (inputMonitor, "changed", G_CALLBACK(eventInputCallback), NULL);
 
	//run main Glib Loop
	g_main_loop_run (loop);
    g_main_loop_unref(loop);
 
 
 
 
	return 0;
}
