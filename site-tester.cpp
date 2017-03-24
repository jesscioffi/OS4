// Esmeralda Cervantes and Jessica Cioffi
// Operating Systems Project 4

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <utility>
#include <ctime>
#include <typeinfo>
#include <thread>
#include <signal.h>
#include <pthread.h>
#include <thread>
#include <mutex>

using namespace std;

// Global Variables
struct MemoryStruct {
    char *memory;
    size_t size;
};

struct MemoryStruct chunk;

struct Pair {
    string url;
    string code;
};

pthread_cond_t fetchEmpty;
pthread_cond_t parseEmpty;
int PERIOD_FETCH = 180;
int NUM_FETCH = 1;
int NUM_PARSE = 1;
string SEARCH_FILE = "";
string SITE_FILE = "";
vector<string> searches;
vector<string> sites;
pthread_mutex_t lock0 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock3 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock4 = PTHREAD_MUTEX_INITIALIZER;
ofstream fs;
int COUNTER = 1;
int keeplooking = 1;
int keeplooking2 = 1;

// Functions
void error(string message){
    cout << "ERROR: " << message << endl;
    exit(1);
}

// function to return the time
void time(){
    time_t t = time(0);
    struct tm *now = localtime(&t);

    fs << (now->tm_mon + 1) << "-";

    fs << (now->tm_mday) << "-";

    fs << (now->tm_year - 100) << "-";

    fs << (now->tm_hour) << ":";

    fs << (now->tm_min) << ":";

    fs << (now->tm_sec);
}

// function to return vector of str from site and search files
vector<string> get(char const* ofile){
    string info;
    vector<string> vstrings;
    ifstream ifs;
    ifs.open(ofile);
    if (!ifs.is_open()){
        error("Unable to open file");
    }
    while (ifs >> info){
        vstrings.push_back(info);
    }
    ifs.close();
    return vstrings;
}

void handler(int s) {
    sites=get(SITE_FILE.c_str());
    pthread_cond_broadcast(&fetchEmpty);
    COUNTER++;
    fs.close();
    if (keeplooking2 == 1) {
        alarm(PERIOD_FETCH);
        stringstream ss;
        ss<<COUNTER;
        string f = ss.str();
        string n = f.append(".csv");
        fs.open(n);
    }
    else {
        keeplooking = 0;
    }
}

void h (int p) {
    keeplooking2 = 0;
}

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size *nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    mem->memory = (char *) realloc(mem->memory, mem->size + realsize + 1);
    if(mem->memory == NULL) {
        printf("Not enough memory (realloc returned NULL)\n");
        return 0;
    }

    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

bool webFetcher(string website, string &buffer) {
    CURL *curl_handle;
    CURLcode res;
    
    chunk.memory = (char *)malloc(1);
    chunk.size = 0;

    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, website.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    res = curl_easy_perform(curl_handle);

    if(res != CURLE_OK) {
//        cout << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
        cout << "curl failed!" << endl;
        return false;
    }
    else {
    }

    buffer = chunk.memory;

    curl_easy_cleanup(curl_handle);
    free(chunk.memory);

    return true;
}  

int wordCount(string file, string phrase) {
    int counter = 0;
    size_t index = -1;
    do {
        index = file.find(phrase, index+1);
        if (index != string::npos) {
            counter++;
        }
    } while (index != string::npos);
    return counter;
}

vector<Pair> curlfetch;

void * fetch(void * uneeded) {
    string add = "";

    while(1) { //prevents the thread from dying so you do not have to create more threads
        pthread_mutex_lock(&(lock0));

        while(sites.empty()) { //the queue holding the urls
            pthread_cond_wait(&fetchEmpty, &lock0); //obviously more args  
        }

        string str = sites.back(); //pop url from queue
        sites.pop_back();
        
        pthread_cond_broadcast(&fetchEmpty); //not correct syntax, but you need to broadcast
        pthread_mutex_unlock(&(lock0));

        bool success = webFetcher(str, add);
        if (success == true) {
            pthread_mutex_lock(&lock2);
            Pair temporary;
            temporary.url = str;
            temporary.code = add;
            curlfetch.push_back(temporary);
            pthread_cond_broadcast(&parseEmpty);
            pthread_mutex_unlock(&lock2);
        }

    }
    return NULL;
}

