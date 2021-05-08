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

// Macros
#define vtrace(fmt...)  do { if (vFlag) { printf(fmt); fflush(stdout); } } while(0)
#define qtrace(fmt...)  do { if (qFlag) { printf(fmt); fflush(stdout); } } while(0)
#define ftrace(fmt...)  do { if (fFlag) { printf(fmt); fflush(stdout); } } while(0)

// Struct
struct IORequest {
    static int count;
    IORequest(int at, int tr) : id(count++), arrival_time(at), track(tr), service_start(-1), service_end(-1){}
    void print(){
        printf("id[%d], time[%d], track[%d]\n", id, arrival_time, track); 
    }
    void print_sol(){
        printf("%5d: %5d %5d %5d\n",id, arrival_time, service_start, service_end);
    }
    int id;
    int arrival_time;
    int track;
    int service_start;
    int service_end;
};
int IORequest::count = 0;

// Class definitions
class IOScheduler {
public:
    virtual ~IOScheduler() {}
private:
    queue<IORequest*> IOQueue;
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

// Prototype functions
int simulation(IOScheduler*, vector<IORequest*>&);

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
    
    //Opening input file
    ifstream ifs(inputFile);
    if (!ifs) {
        cerr << "Error: Could not open input file.\n";
        exit(1);
    }
    int time, track;
    string line;
    vector<IORequest*> requestVector;
    while (getline(ifs, line)) {
        if (line[0] == '#') {
            continue;
        }
        istringstream iss(line);
        iss >> time >> track;
        requestVector.push_back(new IORequest(time,track));
    }
    ifs.close();
    if (requestVector.empty()) {
        cerr << "No requests were given." << endl;
        exit(1);
    }

    // Simulation
    int tot_movement = simulation(myIOSched, requestVector);

    // Results 
    int total_time = 0;
    double avg_turnaround = 0;
    double avg_waittime = 0;
    int max_waittime = 0;
    for (size_t i = 0; i < requestVector.size(); i++){
        IORequest* req = requestVector[i];
        req->print_sol();
        total_time = max(total_time, req->service_end);
        avg_turnaround += (req->service_end - req->arrival_time);
        int wait_time = req->service_start - req->arrival_time;
        avg_waittime += wait_time;
        max_waittime = max(max_waittime, wait_time);
    }
    avg_turnaround /= requestVector.size();
    avg_waittime /= requestVector.size();
    printf("SUM: %d %d %.2lf %.2lf %d\n",total_time, tot_movement, avg_turnaround, avg_waittime, max_waittime);

    // Clean up
    delete myIOSched;
    for (size_t i = 0; i < requestVector.size(); i++){
        delete requestVector[i];
    }
}

int simulation(IOScheduler* myIOSched, vector<IORequest*>& requestVector){
    int n = requestVector.size();
    int tot_movement = 0;
    int currentTime = 0;
    int currentRequestIndex = 0;
    int track = 0; // 1 track = 1 time
    IORequest* currentRequest = NULL;
   

    while (false) {

        currentTime += 1;
    }

    return tot_movement;
}