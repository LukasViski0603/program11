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





void listMovieStars(const vector<Media*>& mediaList, ostream& out) {
    out << "\nList of Movie Stars:\n";
    out << "----------------------------------------\n";
    for (const Media* media : mediaList) {
        const Movie* movie = dynamic_cast<const Movie*>(media);
        if (movie) { // Check if the cast is successful
            out << movie->getTitle() << " - ";
            for (const auto& star : movie->getStars()) {
                out << star << ", ";
            }
            out << endl;
        }
    }
    out << "----------------------------------------\n";
}


void listByKeyname(const vector<Media*>& mediaList, const string& keyName, ofstream& reportFile) {
    for (const auto& media : mediaList) {
        if (media->getKeyName() == keyName) {
            // OUTPUT FORMAT EDITING HERE!!! 
            reportFile << left << setw(40) << media->getTitle() << setw(8) << media->getYearReleased() << media->getRating() << "\n";
        }
    }
}


void readMediaList(const string& filename, vector<Media*>& mediaList, ofstream& errorLog) {
    ifstream file(filename);
    if (!file) {
        logError(errorLog, "Failed to open file: " + filename);
        return;
    }

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        char type = ss.get(); // Read the type directly
        ss.ignore(numeric_limits<streamsize>::max(), ','); // Ignore until comma

        string title, keyName, genre, token;
        int length, yearReleased, rating;
        vector<string> stars;
        int weeksNYT;
        bool top40 = false;

        getline(ss, title, ',');
        getline(ss, keyName, ',');
        ss >> rating;
        ss.ignore(1, ',');
        getline(ss, genre, ',');
        ss >> length;
        ss.ignore(1, ',');
        ss >> yearReleased;
        ss.ignore(1, ',');

        try {
            switch (type) {
            case 'M': {
                string star;
                while (getline(ss, star, ',')) {
                    stars.push_back(trim(star));
                }
                mediaList.push_back(new Movie(type, title, keyName, rating, genre, length, yearReleased, stars));
                break;
            }
            case 'B': {
                string weeksInput;
                getline(ss, weeksInput, ','); // Get weeksNYT as string from input
                try {
                    weeksNYT = stoi(weeksInput); // Convert weeksNYT to integer
                    mediaList.push_back(new Book(type, title, keyName, rating, genre, length, yearReleased, weeksNYT));
                }
                catch (const invalid_argument&) {
                    logError(errorLog, "ERROR: Invalid weeksNYT value for Book '" + title + "': " + weeksInput);
                }
                catch (const out_of_range&) {
                    logError(errorLog, "ERROR: Out of range weeksNYT value for Book '" + title + "': " + weeksInput);
                }
                break;
            }
            case 'S': {
                ss >> token;
                if (token == "Y" || token == "1") top40 = true;
                else if (token == "N" || token == "0") top40 = false;
                else throw invalid_argument("Invalid boolean value for Top40");
                mediaList.push_back(new Song(type, title, keyName, rating, genre, length, yearReleased, top40));
                break;
            }
            default:
                throw invalid_argument("Unsupported media type encountered: " + string(1, type));
            }
        }
        catch (const invalid_argument& e) {
            logError(errorLog, e.what());
        }
    }
    file.close();
}




// Reads commandds from file and executes proper actions 
void processCommands(const string& filename, vector<Media*>& mediaList, ofstream& errorLog) {
    ifstream file(filename);
    if (!file) {
        logError(errorLog, "Failed to open file: " + filename);
        return;
    }

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        char commandType = ss.get();  // Get the command type
        ss.ignore();  // Ignore the comma

        try {
            switch (commandType) {
            case 'N': {  // Add new media entry
                processNewMediaEntry(ss, mediaList, errorLog);
                break;
            }
            case 'A': {  // Adjust ratings
                char mediaType;
                int adjustment;
                ss >> mediaType >> adjustment;
                adjustRatings(mediaList, mediaType, adjustment, errorLog);
                break;
            }
            default:
                logError(errorLog, "Unrecognized command: " + string(1, commandType));
                break;
            }
        }
        catch (const std::exception& e) {
            logError(errorLog, "Error processing command: " + string(e.what()));
        }
    }
    file.close();
}

