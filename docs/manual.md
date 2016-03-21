# The Gino manual

Gino is a simple handler for Linux Kernel events. Originally for 
automounting, it can now be used to run arbitrary commands when events
occurs.

Gino uses a configuration file called gino.conf It can be in 
`/etc/gino.conf` if you are root, or in `$HOME/conf/gino/gino.conf` if 
you are a user.


If you want you can also specify a custom file gino flipping with the 
following command: 

```
./gino --conf=myfile.conf

```


##Automounting 
Once installed Gino is ready to make the automounting of devices. 
In other words it will mount your disks and pendrive thanks to udisks.

```
alwaysMounts=yes
mountCommand=udisksctl mount -b %s 

```

where %s is the device (partition). 
Once you mounted you can run a command or script on mount point

```
openCommand=spacefm
```
In this case run spacefm (graphic file manager).

## Rules
....
