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
    IORequest(int t, int tr) : id(count++), time(t), remain(t),
        track(tr), service_start(-1), service_end(-1) {}
    void print(){
        printf("id[%d], time[%d], track[%d]\n", id, time, track); 
    }
    void print_sol(){
        printf("%5d: %5d %5d %5d\n", id, time, service_start, service_end);
    }
    int id;
    int time;
    int remain;
    int track;
    int service_start;
    int service_end;
};
int IORequest::count = 0;

// Class definitions
class IOScheduler {
public:
    IOScheduler() : direction(1) {} // Direction is always up first
    virtual ~IOScheduler(){}
    virtual void add_to_queue(IORequest*) = 0;
    virtual IORequest* get_from_queue() = 0;
    int get_direction(){ return direction; }
    bool get_empty() { return IOQueue.empty(); }
protected:
    queue<IORequest*> IOQueue;
    int direction;
};

class FIFO : public IOScheduler{
public:
    void add_to_queue(IORequest* req){
        IOQueue.push(req);
    }
    IORequest* get_from_queue(){
        if (IOQueue.empty()) { return NULL; }
        IORequest* result = IOQueue.front();
        IOQueue.pop();
        return result;
    }
};

class SSTF : public IOScheduler{
public:
    void add_to_queue(IORequest* req){

    }
    IORequest* get_from_queue(){
        IORequest* result;
        return result;
    }
};

class LOOK : public IOScheduler{
public:
    void add_to_queue(IORequest* req){

    }
    IORequest* get_from_queue(){
        IORequest* result;
        return result;
    }
};

class CLOOK : public IOScheduler{
public:
    void add_to_queue(IORequest* req){

    }
    IORequest* get_from_queue(){
        IORequest* result;
        return result;
    }
};

class FLOOK : public IOScheduler{
public:
    void add_to_queue(IORequest* req){

    }
    IORequest* get_from_queue(){
        IORequest* result;
        return result;
    }
};

// Prototype functions
int simulation(IOScheduler*, queue<IORequest*>&, vector<IORequest*>&);

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
    queue<IORequest*> requests;
    while (getline(ifs, line)) {
        if (line[0] == '#') {
            continue;
        }
        istringstream iss(line);
        iss >> time >> track;
        requests.push(new IORequest(time,track));
    }
    ifs.close();
    if (requests.empty()) {
        cerr << "No requests were given." << endl;
        exit(1);
    }

    // Size and results
    int size = requests.size();
    vector<IORequest*> completedReqs(size,NULL);

    // Simulation
    int tot_movement = simulation(myIOSched, requests, completedReqs);

    // Results 
    int total_time = 0;
    double avg_turnaround = 0;
    double avg_waittime = 0;
    int max_waittime = 0;
    for (size_t i = 0; i < size; i++){
        IORequest* req = completedReqs[i];
        req->print_sol();
        total_time = max(total_time, req->service_end);
        avg_turnaround += (req->service_end - req->time);
        int wait_time = req->service_start - req->time;
        avg_waittime += wait_time;
        max_waittime = max(max_waittime, wait_time);
    }
    avg_turnaround /= size;
    avg_waittime /= size;
    printf("SUM: %d %d %.2lf %.2lf %d\n",total_time, tot_movement, avg_turnaround, avg_waittime, max_waittime);

    // Clean up
    delete myIOSched;
    for (size_t i = 0; i < size; i++){
        delete completedReqs[i];
    }
}

int simulation(IOScheduler* myIOSched, queue<IORequest*>& requestQueue, vector<IORequest*>& completedReqs){
    int tot_movement = 0;
    int currentTime = 0;
    int currentTrack = 0; // 1 track = 1 time
    IORequest* currentRequest = NULL;
    vtrace("TRACE\n");
    while (true) {
        // There is still requests, and current time is its arrival time
        if (currentTime == requestQueue.front()->time) {
            vtrace("%d:\t%d add %d\n", currentTime, requestQueue.front()->id, 
                requestQueue.front()->track);
            // Add to queue
            myIOSched->add_to_queue(requestQueue.front());
            // Pop request
            requestQueue.pop();
        }
        // There is a current request, and its end time is now
        if ((currentRequest != NULL) && (currentRequest->service_end == currentTime)) {
            // Request complete - Service end - starting time
            vtrace("%d:\t%d finish %d\n", currentTime, currentRequest->id, 
                currentRequest->service_end-currentRequest->time);
            completedReqs[currentRequest->id] = currentRequest;
            currentRequest = NULL;
            currentTrack += 1;
            tot_movement += 1;
        }
        // There is a current request, and its end time is not now
        if ((currentRequest != NULL) && (currentRequest->service_end != currentTime)) {
            // Move the track
            currentTrack += 1;
            tot_movement += 1;
            // currentRequest->remain -= 1;
        }
        // There is no current request, but there are pending requests
        if ((currentRequest == NULL) && !myIOSched->get_empty()){
            currentRequest = myIOSched->get_from_queue();
            // Start serving this request now
            currentRequest->service_start = currentTime;
            // Service ends after track time
            currentRequest->service_end = currentRequest->service_start + abs(currentRequest->track - currentTrack) +
                currentRequest->time;
            printf("start[%d] abs[%d] time[%d]\n", currentRequest->service_start, abs(currentRequest->track - currentTrack),
                currentRequest->time);
            // Issue
            vtrace("%d:\t%d issue %d %d %d %d\n", currentTime, 
                currentRequest->id, currentRequest->track, currentTrack, 
                currentRequest->service_start, currentRequest->service_end);
        }
        // No current request, no pending request
        if ((currentRequest == NULL) && requestQueue.empty() && myIOSched->get_empty()) {
            break; // End sim
        }
        currentTime += 1; // Time goes up each loop
    }

    return tot_movement;
}