void processNewMediaEntry(stringstream& ss, vector<Media*>& mediaList, ofstream& errorLog) {
    char type = ss.get(); ss.ignore();
    string title, keyName, genre, token;
    int rating, length, yearReleased;
    vector<string> stars;

    getline(ss, title, ',');
    getline(ss, keyName, ',');
    ss >> rating; ss.ignore();
    getline(ss, genre, ',');
    ss >> length; ss.ignore();
    ss >> yearReleased; ss.ignore();

    if (!(ss >> yearReleased)) {  // Validate yearReleased parsing
        logError(errorLog, "Failed to parse yearReleased for " + title);
        yearReleased = 0;  // Default to 0 or an indicative value
    }
    ss.ignore();

    // Check and log invalid rating, do not add to media list
    if (rating < 0 || rating > 10) {
        logError(errorLog, "Invalid rating value for " + title + ": " + to_string(rating));
        return;  // Skip adding this media to the list
    }

    if (type == 'M') {  // Movie specific parsing
        string star;
        while (getline(ss, star, ',')) {
            stars.push_back(trim(star));
        }
        mediaList.push_back(new Movie(type, title, keyName, rating, genre, length, yearReleased, stars));
    }
    else if (type == 'B') {  // Book specific parsing
        int weeksNYT;
        ss >> weeksNYT;
        mediaList.push_back(new Book(type, title, keyName, rating, genre, length, yearReleased, weeksNYT));
    }
    else if (type == 'S') {  // Song specific parsing
        bool top40 = (ss >> std::ws, ss.peek() == 'Y' || ss.peek() == '1');
        mediaList.push_back(new Song(type, title, keyName, rating, genre, length, yearReleased, top40));
    }
    else {
        logError(errorLog, "Unsupported media type encountered: " + string(1, type));
    }
}




void adjustRatings(vector<Media*>& mediaList, char mediaType, int adjustment, ofstream& errorLog) {
    for (auto& media : mediaList) {
        if (media->getType() == mediaType) {
            int newRating = media->getRating() + adjustment;
            if (newRating > 10 || newRating < 0) {
                logError(errorLog, "Adjusted rating out of bounds for " + media->getTitle() + ": " + to_string(newRating));
                newRating = max(0, min(newRating, 10));  // Ensure rating is clamped between 0 and 10
            }
            media->setRating(newRating);
        }
    }
}

void logError(ofstream& errorLog, const string& message) {
    if (errorLog.is_open()) {
        errorLog << message << endl;
    }
    else {
        cerr << "Error logging failed: " + message << endl;
    }
}

string trim(const string& str) {
    size_t first = str.find_first_not_of(' ');
    size_t last = str.find_last_not_of(' ');
    return (first == string::npos || last == string::npos) ? "" : str.substr(first, (last - first + 1));
}




// Parses details from stringstream and adds new Media object 
void addNewMedia(const string& type, const string& title, const string& keyName, int rating,
    const string& genre, int length, int yearReleased, const vector<string>& stars,
    vector<Media*>& mediaList, ofstream& errorLog) {
    if (type == "M") {
        mediaList.push_back(new Movie(type[0], title, keyName, rating, genre, length, yearReleased, stars)); // Change type from string to char
    }
    else if (type == "B") {
        int weeksNYT = 0; // Placeholder, adjust as needed
        mediaList.push_back(new Book(type[0], title, keyName, rating, genre, length, yearReleased, weeksNYT)); // Change type from string to char
    }
    else if (type == "S") {
        bool top40 = false; // Placeholder, adjust based on your input parsing
        mediaList.push_back(new Song(type[0], title, keyName, rating, genre, length, yearReleased, top40)); // Change type from string to char
    }
    else {
        errorLog << "Unsupported media type for new entry: " << type << "\n";
    }
}




