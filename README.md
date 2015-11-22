# hdd_monitor

hdd_monitor is a hard disk health monitor program via S.M.A.R.T.
You can logging hard disk status(sleep/active), temperature, health condition
(GOOD/CAUTION/FAIL).

It works on 2.6 or later linux kernel

# Requirements

* libatasmart
http://0pointer.de/blog/projects/being-smart.html

* You need a working development environment that can compile programs.
On popular Linux distributions, this means having the glibc development
package(s) installed.

# Before you start

Make sure that your system has S.M.A.R.T. support. Please check bios setting and,
hard disk spec. 

# Getting hdd_monitor

Source codes are available on the Github snapshots. 
https://github.com/suzutsuki0220/hdd_monitor

# Compile

Move to source code directory and run make

<pre>
$ cd hdd_monitor
$ make
</pre>

# How to use

<pre>
hdd_monitor [OPTIONS] DEVICES
</pre>

## OPTIONS

* -l: output log directory (If this option not set, save to current directory.)
* -s: check span second (Default 60 second)

## Example

In this option, check health of three disks every 1 hour
and write log to /var/log/hdd_monitor

<pre>
# hdd_monitor -l /var/log/hdd_monitor -s 3600 /dev/sda /dev/sdb /dev/sdc
</pre>

# log format

## Directory and Filename

Sub directory is made by a date(YYYY-MM-DD style) in a specified ouptput directory.
Filename is hd_health and devicename (replace '/' to '-').

Example
<pre>
$ ls -lR
.:
total 12
drwxr-xr-x 2 root root 4096 Nov 15 22:15 2015-11-15/
drwxr-xr-x 2 root root 4096 Nov 16 00:05 2015-11-16/
drwxr-xr-x 2 root root 4096 Nov 17 00:37 2015-11-17/

./2015-11-15:
total 12
-rw-r--r-- 1 root root 165 Nov 15 23:55 hd_health-dev-sda.txt
-rw-r--r-- 1 root root 165 Nov 15 23:55 hd_health-dev-sdb.txt
-rw-r--r-- 1 root root 165 Nov 15 23:55 hd_health-dev-sdc.txt

./2015-11-16:
total 12
-rw-r--r-- 1 root root 754 Nov 16 23:05 hd_health-dev-sda.txt
-rw-r--r-- 1 root root 754 Nov 16 23:05 hd_health-dev-sdb.txt
-rw-r--r-- 1 root root 754 Nov 16 23:05 hd_health-dev-sdc.txt
</pre>

## Fields

1. check time
2. status (1: active, 2: sleep)
3. temperature of celsius
4. health status (GOOD or CAUTION or FAIL)

Example
<pre>
23:05:54,0,27,GOOD
</pre>
