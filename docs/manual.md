# The Gino manual

Gino is a simple handler for Linux Kernel events. Originally for 
automounting, it can now be used to run arbitrary commands when events
occurs.

Gino uses a configuration file called gino.conf It can be in 
`/etc/gino.conf` if you are root, or in `$HOME/config/gino/gino.conf` if 
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

The rules are contained in a specific folder called rules. If you are 
root folder is `/usr/share/gino/rules`, otherwise 
`$HOME/.config/gino/rules/`.
You can choose a custom file by modifying the directive in `gino.conf` .

```
rulesDir=/home/fabio/.config/gino/rules/
```

### How the rules
It's very simple. Just create a file with the id of the device or 
partition as the file name and add the extension .conf . 
And then add in the connected or disconnected subfolder according to 
your needs.

like `usb-SXX_Mass_Storage_Device_XXXXXX-0:0-part1.conf`
You can get the name of the device launching in a terminal ./gino and 
after inserting the device.



Within the file are the directives with the commands to launch on the 
device.

```
[Rules]
command=echo "Pendrive 2 GB connected"
```

Or if you want mount a specific disk:

```
[Rules]
command=udisksctl mount -b 
```
