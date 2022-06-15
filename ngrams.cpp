/* Karen Munyan and Candice Murray
 *
 * File: ngrams.cpp
 * --------------------
 * This program reads in an input file and uses it to
 * make a large data structure of word groups called
 * N-grams. It then randomly generates new text that
 * sounds like it came from the same author as the file.
 */

#include <cctype>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include "console.h"
#include "filelib.h"
#include "gwindow.h"
#include "simpio.h"
#include "map.h"
#include "vector.h"
#include "random.h"
using namespace std;

//Declare function prototypes
Vector<string> makeVectorOfWords();
int getNFromUser();
Map<Vector<string>, Vector<string> > makeNGramMap(Vector<string>& allWords, int n);
void addWrapAround(Map<Vector<string>, Vector<string> >& nGramMap, Vector<string>& allWords, int n);
void findRandomOutput(Map<Vector<string>, Vector<string> >& nGramMap, Vector<string>& allWords, int n);

/* Function: makeVectorOfWords
 * Usage: Vector<string> allWords = makeVectorOfWords();
 * -----------------------------------------------------
 * This method prompts the user for a filename, reads the file,
 * and makes a Vector of all of the words in the file. It then closes
 * the file and returns the Vector with the words from the file.
 */
Vector<string> makeVectorOfWords()
{
    //Prompt user for file name.
    ifstream input;
    promptUserForFile(input, "Input file name? ");

    Vector<string> allWords; //Make a vector of all the words in the file
    string word;
    while(input >> word)
    {
        allWords.add(word);
    }
    input.close(); //Close the file
    return allWords;
}

/* Function: getNFromUser()
 * Usage: int n = getNFromUser();
 * -------------------------------
 * This method prompts the user for the N-gram value, and keeps
 * re-prompting the user until they have entered an N-gram value
 * that is at least 2. It then returns the N-gram value.
 */
int getNFromUser()
{
    int n = getInteger("Value of N? ");
    while(n < 2) //Re-prompt the user if the integer is less than 2
    {
        cout << "N must be 2 or greater. " << endl;
        n = getInteger("Value of N? ");
    }
    return n;
}

/* Function: makeNGram
 * Usage: Map<Vector<string>, Vector<string> > nGramMap = makeNGramMap(allWords, n);
 * ---------------------------------------------------------------------------------
 * This method takes in a Vector with all of the words from the file and the N-gram value,
 * then builds a map to connect prefixes (collections of of N-1 words) to all
 * possible suffixes. It then returns this map of prefixes and suffixes.
 */
Map<Vector<string>, Vector<string> > makeNGramMap(Vector<string>& allWords, int n)
{
    Map<Vector<string>, Vector<string> > nGramMap;
    Vector<string> key;
    Vector<string> value;

    int startIndex = 0;
    int valueIndex = startIndex + (n - 1);
    while(valueIndex < allWords.size()) //Goes through vector of all words until
    {                                   //the last word has been added as a suffix.
        key.clear();
        value.clear();
        for(int i = 0; i < n - 1; i++) //Adds N - 1 words to the prefix collection.
        {
            key.add(allWords[startIndex + i]);
        }

        if(nGramMap.containsKey(key)) //Updates the suffix collection if the prefix has already been mapped.
        {
            nGramMap[key] += allWords[valueIndex];
        }
        else //Adds the prefix-suffix mapping if the prefix has not already been mapped.
        {
            value.add(allWords[valueIndex]);
            nGramMap[key] = value;
        }
        startIndex++;
        valueIndex++;
    }
    return nGramMap;
}

/* Function: addWrapAround
 * Usage: addWrapAround(nGramMap, allWords, n);
 * ---------------------------------------------
 * This method passes in the nGramMap by reference, the vector of all of the words in the file,
 * and the N-Gram value. It then performs N - 1 more iterations of mapping, so that the last prefixes
 * in the file are mapped to the first words in the file as suffixes. (Wraps around and connects the last
 * N - 1 words to the first N - 1 words in the file).
 */
