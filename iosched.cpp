#include <iostream> //cout
#include <fstream> //ifstream
#include <sstream> //Stringstream
#include <string>
#include <vector>
#include <stdio.h>
#include <getopt.h> //Arg parsing
#include <stdlib.h>
#include <queue>

using namespace std;

// Flags
int vFlag = 0;
int qFlag = 0;
int fFlag = 0;

// Struct
struct IORequest {
    IORequest(int t, int tr) : time(t), track(tr) {}
    int time;
    int track;
};

// Class definitions
class IOScheduler {
public:
    virtual ~IOScheduler() {}
private:
};

class FIFO : public IOScheduler{

};

class SSTF : public IOScheduler{

};

class LOOK : public IOScheduler{

};

class CLOOK : public IOScheduler{

};

class FLOOK : public IOScheduler{

};

// Global
queue<IORequest*> requestQueue;

int main(int argc, char* argv[]) {
    int c;
    IOScheduler* myIOSched;
    while ((c = getopt(argc,argv,"s:vqf")) != -1 )
    {   
        // ./iosched –s<schedalgo> [options] <inputfile> // options are -v -q -f (see below)
        switch(c) {
            case 's': 
                char algo;
                sscanf(optarg, "%c",&algo);
                switch (algo) {
                    case 'i':
                        myIOSched = new FIFO(); // First come first serve
                        break;
                    case 'j':
                        myIOSched = new SSTF(); // Shortest seek time first
                        break;
                    case 's':
                        myIOSched = new LOOK();
                        break;
                    case 'c':
                        myIOSched = new CLOOK();
                        break;
                    case 'f':
                        myIOSched = new FLOOK();
                        break;
                    default:
                        cerr << "Error: Unknown algo: " << algo << endl;
                        exit(1);
                }
                break;
            case 'v':
                vFlag = 1;
                break;
            case 'q':
                qFlag = 1;
                break;
            case 'f':
                fFlag = 1;
                break;
        }        
    }
    if ((argc - optind) < 1) { //optind is the index of current argument
        cerr << "Error: Missing input file\n";
        exit(1);
    }

    //Gettng file names
    char* inputFile = argv[optind];
    
    //Opening random value file and creating random number generator
    ifstream ifs(inputFile);
    if (!ifs) {
        cerr << "Error: Could not open the rfile.\n";
        exit(1);
    }
    int time,track;
    string line;
    while (getline(ifs, line)) {
        if (line[0] == '#') {
            continue;
        }
        istringstream iss(line);
        iss >> time >> track; //number of procs
        cout << time << " " << track << endl;
        requestQueue.push(new IORequest(time,track));
    }
    
    ifs.close();
    delete myIOSched;
    while (!requestQueue.empty()){
        delete requestQueue.front();
        requestQueue.pop();
    }
}