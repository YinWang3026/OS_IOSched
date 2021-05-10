#include <iostream> // cout
#include <fstream> // ifstream
#include <sstream> // istringstream
#include <string>
#include <vector>
#include <stdio.h>
#include <getopt.h> //Arg parsing
#include <stdlib.h>
#include <queue>
#include <deque>

using namespace std;

// Flags
int vFlag = 0;
int qFlag = 0;
int fFlag = 0;

// Macros
#define vtrace(fmt...)  do { if (vFlag) { printf(fmt); fflush(stdout); } } while(0)
#define qtrace(fmt...)  do { if (qFlag) { printf(fmt); fflush(stdout); } } while(0)
#define ftrace(fmt...)  do { if (fFlag) { printf(fmt); fflush(stdout); } } while(0)
#define MAX_OP 10000 // Max number of IO Operations

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
    virtual IORequest* get_from_queue(int) = 0;
    virtual bool get_empty() = 0;
    int get_direction(){ return direction; }
protected:
    int direction;
};

class FIFO : public IOScheduler{
public:
    void add_to_queue(IORequest* req){
        IOQueue.push(req);
    }
    IORequest* get_from_queue(int currentTrack){
        if (IOQueue.empty()) {
            return NULL; 
        }
        IORequest* result = IOQueue.front();
        IOQueue.pop();
        // Set dir based on position of track
        if (result->track > currentTrack) {
            direction = 1;
        } else if (result->track < currentTrack) {
            direction = -1;
        }
        return result;
    }
    bool get_empty() { return IOQueue.empty(); }
private:
    queue<IORequest*> IOQueue;
};

class SSTF : public IOScheduler{
public:
    SSTF() : IOQueue(MAX_OP, NULL), size(0) {}
    void add_to_queue(IORequest* req){
        IOQueue[req->id] = req;
        size += 1;
    }
    IORequest* get_from_queue(int currentTrack){
        int resultInd = -1;
        uint distance = INT_MAX;
        // Find smallest distance
        for(size_t i = 0; i<MAX_OP; i++){
            if (IOQueue[i] != NULL){
                uint tempDist = abs(IOQueue[i]->track - currentTrack);
                qtrace("%d:%d ", IOQueue[i]->id, tempDist);
                if (tempDist < distance){
                    distance = tempDist;
                    resultInd = i;
                }
            }
        }
        qtrace("\n");
        if (resultInd == -1) {
            return NULL; 
        }
        IORequest* result = IOQueue[resultInd];
        IOQueue[resultInd] = NULL;
        size -= 1;
        // Set dir based on track
        if (result->track > currentTrack) {
            direction = 1;
        } else if (result->track < currentTrack) {
            direction = -1;
        }  
        return result;
    }
    bool get_empty() { return size == 0; }
private:
    vector<IORequest*> IOQueue;
    int size;
};

class LOOK : public IOScheduler{
public:
    LOOK() : IOQueue(MAX_OP, NULL), size(0) {}
    void add_to_queue(IORequest* req){
        IOQueue[req->id] = req;
        size += 1;
    }
    // Helper function, finds the closest request to track
    int get_closest(int currentTrack){
        int resultInd = -1;
        int distance = INT_MAX;
        // Find smallest distance
        for(size_t i = 0; i<MAX_OP; i++){
            if (IOQueue[i] != NULL){
                int tempDist = (IOQueue[i]->track - currentTrack) * direction;
                if (0 <= tempDist){
                    qtrace("%d:%d ", IOQueue[i]->id, tempDist);
                    if (tempDist < distance){
                        distance = tempDist;
                        resultInd = i;
                    }
                }
            }
        }
        return resultInd;
    }
    IORequest* get_from_queue(int currentTrack){
        int resultInd = get_closest(currentTrack);
        if (resultInd == -1) {
            direction *= -1;
            qtrace("--> change direction to %d\n", direction);
            resultInd = get_closest(currentTrack);
        } 
        IORequest* result = IOQueue[resultInd];
        IOQueue[resultInd] = NULL;
        size -= 1;
        qtrace("--> %d dir=%d\n", result->id, direction);
        return result;
    }
    bool get_empty() { return size == 0; }
private:
    vector<IORequest*> IOQueue;
    int size;
};

// class CLOOK : public IOScheduler{
// public:
//     void add_to_queue(IORequest* req){

//     }
//     IORequest* get_from_queue(int){
//         IORequest* result = NULL;
//         return result;
//     }
// };

// class FLOOK : public IOScheduler{
// public:
//     void add_to_queue(IORequest* req){

//     }
//     IORequest* get_from_queue(int){
//         IORequest* result = NULL;
//         return result;
//     }
// };

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
                    // case 'c':
                    //     myIOSched = new CLOOK();
                    //     break;
                    // case 'f':
                    //     myIOSched = new FLOOK();
                    //     break;
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
        avg_turnaround += (req->service_end - req->time); // Service end - req time
        int wait_time = req->service_start - req->time; // Between service start and req time
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
    uint tot_movement = 0;
    uint currentTime = 0;
    uint currentTrack = 0; // 1 track = 1 time
    IORequest* currentRequest = NULL;
    vtrace("TRACE\n");
    while (true) {
        // There are requests, and current time is its arrival time
        if (!requestQueue.empty() && currentTime == requestQueue.front()->time) {
            // Get request
            IORequest* temp = requestQueue.front();
            requestQueue.pop();
            vtrace("%d:\t%d add %d\n", currentTime, temp->id, temp->track);
            // Add to queue
            myIOSched->add_to_queue(temp);
        }
        // There is a current request, and requested track is the current track
        if (currentRequest != NULL && currentRequest->track == currentTrack) {
            currentRequest->service_end = currentTime;
            // Request complete
            vtrace("%d:\t%d finish %d\n", currentTime, currentRequest->id, 
                currentRequest->service_end-currentRequest->time);
            completedReqs[currentRequest->id] = currentRequest;
            currentRequest = NULL;
        } 
        // There is a current request, and requested track is not the current track
        if (currentRequest != NULL && currentRequest->track != currentTrack) {
            // Move the track in my direction
            currentTrack += myIOSched->get_direction();
            tot_movement += 1;
        } 
        // There is no current request, but there are pending requests
        if (currentRequest == NULL && !myIOSched->get_empty()){
            currentRequest = myIOSched->get_from_queue(currentTrack);
            // Start serving this request now
            currentRequest->service_start = currentTime;
            // Issue
            vtrace("%d:\t%d issue %d %d\n", currentTime, 
                currentRequest->id, currentRequest->track, currentTrack);
            // Time goes up right after, so track needs to go up too
            if (currentRequest->track == currentTrack) {
                // No movement, we are on the current strip
                continue;
            }
            currentTrack += myIOSched->get_direction();
            tot_movement += 1;
        }
        // No current request, no pending request
        if (currentRequest == NULL && requestQueue.empty() && myIOSched->get_empty()) {
            break; // End sim
        }
        currentTime += 1; // Time goes up each loop
    }
    return tot_movement;
}