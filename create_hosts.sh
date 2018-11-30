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
