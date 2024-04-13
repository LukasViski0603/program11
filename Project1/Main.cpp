// Lukas Viskovic 
// CS201 - Program 11 (Media Library)
// 3/24/24 

#include "Media.h"
#include "Functions.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip> 
#include <memory>
#include <numeric>
using namespace std;


int main() {
    vector<Media*> mediaList;
    ofstream errorLog("errorLog.txt");
    //ofstream reportFile("mediaReport.txt");

    readMediaList("Data/mediaList.txt", mediaList, errorLog);            // Read media items from file
    processCommands("Data/mediaCommands.txt", mediaList, errorLog);      // Process commands from file
    generateReport(mediaList, "mediaReport.txt");                        // Generate a report of all media in the list
    //listMovieStars(mediaList, reportFile);

    errorLog.close();
    return 0;
}
