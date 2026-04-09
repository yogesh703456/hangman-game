
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>   // for find()
#include <ctime>
#include <cstdlib>

using namespace std;

// ── 1. STRUCT — group word + hint together ───────────────────
//
// A "struct" is like a box that holds related data.
// Here each entry has:
//   word  → the secret month name
//   hint  → a clue for the player
struct MonthEntry {
    string word;
    string hint;
};

// ── 2. WORD BANK — all 12 months with hints ──────────────────
vector<MonthEntry> wordBank = {
    { "january",   "named after Janus, the Roman god with two faces." },
    { "february",  "Shortest month — 28 days normally, 29 in a leap year." },
    { "march",     "spring starts here. Named after Mars, god of war." },
    { "april",     " April Fools Day is on the 1st!" },
    { "may",       " flowers bloom, exams begin. Named after goddess Maia." },
    { "june",      " summer solstice. Named after goddess Juno." },
    { "july",      " named after Julius Caesar." },
    { "august",    " named after Emperor Augustus." },
    { "september", " school restarts. 'Septem' means 7 in Latin." },
    { "october",   " Halloween is on the 31st! 'Octo' means 8." },
    { "november",  " Diwali often falls here. 'Novem' means 9." },
    { "december",  " last month of the year. 'Decem' means 10." }
};

// ── 3. DRAW HANGMAN ──────────────────────────────────────────
void drawHangman(int wrongGuesses) {
    cout << "\n";
    cout << "  +---+\n";
    cout << "  |   |\n";
    cout << "  |   " << (wrongGuesses >= 1 ? "O" : " ") << "\n";   // head

    cout << "  |  ";
    if      (wrongGuesses >= 4) cout << "-|-";   // both arms
    else if (wrongGuesses >= 3) cout << " |-";   // one arm
    else if (wrongGuesses >= 2) cout << " | ";   // body only
    else                         cout << "   ";
    cout << "\n";

    cout << "  |   ";
    if      (wrongGuesses >= 6) cout << "| |";   // both legs
    else if (wrongGuesses >= 5) cout << "|  ";   // one leg
    else                         cout << "   ";
    cout << "\n";

    cout << " ===\n\n";
}

// ── 4. DISPLAY WORD (blanks and revealed letters) ────────────
void displayWord(const string& secretWord, const vector<char>& guessed) {
    cout << "Word: ";
    for (char letter : secretWord) {
        bool found = find(guessed.begin(), guessed.end(), letter) != guessed.end();
        cout << (found ? letter : '_') << " ";
    }
    cout << "\n";
}

// ── 5. CHECK WIN ─────────────────────────────────────────────
bool playerWon(const string& secretWord, const vector<char>& guessed) {
    for (char letter : secretWord) {
        if (find(guessed.begin(), guessed.end(), letter) == guessed.end())
            return false;   // found an unguessed letter
    }
    return true;
}

// ── 6. MAIN ──────────────────────────────────────────────────
int main() {

    // Stats across all games in this session
    int totalGames  = 0;
    int totalWins   = 0;
    int totalLosses = 0;

    char playAgain = 'y';

    while (playAgain == 'y' || playAgain == 'Y') {

        cout << "\n============================\n";
        cout << "  HANGMAN — MONTHS EDITION\n";
        cout << "============================\n\n";

        // --- Pick a random month ---
        srand(time(0) + totalGames);     // different seed each round
        int randomIndex     = rand() % wordBank.size();
        MonthEntry entry    = wordBank[randomIndex];
        string secretWord   = entry.word;
        string hint         = entry.hint;

        // --- Setup game state ---
        vector<char> guessedLetters;
        int wrongGuesses = 0;
        int maxWrong     = 6;
        bool hintShown   = false;

        cout << "The secret word has " << secretWord.length() << " letters.\n";
        cout << "It is a MONTH name. Type 'hint' to see a clue!\n\n";

        // --- Game loop ---
        while (wrongGuesses < maxWrong) {

            drawHangman(wrongGuesses);
            displayWord(secretWord, guessedLetters);

            cout << "Letters tried: ";
            for (char c : guessedLetters) cout << c << " ";
            cout << "\n";
            cout << "Wrong guesses left: " << (maxWrong - wrongGuesses) << "\n\n";

            // --- Get input ---
            string input;
            cout << "Enter a letter (or type 'hint'): ";
            cin >> input;

            // Convert to lowercase
            for (char& c : input) c = tolower(c);

            // --- HINT command ---
            if (input == "hint") {
                if (hintShown) {
                    cout << "\nHint already shown: " << hint << "\n\n";
                } else {
                    hintShown = true;
                    cout << "\n💡 HINT: " << hint << "\n\n";
                }
                continue;
            }

            // --- Validate single letter ---
            if (input.length() != 1 || !isalpha(input[0])) {
                cout << "\nPlease enter a single letter or type 'hint'.\n\n";
                continue;
            }

            char guess = input[0];

            // Already tried?
            bool alreadyTried = find(guessedLetters.begin(),
                                     guessedLetters.end(), guess)
                                != guessedLetters.end();
            if (alreadyTried) {
                cout << "\nYou already tried '" << guess << "'! Try another.\n\n";
                continue;
            }

            guessedLetters.push_back(guess);

            if (secretWord.find(guess) != string::npos) {
                cout << "\nCorrect! '" << guess << "' is in the word!\n\n";
            } else {
                wrongGuesses++;
                cout << "\nWrong! '" << guess << "' is NOT in the word.\n\n";
            }

            // Check win
            if (playerWon(secretWord, guessedLetters)) {
                drawHangman(wrongGuesses);
                cout << "You guessed it: " << secretWord << " 🎉\n";
                cout << "YOU WIN!\n";
                totalGames++;
                totalWins++;
                goto nextGame;
            }
        }

        // Out of guesses — game over
        drawHangman(wrongGuesses);
        cout << "Game Over! The month was: " << secretWord << "\n";
        totalGames++;
        totalLosses++;

        nextGame:

        // --- Mini dashboard after each game ---
        cout << "\n--- SESSION STATS ---\n";
        cout << "Games played : " << totalGames  << "\n";
        cout << "Wins         : " << totalWins   << "\n";
        cout << "Losses       : " << totalLosses << "\n";
        if (totalGames > 0) {
            int winPct = (totalWins * 100) / totalGames;
            cout << "Win rate     : " << winPct << "%\n";
        }
        cout << "---------------------\n\n";

        cout << "Play again? (y/n): ";
        cin >> playAgain;
    }

    cout << "\nThanks for playing! Final wins: "
         << totalWins << " / " << totalGames << "\n";

    return 0;
}

/*
 * ============================================================
 *  HOW TO COMPILE AND RUN:
 *
 *  g++ hangman.cpp -o hangman
 *  ./hangman         (Linux / Mac)
 *  hangman.exe       (Windows)
 * ============================================================
 */
