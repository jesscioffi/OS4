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

using namespace std;

struct MemoryStruct {
  char *memory;
  size_t size;
};

struct MemoryStruct chunk;

void error(string message){
    cout << "ERROR: " << message << endl;
    exit(1);
}

// function to return the time
string time(){
    string s = "";
    time_t t = time(0);
    struct tm *now = localtime(&t);
    cout.width(2);
    cout.fill('0');
    cout << (now->tm_mon + 1) << endl;
    s.append("-");
    cout << (now->tm_mday) << endl;
    s.append("-");
    cout << (now->tm_year - 100) << endl;
    s.append("-");
    cout << (now->tm_hour) << endl;
    s.append(":");
    cout << (now->tm_min) << endl;
    s.append(":");
    cout << (now->tm_sec) << endl;
 //   cout << s << endl;
    return s;
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

//MemoryStruct webFetcher(string website) {
  void webFetcher(string website) {
  CURL *curl_handle;
  CURLcode res;
//  struct MemoryStruct chunk;
  chunk.memory = (char *)malloc(1);
  chunk.size = 0;

  curl_global_init(CURL_GLOBAL_ALL);
  curl_handle = curl_easy_init();
  curl_easy_setopt(curl_handle, CURLOPT_URL, website.c_str());
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
  res = curl_easy_perform(curl_handle);

  if(res != CURLE_OK) {
//    fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    cout << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
    }
  else {
//    printf("%lu bytes retrieved\n", (long)chunk.size);
//    cout << chunk.size << " bytes retrived" << endl;
  }

  curl_easy_cleanup(curl_handle);
  //free(chunk.memory);
  curl_global_cleanup();

//  return chunk;
}  

int wordCount(string file, string phrase) {
   int counter = 0;
   size_t index = -1;
   do {
      index = file.find(phrase, index+1);
      if (index != string::npos) {
         counter++;
  //      cout << "counter is " << counter << " index is: " << index << endl;
      }
   } while (index != string::npos);
//    cout << "last counter is " << counter << endl;
   return counter;
}

vector<string> curlfetch;

int main (int argc, char *argv[]){
    if (argc == 1){ // not enough arguments given in command line
        error("Configuration file needed");
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
    vector<string> searches = get(cstr);
    // cout << "here" << endl;
    char const* sstr = SITE_FILE.c_str();
    vector<string> sites = get(sstr);
    for (size_t i = 0; i < sites.size(); i++){
        webFetcher(sites[i]);
        curlfetch.push_back(chunk.memory);
//        cout << chunk.memory << endl;
        free(chunk.memory);
  //       cout << webFetcher(sites[i]).memory << endl;
 //       free(webFetcher(sites[i]).memory);
    }

    for (size_t j = 0; j < sites.size(); j++){
        for (size_t i = 0; i < searches.size(); i++){
            cout << sites[j] << " : " << searches[i] << " : " << wordCount(curlfetch[j], searches[i]) << endl;
//            int o = wordCount(curlfetch[j], searches[i]);
        }
    }
    string s = time();
    cout << s << endl;
//    cout << PERIOD_FETCH << endl;
//    cout << NUM_FETCH << endl;
//    cout << NUM_PARSE << endl;
//    cout << SEARCH_FILE << endl;
//    cout << SITE_FILE << endl;

    return 0;
}
