
socket_path='/usr/bin/aesdsocket'
case "$1" in
	start)
		echo "Starting aesdsocket"
		start-stop-daemon -S -n aesdsocket -a "${socket_path}" -- -d 
		;;
	stop)
		echo "stopping aesdsocket"
		start-stop-daemon -K -n aesdsocket
		;;
	*)
		echo "usage $0 {start | stop}"
	exit 1
esac
exit 0
