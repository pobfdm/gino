gchar* getDeviceById(gchar* id);
void scanRules(gchar* device);
char* GetKey(gchar* file, const gchar* group ,const gchar* key);
void* SetKey(gchar* file, const gchar* group , const gchar* key, const gchar* content);
gint getUserId();
gchar* getMountByDevice(gchar* device);
void makeConfDirs();
