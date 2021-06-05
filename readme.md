# IO SCheduler
Accessing data on a hard drive is time consuming, because a physical needle needs to be moved around to read the disk. As a result, there are few policies that the Operating System use to process the incoming IO requests. Here, different policies are simulated using Object Oriented Programming. The policies simulated are FIFO (First In First Out), SSTF (Shortest Seek Time First), LOOK, CLOOK, and FLOOK. The simulation runs until all requests are completed. The result of each policy is displayed at the end.

## Running the Program
1. Compile the cpp file with the Makefile
2. ./iosched –s< policy > [-vqf] [inputfile]
- The -vqf are optional flags for more runtime information
- The -s flag specifies the policy
  - i=FIFO
  - j=SSFT
  - s=LOOK
  - c=CLOOK
  - f=FLOOK

## Input Files
- Mandatory inputs
- Each line is an instruction that specifies the time that a request is issued and the track on the disk that is needed.