void * parse (void * k) {
    while(1) {
        pthread_mutex_lock(&(lock4));
        while (curlfetch.empty()) {
            pthread_cond_wait(&parseEmpty, &lock4);
        }

        Pair str1 = curlfetch.front();
        curlfetch.pop_back();
        pthread_cond_broadcast(&parseEmpty);
        pthread_mutex_unlock(&(lock4));

        for (size_t i = 0; i < searches.size(); i++){
            time();
            pthread_mutex_lock(&(lock3));
            fs << "," << searches[i] << "," << str1.url << "," << wordCount(str1.code, searches[i]) << endl;
            pthread_mutex_unlock(&(lock3));
        }
    }
    return NULL;
}

int main (int argc, char *argv[]){
    if (argc == 1){ // not enough arguments given in command line
        error("Configuration file needed");
    }
    curl_global_init(CURL_GLOBAL_ALL);
    // parsing config file
    string stream; // line from file
    string token; // broken down string
    string t[2]; //array of words
    ifstream infile;
    infile.open(argv[1]);
    if (!infile.is_open()){
        error("Config file not opened");
    }
    while (infile >> stream){
        istringstream ss(stream);
        int i = 0;
        while(getline(ss, token, '=')){
            t[i] = token;
            i++;
        }
        if (t[0] == "PERIOD_FETCH"){
            int re;
            stringstream(t[1]) >> re; // converts str to int
            PERIOD_FETCH = re;
        }
        else if (t[0] == "NUM_FETCH"){
            int re;
            stringstream(t[1]) >> re; // str to int
            NUM_FETCH = re;
            if (NUM_FETCH < 1 || NUM_FETCH > 8){
                cout << "ERROR: NUM_FETCH out of range!" << endl;
                exit(1);
            }

        }
        else if (t[0] == "NUM_PARSE"){
            int re;
            stringstream(t[1]) >> re; // str to int
            NUM_PARSE = re;
            if (NUM_PARSE < 1 || NUM_PARSE > 8){
                error("NUM_PARSE out of range!");
            }   
        }
        else if (t[0] == "SEARCH_FILE"){
            SEARCH_FILE = t[1];
        } 
        else if (t[0] == "SITE_FILE"){
            SITE_FILE = t[1];
        }
        else{
            cout << "WARNING: Unknown parameter " << t[0] << endl;
        }

    }   
    infile.close(); // closes config file
    if (SEARCH_FILE == "" || SITE_FILE == ""){
        error("No Search or Site files found");
    }
    char const* cstr = SEARCH_FILE.c_str();
    searches = get(cstr);
    char const* sstr = SITE_FILE.c_str();
    sites = get(sstr);
    
    pthread_t* fThreads = (pthread_t*)malloc(NUM_FETCH * sizeof(pthread_t));
    pthread_t* pThreads = (pthread_t*)malloc(NUM_PARSE * sizeof(pthread_t));
    signal(SIGALRM, handler);
    alarm(PERIOD_FETCH);

    fs.open("1.csv");
    for (int t = 0; t < NUM_FETCH; t++){
        pthread_create(&fThreads[t], NULL, fetch, NULL);
    }
    while (curlfetch.empty()) {
        pthread_mutex_lock(&(lock1));
        pthread_cond_wait(&parseEmpty, &lock1);
        pthread_mutex_unlock(&(lock1));
    }

    for (int t = 0; t < NUM_PARSE; t++) {
        pthread_create(&pThreads[t], NULL, parse, NULL);
    }

    signal(SIGINT,h);
    signal(SIGHUP,h);
    while (keeplooking) {
        pause();     
    }

    curl_global_cleanup();
    return 0;
}