void addWrapAround(Map<Vector<string>, Vector<string> >& nGramMap, Vector<string>& allWords, int n)
{
    Vector<string> key;
    Vector<string> value;
    int wrapIndex = allWords.size();
    int wrapValueIndex = 0;
    for(int i = n - 1; i > 0; i--)
    {
       int numAdded = 0;
       key.clear();
       value.clear();
       wrapIndex -= i;
       int fromStart = 0;
       while(numAdded < n - 1)
       {
           if(wrapIndex < allWords.size())
           {
               key.add(allWords[wrapIndex]);
               wrapIndex++;
           }
           else
           {
               key.add(allWords[fromStart]);
               fromStart++;
           }
           numAdded++;
       }

       if(nGramMap.containsKey(key)) //Adds the key-value pair to the map
       {
           nGramMap[key] += allWords[wrapValueIndex];
       }
       else
       {
           value.add(allWords[wrapValueIndex]);
           nGramMap[key] = value;
       }
       wrapValueIndex++;
    }
}

/* Function: findRandomOutput
 * Usage: findRandomOutput(nGramMap, n);
 * --------------------------------------
 * This method takes in the nGramMap and the N-Gram value as parameters and
 * prompts the user for the number of words to generate. Then it chooses a random
 * starting point for the document by choosing a random key from the prefix-suffix map.
 * It then uses the map to look up all of the possible next words that can follow the
 * prefix words chosen, randomly chooses one of them, uses that for the new prefix
 * set, and continues this chain until it has printed out the user's desired
 * number of random words.
 */
void findRandomOutput(Map<Vector<string>, Vector<string> >& nGramMap, int n)
{
    Vector<Vector<string> > mapKeys = nGramMap.keys();
    while(true)
    {
        int numWords = getInteger("# of random words to generate (0 to quit)? ");
        while(numWords != 0 && numWords < n)
        {
            cout << "Must be at least " << n << " words. " << endl;
            cout << endl;
            numWords = getInteger("# of random words to generate (0 to quit)? ");
        }
        if(numWords != 0)
        {
            int high = mapKeys.size() - 1;
            int startingPoint = randomInteger(0, high);
            Vector<string> startingKey = mapKeys[startingPoint]; //gives you the very first key
            cout << "... ";
            for(int i = 0; i < startingKey.size(); i++) //Prints out the starting key prefix words
            {
                cout << startingKey[i] << " ";
            }

            int wordsUsed = startingKey.size();
            for(int word = wordsUsed; word < numWords; word++) //Continues until it has printed numWords
            {
                Vector<string> valueOptions = nGramMap[startingKey]; //Makes a vector of the possible suffix choices.
                int randomValue = randomInteger(0, valueOptions.size() - 1); //Chooses a random index of that vector.
                string chosen = valueOptions[randomValue];
                cout << chosen << " "; //Prints out the word at the random index of the suffix vector.

                Vector<string> updatedKey = startingKey;
                updatedKey.remove(0);
                updatedKey.add(chosen);
                startingKey = updatedKey; //Updates the current prefix collection for the next word.
            }
            cout << "..." << endl;
            cout << endl;
        }
        else
        {
            cout << "Exiting." << endl;
            break;
        }
    }
}

/* Main program:
 * This program makes random text based on an input file chosen by the user. It
 * makes a large data structure of word groups called N-grams and uses it to randomly
 * generate new text that sound like it came from the same author as the file.
 */
int main() {

    cout << "Welcome to CS 106B Random Writer ('N-Grams')." << endl;
    cout << "This program makes random text based on a document." << endl;
    cout << "Give me an input file and an 'N' value for groups " << endl;
    cout << "of words, and I'll create random text for you." << endl;
    cout << endl;

    Vector<string> allWords = makeVectorOfWords();
    int n = getNFromUser();
    cout << endl;

    Map<Vector<string>, Vector<string> > nGramMap = makeNGramMap(allWords, n);
    addWrapAround(nGramMap, allWords, n);
    findRandomOutput(nGramMap, n);

    return 0;
}
