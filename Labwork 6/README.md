# Report 6

## Install GlusterFS

Add the GlusterFS PPA repository (version 3.10.x) and update.

Note: All peers must have the same version of GlusterFS.

```
sudo add-apt-repository ppa:gluster/glusterfs-3.10
sudo apt-get update
```

Install GlusterFS package.

```
sudo apt-get install -y glusterfs-server
```

Configure the firewall

```
sudo iptables -I INPUT -p all -s <ip-address> -j ACCEPT

```

## Make a Trusted Pool

Get the server IP Address.

```
hostname -I
```

Add servers to the Trusted Pool

```
sudo gluster peer probe <ip-address>
```

Check the peer status

```
sudo gluster peer status
```

## Create a Distributed Replicated Volume

### Add a Storage

For each node, make a folder in the /home directory to act as a Brick (ex. ~/data/gluster).

### Setup a Volume

Create a Brick in each node (ex. ~/data/gluster/gv0).

Create a Distributed Replicated Volume.

```
sudo gluster volume create <VOL_NAME> replica <INT> transport tcp <HOSTNAME>:<BRICK_PATH>... force
```

For instance:

```
sudo gluster volume create test-volume replica 3 transport tcp 192.168.10.250:/home/dangvinhbao/data/gluster/gv0 192.168.11.23:/home/vhlinh/data/gluster/gv0 192.168.11.0:/home/tuan/data/gluster/gv0 force
```
Start the Volume.

```
sudo gluster volume start <VOL_NAME>
```

Check the Volume status.
```
sudo gluster volume info <VOL_NAME>
```

## Setup the Client

Install the GlusterFS Client.

```
sudo apt-get install -y glusterfs-client
```

Create a directory to mount the GlusterFS filesystem (ex. ~/mnt/glusterfs).

Mount the GlusterFS filesystem to the Client.

```
mount -t glusterfs <HOSTNAME>:/<VOL_NAME> <MOUNT_DIR>
```

For instance:

```
mount -t glusterfs 192.168.11.23:/test-volume ~/mnt/glusterfs
```

Verify the mounted GlusterFS filesystem.

```
# df -h <MOUNT_DIR>
```

