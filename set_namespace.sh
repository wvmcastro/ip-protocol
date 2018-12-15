#!/bin/bash

ip netns add h1
ip netns add h2
ip netns add r

ip link add h1-eth0 type veth peer name r-eth0
ip link add h2-eth0 type veth peer name r-eth1

ip link set h1-eth0 netns h1
ip link set h2-eth0 netns h2
ip link set r-eth0 netns r
ip link set r-eth1 netns r

ip netns exec h1 ifconfig lo up
ip netns exec h1 ifconfig h1-eth0 up

ip netns exec h2 ifconfig lo up
ip netns exec h2 ifconfig h2-eth0 up

ip netns exec r ifconfig lo up
ip netns exec r ifconfig r-eth0 up
ip netns exec r ifconfig r-eth1 up


# add the the ips to the interfaces of the veth of hosts
ip netns exec h1 ip addr add 192.168.1.11/24 dev h1-eth0
ip netns exec h2 ip addr add 192.168.2.11/24 dev h2-eth0

# add the the ips to the interfaces of the veth
# ip netns exec r ip addr add 192.168.1.10/24 dev r-eth0
# ip netns exec r ip addr add 192.168.2.10/24 dev r-eth1

#add a route for all the packets to the router interface
ip netns exec h1 ip route add default via 192.168.1.10
ip netns exec h2 ip route add default via 192.168.2.10

ip netns exec r ./ipd/ipd r-eth0 r-eth1
