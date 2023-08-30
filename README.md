 ~~~
 █████   █████                   ███████████                               █████     
░░███   ░░███                   ░░███░░░░░███                             ░░███      
 ░███    ░███   ██████   ██████  ░███    ░███  ██████  ████████    ██████  ░███████  
 ░███████████  ███░░███ ███░░███ ░██████████  ███░░███░░███░░███  ███░░███ ░███░░███ 
 ░███░░░░░███ ░███ ░░░ ░███ ░░░  ░███░░░░░███░███████  ░███ ░███ ░███ ░░░  ░███ ░███ 
 ░███    ░███ ░███  ███░███  ███ ░███    ░███░███░░░   ░███ ░███ ░███  ███ ░███ ░███ 
 █████   █████░░██████ ░░██████  ███████████ ░░██████  ████ █████░░██████  ████ █████
░░░░░   ░░░░░  ░░░░░░   ░░░░░░  ░░░░░░░░░░░   ░░░░░░  ░░░░ ░░░░░  ░░░░░░  ░░░░ ░░░░░
 ~~~
                                                                                                                                                                         
# Yet another proggi to test your network applications on the fly

HccBench provides a simple way to transmit and receive data packets over the network in scenarios such as network tests to analyze data transmission and/or monitoring quality and robustness. Therefore, it consists of 2 programs, which are presented below:


HccBench-S or sender.c: A program to send data packets.

The sender.c program is designed to send data packets to a remote recipient using the TCP/IP protocol. It allows the user to specify the size of the packets, the number of packets to send, and the delay time between packets. Additionally, a configuration file (senderconfig.txt and receiverconfig.txt) can be used to specify the destination IP/TCP and TCP to listen to. The program generates random data for each packet and sends it to the receiver. It provides feedback on the progress of the sending process, including timestamps and packet details.

HccBench-R or receiver.c: A program to receive data packets

The receiver.c program receives data packets from the remote sender over the TCP/IP protocol. It reads the configuration file (receiverconfig.txt) to configure the receive port and buffer size. The program waits for incoming packets and outputs information about the receiving process, including timestamp, packet size and content of packets. Using this program makes it possible to receive and monitor data from a remote sender.


#Short user guide:

To HccBench-R (the receiver):
-----------------------------------
1) Open Receiverconfig.txt in your notepad or so:
2) Edit the port you expect the packets to come from
3) Edit the maximum size of the packets you want to receive
4) Run Receiver.exe in a CMD (always before Sender.exe)
5) Start Sender.exe
6) Wait until Sender.exe is finished --> Usually, Receiver will shut down - if not, kill it with ctrl-c or so
7) Make some statistics in Excel, R, or whatever with the data in Senderoutput and Receiveroutput.

Tip: Save your output files; otherwise, the data will be overwritten!!!!


To HccBench-S (the transmitter):
------------------------------------
1) Open Senderconfig.txt in your notepad or so:
2) Edit the IP to which you want to send the test packets
3) Edit the port on which you want to smash your packets
4) Start Sender.exe in a CMD
5) Enter the desired packet width in bytes (but max. one byte less than configured in receiverconfig.txt...yeah, it's a bug, I know!!!)
6) Enter the number of packets you want to send
7) Enter the milliseconds of sleep time between consecutive packets
8) Flood your network...
9) Make some statistics in Excel, R, or whatever using the data in Senderoutput and Receiveroutput. --> See the suggestions bellow

Tip: Also here -- Save your output files; otherwise, the data will be overwritten!!!!





# Stuff you can do with it for Analysis

Very basic metrics:
1) Total Packages Sent: Evaluate the number of records in the sender file.
2) Total Packages Received: Evaluate the number of records in the receiver file.
3) Using a file comparison tool, compare receiveroutput.txt to senderoutput.txt to check for out-of-order packets and duplicate packets.

Use Excel or any statistics tool:
4) Packets Arrived Unchanged: This is the number of packets for which the sender file's packet content matches the receiver file's packet content.
5) Packets arrived modified: This is the number of packets for which the packet content in the source file does not match the packet content in the destination file.
6) Transit time of each packet: This is the difference in timestamps between the receiver and the sender for each packet.
7) Average Transit Time: This is the average transit time for all packets.
8) Standard Deviation of Transit Time: This is the standard deviation of the transit times for all packets.
9) Packet Loss: The number of packets sent but not received.
10) Throughput: This is the total number of bits received divided by the total time to receive them.
Error rate: This is the number of packets received with errors divided by the total number of packets received.

11) Bandwidth: If you have done enough tests, including unsuccessful network tests with very high packets and very short intervals, you can estimate the maximum data transfer rate over the network. It can be calculated as the total number of bits sent or received divided by the total time.

12) Runtime distribution: Create a histogram of the transit times of all packets to visualize the distribution.
13) Packet Size Distribution: Analyze the distribution of packet sizes at both the sender and receiver side.
14) Create a time series plot of all packet transit times to identify any current trends or periodic patterns. 
15) Jitter analysis: Jitter refers to the variation in latency across a network. In other words, it is the variation in the time it takes packets to get from one point to another.

# Stuff you cannot do with it for Analysis

 At least not without further development of the two proggis:
1) Round Trip Time (RTT)
2) Utilization: This is the percentage of time the network is used to transmit packets. It can be calculated as the total time taken to transmit all packets divided by the total time period.
3) Retransmissions:  
4) TCP connection establishment time 
5) TCP connection termination time:  
