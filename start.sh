pid=`ps -ef|grep air_server|grep -v grep| awk '{print $2}'`
kill -9 $pid
sleep 1
./air_server
