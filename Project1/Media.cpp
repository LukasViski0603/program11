#include "Media.h"

// Definitions for the Media base class
Media::Media() : type(' '), length(0), yearReleased(0), rating(0) {}

Media::Media(char type, const string& title, const string& keyName, int rating,
    const string& genre, int length, int yearReleased)
    : type(type), title(title), keyName(keyName), rating(rating),
    genre(genre), length(length), yearReleased(yearReleased) {}

Media::~Media() {}

char Media::getType() const { return type; }
string Media::getTitle() const { return title; }
string Media::getKeyName() const { return keyName; }
int Media::getRating() const { return rating; }
string Media::getGenre() const { return genre; }
int Media::getLength() const { return length; }
int Media::getYearReleased() const { return yearReleased; }

void Media::setType(char type) { this->type = type; }
void Media::setTitle(const string& title) { this->title = title; }
void Media::setKeyName(const string& keyName) { this->keyName = keyName; }
void Media::setRating(int rating) { this->rating = rating; }
void Media::setGenre(const string& genre) { this->genre = genre; }
void Media::setLength(int length) { this->length = length; }
void Media::setYearReleased(int yearReleased) { this->yearReleased = yearReleased; }
void Media::adjustRating(int adjustment) {
    int newRating = rating + adjustment;
    rating = max(0, min(newRating, 10));  // to put ratings between 0 and 10 BUT IT'S NOT WORKING!!!!!!!!! 
}


string Movie::getStarsFormatted() const {
    string formattedStars;
    for (size_t i = 0; i < stars.size(); ++i) {
        formattedStars += stars[i];
        if (i < stars.size() - 1) { 
            formattedStars += ", ";
        }
    }
    return formattedStars;
}

