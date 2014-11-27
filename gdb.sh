
pid=`ps -ef|grep air_server|grep -v grep| awk '{print $2}'`
echo $pid
gdb -p $pid --command=gdb.cm
