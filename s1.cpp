// Esmeralda Cervantes
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>

using namespace std;

void error(string message){
    cout << "ERROR: " << message << endl;
    exit(1);
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
    //    cout << info << endl;
    }
    ifs.close();
    return vstrings;
}

// strunct for libcurl
struct MemoryStruct {
   char *memory;
    size_t size;
};

struct MemoryStruct chunk;

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp){
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
    mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
    if(mem->memory == NULL) {
        /* out of memory! */ 
        cout << "not enough memory (realloc returned NULL)" << endl;
        return 0;
    }
 
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
 
  return realsize;
}

void curler (string site){
    CURL *curl_handle;
    CURLcode res;

    chunk.memory = (char *)malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */

    curl_global_init(CURL_GLOBAL_ALL);

    /* init the curl session */
    curl_handle = curl_easy_init();

    /* specify URL to get */
//    curl_easy_setopt(curl_handle, CURLOPT_URL, "http://www.example.com/");
    curl_easy_setopt(curl_handle, CURLOPT_URL, site.c_str());

    /* send all data to this function  */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

    /* we pass our 'chunk' struct to the callback function */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

    /* some servers don't like requests that are made without a user-agent
     field, so we provide one */
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    /* get it! */
    res = curl_easy_perform(curl_handle);

    /* check for errors */
    if(res != CURLE_OK) {
        // fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        cout << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;

  }
  else {
    /*
     * Now, our chunk.memory points to a memory block that is chunk.size
     * bytes big and contains the remote file.
     *
     * Do something nice with it!
     */

    cout << chunk.size << " bytes retrieved" << endl;
  }

}

int main (int argc, char *argv[]){
    if (argc == 1){ // not enough arguments given in command line
        cout << "ERROR: Configuration file needed" << endl;
        exit(1);
    }
    // parsing config file
    int PERIOD_FETCH = 180;
    int NUM_FETCH = 1;
    int NUM_PARSE = 1;
    string SEARCH_FILE = "";
    string SITE_FILE = "";
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
                cout << "ERROR: NUM_PARSE out of range!" << endl;
                exit(1);
            }   
        }
        else if (t[0] == "SEARCH_FILE"){
            SEARCH_FILE = t[1];
        } 
        else if (t[0] == "SITE_FILE"){
            SITE_FILE = t[1];
        }
        else{
            cout << "WARNING: Unkown parameter " << t[0] << endl;
        }

    }   
    infile.close(); // closes config file
    if (SEARCH_FILE == "" || SITE_FILE == ""){
        error("No Search or Site files found");
    }
    char const* cstr = SEARCH_FILE.c_str();
    vector<string> searches = get(cstr);
//    cout << "here" << endl;
    for (unsigned j = 0; j < searches.size(); j++){
        cout << searches[j] << endl;
    }
    char const* sstr = SITE_FILE.c_str();
    vector<string> sites = get(sstr);
    for (unsigned j = 0; j < sites.size(); j++){
        cout << sites[j] << endl;
    }

//    for (j = 0; j < sites.size(); j++){
        curler("https://www.nd.edu/");
        cout << chunk.memory << endl;
//        for (int i = 
//    }
// free after copy to str

//    cout << PERIOD_FETCH << endl;
//    cout << NUM_FETCH << endl;
//    cout << NUM_PARSE << endl;
//    cout << SEARCH_FILE << endl;
//    cout << SITE_FILE << endl;

    return 0;
}