string top40ToString(bool top40) {
    return top40 ? "YES" : "NO";
}

// generate a report of all media items
void generateReport(const vector<Media*>& mediaList, const string& reportFilename) {
    ofstream reportFile(reportFilename);
    if (!reportFile) {
        cerr << "Failed to open report file: " << reportFilename << endl;
        return;
    }


    // Movies Section
    reportFile << "++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
    reportFile << "YOUR MOVIE LIST\n\n";
    reportFile << left << setw(5) << "#" << setw(40) << "TITLE" << setw(8) << "YEAR" << setw(8) << "RATING" << "STARS\n";
    int movieCounter = 1;
    for (const Media* media : mediaList) {
        const Movie* movie = dynamic_cast<const Movie*>(media);
        if (movie) {
            reportFile << left << setw(5) << movieCounter++ << setw(40) << movie->getTitle()
                << setw(8) << movie->getYearReleased() << setw(8) << movie->getRating()
                << movie->getStarsFormatted() << "\n";
        }
    }

    // Books Section
    reportFile << "\n++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
    reportFile << "YOUR BOOK LIST\n\n";
    reportFile << left << setw(5) << "#" << setw(40) << "TITLE" << setw(8) << "YEAR" << setw(8) << "RATING" << "WKS ON NYT\n";
    int bookCounter = 1;
    for (const Media* media : mediaList) {
        const Book* book = dynamic_cast<const Book*>(media);
        if (book) {
            reportFile << left << setw(5) << bookCounter++ << setw(40) << book->getTitle()
                << setw(8) << book->getYearReleased() << setw(8) << book->getRating()
                << book->getWeeksNYT() << "\n";
        }
    }

    // Songs Section
    reportFile << "\n++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
    reportFile << "YOUR SONG LIST\n\n";
    reportFile << left << setw(5) << "#" << setw(40) << "TITLE" << setw(8) << "YEAR" << setw(8) << "RATING" << "TOP 40\n";
    int songCounter = 1;
    for (const Media* media : mediaList) {
        const Song* song = dynamic_cast<const Song*>(media);
        if (song) {
            reportFile << left << setw(5) << songCounter++ << setw(40) << song->getTitle()
                << setw(8) << song->getYearReleased() << setw(8) << song->getRating()
                << (song->getTop40() ? "YES" : "NO") << "\n";
        }
    }

    reportFile << "\n++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
    reportFile << "YOUR MOVIE LIST FOR RATING >= 9\n\n";
    reportFile << left << setw(12) << "#" << setw(40) << "TITLE" << setw(8) << "YEAR" << setw(8) << "RATING" << "STARS\n";
    printFilteredMovies(mediaList, reportFile, 9);
    reportFile << "\n";

    reportFile << "\n++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
    printAllMediaByRating(mediaList, reportFile, 10);

    reportFile << "\n++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
    printFilteredSongsByRating(mediaList, reportFile, 9);

    reportFile << "\n++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
    printFilteredBooksByRating(mediaList, reportFile, 7);

    // Footer
    reportFile << "++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
    reportFile << "YOUR MEDIA LIBRARY\n";
    reportFile << left << setw(5) << movieCounter - 1 << "Movies\n";
    reportFile << left << setw(5) << bookCounter - 1 << "Books\n";
    reportFile << left << setw(5) << songCounter - 1 << "Songs\n";
    reportFile << left << setw(5) << movieCounter + bookCounter + songCounter - 3 << "Items\n";
    reportFile << "++++++++++++++++++++++++++++++++++++++++++++++++++++\n";

    printMediaSummary(mediaList, reportFile);


    reportFile.close();
}

