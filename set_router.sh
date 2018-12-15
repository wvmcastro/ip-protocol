ip netns exec r ./xifconfig/xifconfig r-eth0 192.168.1.10 255.255.255.0
ip netns exec r ./xifconfig/xifconfig r-eth1 192.168.2.10 255.255.255.0
ip netns exec r ./xarp/xarp ttl -1
ip netns exec r ./xarp/xarp res 192.168.1.11
ip netns exec r ./xarp/xarp res 192.168.2.11
ip netns exec r ./xarp/xarp res 192.168.1.10
ip netns exec r ./xarp/xarp res 192.168.2.10
ip netns exec r ./xroute/xroute add 192.168.2.0 255.255.255.0 192.168.2.11
ip netns exec r ./xroute/xroute add 192.168.1.0 255.255.255.0 192.168.1.11
