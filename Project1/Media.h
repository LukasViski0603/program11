#pragma once
#include <string>
#include <vector>
using namespace std;

class Media {
protected:
    char type;                        // 'M' for Movie, 'B' for Book, 'S' for Song
    string title, keyName, genre; 
    int length, yearReleased, rating; // Rating is 1 to 10.
                                      // Length in minutes for media; in pages for books
public:
    Media();
    Media(char type, const string& title, const string& keyName, int rating, const string& genre, int length, int yearReleased);
    virtual ~Media();

    virtual bool isType(char t) const { return false; }  // Declare this method to be overridden
    virtual bool isMovie() const { return false; }

    char getType() const;
    string getTitle() const;
    string getKeyName() const;
    int getRating() const;
    string getGenre() const;
    int getLength() const;
    int getYearReleased() const;

    void setType(char type);
    void setTitle(const string& title);
    void setKeyName(const string& keyName);
    void setRating(int rating);
    void setGenre(const string& genre);
    void setLength(int length);
    void setYearReleased(int yearReleased);
    void adjustRating(int adjustment);

};

class Movie : public Media {
private:
    vector<string> stars; // List of stars in the movie

public:
    // Default constructor
    Movie() : Media() {}

    // Parameterized constructor
    Movie(char type, const string& title, const string& keyName, int rating,
        const string& genre, int length, int yearReleased, const vector<string>& stars)
        : Media(type, title, keyName, rating, genre, length, yearReleased), stars(stars) {}

    // Override the base class function for type identification
    bool isType(char t) const override {
        return t == 'M';
    }
    bool isMovie() const override { return true; }

    // Getter for stars
    vector<string> getStars() const {
        return stars; 
    }

    // Setter for stars
    void setStars(const vector<string>& stars) {
        this->stars = stars;
    }
    string getStarsFormatted() const;
};


class Book : public Media {
private:

    int weeksNYT; // Number of weeks on the NYT bestseller list

public:
    // Default constructor
    Book() : Media(), weeksNYT(0) {}

    // Parameterized constructor
    Book(char type, const string& title, const string& keyName, int rating,
        const string& genre, int length, int yearReleased, int weeksNYT)
        : Media(type, title, keyName, rating, genre, length, yearReleased), weeksNYT(weeksNYT) {}

    // Override the base class function for type identification
    bool isType(char t) const override {
        return t == 'B';
    }

    // Getter for weeksNYT
    int getWeeksNYT() const {
        return weeksNYT;
    }

    // Setter for weeksNYT
    void setWeeksNYT(int weeksNYT) {
        this->weeksNYT = weeksNYT;
    }
};

class Song : public Media {
private:
    bool top40; // Indicator if the song was ever in the top 40

public:
    // Default constructor
    Song() : Media(), top40(false) {}

    // Parameterized constructor
    Song(char type, const string& title, const string& keyName, int rating,
        const string& genre, int length, int yearReleased, bool top40)
        : Media(type, title, keyName, rating, genre, length, yearReleased), top40(top40) {}

    // Override the base class function for type identification
    bool isType(char t) const override {
        return t == 'S';
    }

    // Getter for top40
    bool getTop40() const {
        return top40;
    }

    // Setter for top40
    void setTop40(bool top40) {
        this->top40 = top40;
    }
};

