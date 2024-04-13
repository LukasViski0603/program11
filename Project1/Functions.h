#pragma once
#include "Media.h"
#include <vector>
#include <fstream>
#include <iostream>

void readMediaList(const string& filename, vector<Media*>& mediaList, ofstream& errorLog);
void generateReport(const vector<Media*>& mediaList, const string& reportFilename);
void logError(ofstream& errorLog, const string& message);
void processCommands(const string& filename, vector<Media*>& mediaList, ofstream& errorLog);
void addNewMedia(const string& type, const string& title, const string& keyName, int rating,
    const string& genre, int length, int yearReleased, const vector<string>& stars,
    vector<Media*>& mediaList, ofstream& errorLog);
void listByKeyname(const vector<Media*>& mediaList, const string& keyName, ofstream& reportFile);
void adjustRatings(vector<Media*>& mediaList, char mediaType, int adjustment, ofstream& errorLog);
void listMovieStars(const vector<Media*>& mediaList, ostream& out);
string trim(const string& str);
void printFilteredMovies(const vector<Media*>& mediaList, ofstream& reportFile, int minRating);
void processNewMediaEntry(stringstream& ss, vector<Media*>& mediaList, ofstream& errorLog);
void printAllMediaByRating(const vector<Media*>& mediaList, ofstream& reportFile, int exactRating);
void printFilteredSongsByRating(const vector<Media*>& mediaList, ofstream& reportFile, int minRating);
void printFilteredBooksByRating(const vector<Media*>& mediaList, ofstream& reportFile, int minRating);
void printMediaSummary(const vector<Media*>& mediaList, ofstream& reportFile);
