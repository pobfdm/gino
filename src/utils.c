#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <mntent.h>


extern gchar* rulesDir;
extern gchar* confFile;
gchar* confDir;


gchar* getDeviceById(gchar* id)
{
	 gchar* device;
	 device=g_file_read_link (id,NULL);
	 device=g_filename_display_basename (device);
	 return device;
}

gint getUserId()
{
	uid_t id;
	id=getuid();
	return id;
}

gchar* getMountByDevice(gchar* device)
{
	gchar** mountPoint;
	struct mntent *ent;
	FILE *aFile;
	sleep(1);
	
	device=g_strdup_printf("/dev/%s", device);

	aFile = setmntent("/proc/mounts", "r");
	if (aFile == NULL) {
		perror("setmntent");
		exit(1);
	}
	while (NULL != (ent = getmntent(aFile))) 
	{
		if (strcmp(ent->mnt_fsname, device)==0)
		{
			mountPoint=g_strdup_printf("%s", ent->mnt_dir);
		}
	}
	
	endmntent(aFile);
    return mountPoint;	
}



char* GetKey(gchar* file, const gchar* group ,const gchar* key)
{
	//Una varibile per gli eventuali errori
	GError *error=NULL;
 
	//Mi preparo a leggere il file con le chiavi
	GKeyFile * mykey = g_key_file_new();
 
	g_key_file_load_from_file (mykey, file, G_KEY_FILE_KEEP_COMMENTS, &error);
	gchar* myval= g_key_file_get_value  (mykey, group, key, &error);
 
	g_key_file_free (mykey);
 
 
	//Gestisco eventuali errori di lettura
	if (error!=NULL)
	{
		 g_print("Gino config error : %s\n",error->message);
		return NULL;
	}
 
	return myval;
} 
 
 
void* SetKey(gchar* file, const gchar* group , const gchar* key, const gchar* content)
{
	//Una varibile per gli eventuali errori
	GError *error=NULL;
 
	//Mi preparo a leggere il file con le chiavi
	GKeyFile *mykey = g_key_file_new();
	g_key_file_load_from_file (mykey, file, G_KEY_FILE_KEEP_COMMENTS, &error);
 
	//Scrivo la chiave (immagine in memoria, non scritta sul file)
	g_key_file_set_string(mykey, group, key, content);
 
    //Adesso scrivo il file CONFFILE
    gsize size;
    gchar* data = g_key_file_to_data (mykey, &size, &error);
    g_file_set_contents (file, data, size,  &error);
 
	//Faccio pulizia delle strutture non piu' necessarie;
	g_free (data);
    g_key_file_free (mykey);
 
    //Gestisco eventuali errori di lettura
	if (error!=NULL)
	{
		 g_print("Gino config error : %s\n",error->message);
		return NULL;
	}
 
}  



void scanConnectedRules(gchar* device)
{
	const gchar* file;
	gchar* ruleFolderConnected=g_strdup_printf("%s/connected",rulesDir); 
	GDir *d = g_dir_open( ruleFolderConnected,0,NULL ); 
	gchar* command;
	GError *error = NULL;
	gchar* rulePath;
	
	while(( file = g_dir_read_name(d) ))
	{
		if (g_strrstr (file, g_path_get_basename (device) )!=NULL)
		{
			g_print("*****Rule detected.*****\n");
			rulePath=g_strdup_printf("%s/%s",ruleFolderConnected, file);
			
			//"command" directive
			command=GetKey(rulePath,"Rules" ,"command");
			//g_print("Command->%s\n", command);
			g_spawn_command_line_async (command, &error);
			if (error!=NULL) { g_print(error->message); g_error_free (error);}
			
			
			//add here another directive
		}
	}
	g_dir_close( d ); 

}


void scanDisconnectedRules(gchar* device)
{
	const gchar* file;
	gchar* ruleFolderConnected=g_strdup_printf("%s/disconnected",rulesDir); 
	GDir *d = g_dir_open( ruleFolderConnected,0,NULL ); 
	gchar* command;
	GError *error = NULL;
	gchar* rulePath;
	
	while(( file = g_dir_read_name(d) ))
	{
		if (g_strrstr (file, g_path_get_basename (device) )!=NULL)
		{
			g_print("*****Rule detected.*****\n");
			rulePath=g_strdup_printf("%s/%s",ruleFolderConnected, file);
			
			//"command" directive
			command=GetKey(rulePath,"Rules" ,"command");
			g_print("Command->%s\n", command);
			g_spawn_command_line_async (command, &error);
			if (error!=NULL) { g_print(error->message); g_error_free (error);}
			
			
			//add here another directive
		}
	}
	g_dir_close( d );

}

void makeConfDirs()
{
	gchar* rulesDirConnected;
	gchar* rulesDirDisconnected;
	gchar* defaultConfFileText="[Main] \n\
alwaysMounts=yes\n\
mountCommand=udisksctl mount -b %s \n\
openCommand=spacefm\n\
rulesDir=\n\
commandOnInputDevice=\n\
pidFile=\n\
";
	
	if (getUserId()==0)
	{
		//Root
		confFile=g_strdup_printf("%s", "/etc/gino.conf");
		rulesDir=g_strdup_printf("%s", "/usr/share/gino/rules/");
		rulesDirConnected=g_strdup_printf("%s/connected/",rulesDir);
		rulesDirDisconnected=g_strdup_printf("%s/disconnected/",rulesDir);
		if (!g_file_test ("/usr/share/gino", G_FILE_TEST_EXISTS)) g_mkdir("/usr/share/gino", 0755);
	}else{
		//users
		confDir=g_strdup_printf("%s/gino/", g_get_user_config_dir ());
		confFile=g_strdup_printf("%s/gino/gino.conf", g_get_user_config_dir ());
		rulesDir=g_strdup_printf("%s/gino/rules/",g_get_user_config_dir ());
		rulesDirConnected=g_strdup_printf("%s/connected/",rulesDir);
		rulesDirDisconnected=g_strdup_printf("%s/disconnected/",rulesDir);
	}
	
	//Create files and dir
	if (!g_file_test (confDir, G_FILE_TEST_EXISTS))
	{
		g_mkdir(confDir, 0755);
		g_file_set_contents (confFile,defaultConfFileText, -1,NULL);
		g_chmod (confFile,0755);
		SetKey(confFile,"Main" ,"rulesDir",rulesDir);
	}
	if (!g_file_test (rulesDir, G_FILE_TEST_EXISTS)) g_mkdir(rulesDir, 0755);
	if (!g_file_test (rulesDirConnected, G_FILE_TEST_EXISTS)) g_mkdir(rulesDirConnected, 0755);
	if (!g_file_test (rulesDirDisconnected, G_FILE_TEST_EXISTS)) g_mkdir(rulesDirDisconnected, 0755);
	
	
}
