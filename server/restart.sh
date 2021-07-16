p=$(ps aux | grep chatster-c/server/server | awk '{print $2}' | head -n 1)
kill -9 $p
echo "Killed process $p"

gcc server.c -o server
echo "Compiled server.c"

/home/kippster/code/c/chatster-c/server/server &
echo "Started server"


