#测试同步
begin=`date +%H:%M:%S`
echo "begin time:$begin"

for ((i=1;i<2;i++))
do
    python test.py
    sleep 1
    #echo $i
done

end=`date +%H:%M:%S`
echo "end time:$end"