// self explanatory 
void printAllMediaByRating(const vector<Media*>& mediaList, ofstream& reportFile, int exactRating) {
    reportFile << "YOUR MEDIA LIST FOR RATING >= " << exactRating << "\n";
    reportFile << left << setw(12) << "#" << setw(40) << "TITLE" << setw(8) << "YEAR" << setw(8) << "RATING" << "TYPE\n";
    int count = 1;
    for (const auto& media : mediaList) {
        if (media->getRating() >= exactRating) {
            string typeStr = (media->getType() == 'M' ? "Movie" :
                media->getType() == 'B' ? "Book" : "Song");
            reportFile << left << setw(12) << count++ << setw(40) << media->getTitle()
                << setw(8) << media->getYearReleased() << setw(8) << media->getRating()
                << typeStr << "\n";
        }
    }
}

void printFilteredSongsByRating(const vector<Media*>& mediaList, ofstream& reportFile, int minRating) {
    reportFile << "\nYOUR SONG LIST FOR RATING >= " << minRating << "\n";
    reportFile << left << setw(5) << "#" << setw(40) << "TITLE" << setw(8) << "YEAR" << setw(8) << "RATING" << "TOP 40\n";
    int songCounter = 1;
    for (const auto& media : mediaList) {
        const Song* song = dynamic_cast<const Song*>(media);
        if (song && song->getRating() >= minRating) {
            reportFile << left << setw(5) << songCounter++ << setw(40) << song->getTitle()
                << setw(8) << song->getYearReleased() << setw(8) << song->getRating()
                << (song->getTop40() ? "YES" : "NO") << "\n";
        }
    }
}


// Helper function to filter and print movies with a rating >= 9
void printFilteredMovies(const vector<Media*>& mediaList, ofstream& reportFile, int minRating) {
    int count = 1;
    for (const Media* media : mediaList) {
        const Movie* movie = dynamic_cast<const Movie*>(media);
        if (movie && movie->getRating() >= minRating) {
            reportFile << left << setw(12) << count++ << setw(40) << movie->getTitle()
                << setw(8) << movie->getYearReleased() << setw(8) << movie->getRating()
                << movie->getStarsFormatted() << "\n";
        }
    }
}

void printFilteredBooksByRating(const vector<Media*>& mediaList, ofstream& reportFile, int minRating) {
    reportFile << "\nYOUR BOOK LIST FOR RATING >= " << minRating << "\n";
    reportFile << left << setw(5) << "#" << setw(40) << "TITLE" << setw(8) << "YEAR" << setw(8) << "RATING" << "WKS ON NYT\n";
    int bookCounter = 1;
    for (const auto& media : mediaList) {
        const Book* book = dynamic_cast<const Book*>(media);
        if (book && book->getRating() >= minRating) {
            reportFile << left << setw(5) << bookCounter++ << setw(40) << book->getTitle()
                << setw(8) << book->getYearReleased() << setw(8) << book->getRating()
                << book->getWeeksNYT() << "\n";
        }
    }
    reportFile << "++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
}

void printMediaSummary(const vector<Media*>& mediaList, ofstream& reportFile) {
    int movieCount = 0, bookCount = 0, songCount = 0;

    // Count each type of media
    for (const auto& media : mediaList) {
        if (dynamic_cast<const Movie*>(media)) movieCount++;
        else if (dynamic_cast<const Book*>(media)) bookCount++;
        else if (dynamic_cast<const Song*>(media)) songCount++;
    }

    reportFile << "\nYOUR MEDIA LIST\n";
    reportFile << "++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
    reportFile << left << setw(15) << "Media Type" << "Count\n";
    reportFile << left << setw(15) << "Movies" << movieCount << "\n";
    reportFile << left << setw(15) << "Books" << bookCount << "\n";
    reportFile << left << setw(15) << "Songs" << songCount << "\n";
    reportFile << "++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
}
