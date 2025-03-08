#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <cstring>
#include <algorithm>

using namespace std;

// Abstract base class (Media)
class Media
{
public:
    virtual void play() = 0;
    virtual void displayInfo() = 0;
    virtual void saveToFile(FILE *file) = 0;
    virtual string getType() = 0;

    virtual ~Media()
    {
        cout << "Media base class destructor called." << endl;
    }
};

// Movie class (derived from Media)
class Movie : public Media
{
private:
    string title;
    string director;
    string genre;
    int duration;
    float rating;

public:
    // Simplified constructor
    Movie(string t, string d, string g, int dur, float r)
    {
        title = t;
        director = d;
        genre = g;
        duration = dur;
        rating = r;
    }

    // Getter for title
    string getTitle() const
    {
        return title;
    }

    void play() override
    {
        // Only print the movie's details once in the correct format
        cout << "\n=========================================" << endl;
        cout << "🎬 Now Playing: " << title << endl;
        cout << "-----------------------------------------" << endl;
        cout << "Director   : " << director << endl;
        cout << "Genre      : " << genre << endl;
        cout << "Duration   : " << duration << " minutes" << endl;
        cout << "IMDB Rating: " << rating << endl;
        cout << "=========================================" << endl;
    }

    void displayInfo() override
    {
        // Display movie details in a clean format
        cout << "\n=========================================" << endl;
        cout << "Movie: " << title << endl;
        cout << "-----------------------------------------" << endl;
        cout << "Director   : " << director << endl;
        cout << "Genre      : " << genre << endl;
        cout << "Duration   : " << duration << " minutes" << endl;
        cout << "IMDB Rating: " << rating << endl;
        cout << "=========================================" << endl;
    }

    string getType() override { return "Movie"; }

    float getRating() const { return rating; }

    void saveToFile(FILE *file) override
    {
        fprintf(file, "Movie:\n");
        fprintf(file, "  Title: %s\n", title.c_str());
        fprintf(file, "  Director: %s\n", director.c_str());
        fprintf(file, "  Genre: %s\n", genre.c_str());
        fprintf(file, "  Duration: %d\n", duration);
        fprintf(file, "  IMDB Rating: %.1f\n", rating);
        fprintf(file, "\n");
    }

    static Movie *loadFromFile(FILE *file)
    {
        char titleBuffer[100], directorBuffer[100], genreBuffer[100];
        int duration;
        float rating;

        fscanf(file, "Movie:\n");
        fgets(titleBuffer, sizeof(titleBuffer), file);
        fgets(directorBuffer, sizeof(directorBuffer), file);
        fgets(genreBuffer, sizeof(genreBuffer), file);
        fscanf(file, "  Duration: %d\n", &duration);
        fscanf(file, "  IMDB Rating: %f\n", &rating);

        titleBuffer[strcspn(titleBuffer, "\n")] = 0;
        directorBuffer[strcspn(directorBuffer, "\n")] = 0;
        genreBuffer[strcspn(genreBuffer, "\n")] = 0;

        return new Movie(titleBuffer, directorBuffer, genreBuffer, duration, rating);
    }
};

// MovieCollection class (derived from Movie)
class MovieCollection : public Movie
{
private:
    string collectionName;
    string description;
    vector<Movie *> items;

public:
    // Constructor (remains unchanged)
    MovieCollection(string n, string d)
        : Movie("", "", "", 0, 0.0f)
    {
        collectionName = n;
        description = d;
    }

    // Method to get the list of movies
    vector<Movie *> &getMovies()
    {
        return items; // Return the vector of movies in the collection
    }

    void add(Movie *movie)
    {
        items.push_back(movie);
    }

    void listMedia() const
    {
        if (items.empty())
        {
            cout << "No media in this collection." << endl;
        }
        else
        {
            cout << "\n=========================================" << endl;
            cout << "Movies in the collection: " << collectionName << endl;
            cout << "-----------------------------------------" << endl;
            for (const auto &movie : items)
            {
                movie->displayInfo();
            }
            cout << "=========================================" << endl;
        }
    }

    string getCollectionName() const
    {
        return collectionName;
    }

    void sortMovies()
    {
        sort(items.begin(), items.end(), [](Movie *a, Movie *b)
             { return a->getRating() > b->getRating(); });
        cout << "Movies in the collection sorted by IMDB rating." << endl;
    }

    void saveToFile(FILE *file)
    {
        fprintf(file, "Collection: %s\n", collectionName.c_str());
        fprintf(file, "Description: %s\n", description.c_str());
        fprintf(file, "-----------------------------\n");

        for (auto &movie : items)
        {
            movie->saveToFile(file);
        }
    }

    static MovieCollection *loadFromFile(FILE *file)
    {
        char nameBuffer[100], descBuffer[200];
        fgets(nameBuffer, sizeof(nameBuffer), file);
        fgets(descBuffer, sizeof(descBuffer), file);

        nameBuffer[strcspn(nameBuffer, "\n")] = 0;
        descBuffer[strcspn(descBuffer, "\n")] = 0;

        MovieCollection *collection = new MovieCollection(nameBuffer, descBuffer);

        while (!feof(file))
        {
            char typeBuffer[10];
            fscanf(file, "%9s\n", typeBuffer);

            Movie *movie = nullptr;

            if (strcmp(typeBuffer, "Movie:") == 0)
            {
                movie = Movie::loadFromFile(file);
            }

            if (movie)
            {
                collection->add(movie);
            }
        }

        return collection;
    }

