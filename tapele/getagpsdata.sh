#!/system/bin/sh
set /storage/.agpsdata
touch $1
chmod 644 $1

sh /vendor/bin/launch.sh

#chown gps:gps $1
while true; do
        line=`/system/bin/ping -c1 -s1 -w1 8.8.8.8 2>/dev/null | grep "packet"|wc -l`
        if [ "${line}" != "0" ]; then
                while true; do
                        date > $1
                        timeout 10 /vendor/bin/supl-client --cell gsm:0,0:0,0 supl.vodafone.com >> $1
                        [ $? -eq 0 ] && sleep 7000
                done
        else
                echo "no network" > $1
                sleep 5
        fi
done