    ~MovieCollection()
    {
        for (auto movie : items)
        {
            delete movie;
        }
        cout << "Collection '" << collectionName << "' destroyed." << endl;
    }
};

void displayMenu()
{
    cout << "\n=== Movie Collection Manager ===" << endl;
    cout << "1. View All Movie Collections" << endl;
    cout << "2. Create Movie Collection" << endl;
    cout << "3. Add Movie to Collection" << endl;
    cout << "4. List Media in Collection" << endl;
    cout << "5. Sort Movies by IMDB Rating" << endl;
    cout << "6. Save Collections to File" << endl;
    cout << "7. Load Collections from File" << endl;
    cout << "8. Play Movie" << endl; // New option to play a movie
    cout << "9. Exit" << endl;
}

int main()
{
    vector<MovieCollection *> collections;
    bool isRunning = true;

    while (isRunning)
    {
        displayMenu();
        int choice;
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore();

        if (choice == 1)
        {
            cout << "\nAvailable Movie Collections:" << endl;
            for (const auto &collection : collections)
            {
                cout << "- " << collection->getCollectionName() << endl;
            }
        }
        else if (choice == 2)
        {
            string name, description;
            cout << "Enter collection name: ";
            getline(cin, name);
            cout << "Enter description: ";
            getline(cin, description);
            collections.push_back(new MovieCollection(name, description));
            cout << "Movie Collection created!" << endl;
        }
        else if (choice == 3)
        {
            string collectionName;
            cout << "Enter collection name: ";
            getline(cin, collectionName);

            bool found = false;
            for (auto &collection : collections)
            {
                if (collection->getCollectionName() == collectionName)
                {
                    found = true;
                    string title, director, genre;
                    int duration;
                    float rating;

                    cout << "Enter movie title: ";
                    getline(cin, title);
                    cout << "Enter movie director: ";
                    getline(cin, director);
                    cout << "Enter movie genre: ";
                    getline(cin, genre);
                    cout << "Enter movie duration: ";
                    cin >> duration;
                    cout << "Enter movie IMDB rating: ";
                    cin >> rating;
                    cin.ignore();

                    Movie *newMovie = new Movie(title, director, genre, duration, rating);
                    collection->add(newMovie);

                    // Optionally play the movie right after adding it
                    newMovie->play();

                    break;
                }
            }

            if (!found)
            {
                cout << "Collection not found." << endl;
            }
        }

        else if (choice == 4)
        {
            string collectionName;
            cout << "Enter collection name to list media: ";
            getline(cin, collectionName);

            bool found = false;
            for (auto &collection : collections)
            {
                if (collection->getCollectionName() == collectionName)
                {
                    found = true;
                    collection->listMedia();
                    break;
                }
            }

            if (!found)
            {
                cout << "Collection not found." << endl;
            }
        }
        else if (choice == 5)
        {
            string collectionName;
            cout << "Enter collection name to sort movies by IMDB rating: ";
            getline(cin, collectionName);

            bool found = false;
            for (auto &collection : collections)
            {
                if (collection->getCollectionName() == collectionName)
                {
                    found = true;
                    collection->sortMovies();
                    collection->listMedia();
                    break;
                }
            }

            if (!found)
            {
                cout << "Collection not found." << endl;
            }
        }
        else if (choice == 6)
        {
            FILE *file = fopen("movie_collections.txt", "w");
            if (file)
            {
                for (auto &collection : collections)
                {
                    collection->saveToFile(file);
                }
                fclose(file);
                cout << "Movie Collections saved to file." << endl;
            }
            else
            {
                cout << "Error saving movie collections." << endl;
            }
        }
        else if (choice == 7)
        {
            FILE *file = fopen("movie_collections.txt", "r");
            if (file)
            {
                while (!feof(file))
                {
                    MovieCollection *collection = MovieCollection::loadFromFile(file);
                    if (collection)
                    {
                        collections.push_back(collection);
                    }
                }
                fclose(file);
                cout << "Movie Collections loaded from file." << endl;
            }
            else
            {
                cout << "Error loading movie collections." << endl;
            }
        }
        else if (choice == 8)
        {
            string collectionName;
            cout << "Enter collection name to play a movie: ";
            getline(cin, collectionName);

            bool found = false;
            for (auto &collection : collections)
            {
                if (collection->getCollectionName() == collectionName)
                {
                    found = true;
                    string movieTitle;
                    cout << "Enter movie title to play: ";
                    getline(cin, movieTitle);

                    bool movieFound = false;
                    for (auto &movie : collection->getMovies())
                    {
                        if (movie->getTitle() == movieTitle)
                        {
                            movieFound = true;
                            movie->play(); // Play the movie
                            break;
                        }
                    }

                    if (!movieFound)
                    {
                        cout << "Movie not found in the collection." << endl;
                    }
                    break;
                }
            }

            if (!found)
            {
                cout << "Collection not found." << endl;
            }
        }

        else if (choice == 9)
        {
            cout << "Exiting..." << endl;
            isRunning = false;
        }
        else
        {
            cout << "Invalid choice. Please try again." << endl;
        }
    }

    // Clean up dynamically allocated memory
    for (auto &collection : collections)
    {
        delete collection;
    }

    return 0;
}
