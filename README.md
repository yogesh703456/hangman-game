🎮Hangman — Months Edition
Complete System Documentation for First Year Students
Table of Contents
1. Project Overview
2. File Structure
3. How to Run the Game
4. Architecture — How the Three Versions Connect
5. HTMLWeb Version — Full Breakdown
Authentication System
Game Engine
Dashboard & Stats
Data Management
6. C++ Console Version — Full Breakdown
7. UI/UX Design System
8. Data Storage — localStorage Explained
9. JavaScript Concepts Used
10. C++ Concepts Used
11. Feature Comparison Table
12. Known Limitations & Notes for Students
1. Project Overview
This is a Hangman word-guessing game where the secret word is always one of the 12 months
of the year. It is built in two parallel versions:
Version File Technology Players
Web (browser) hangman.html HTML + CSS + JavaScript Multiple (per-login)
Console (terminal) hangman.cpp C++ Single session
Both versions share the same core game logic — guess letters one at a time, use a hint if stuck,
and you have 6 wrong guesses before the hangman is complete.
The web version adds a full user account system with persistent stats, a dashboard, and data
export/import.
2. File Structure
project/
│
├── hangman.html ← The complete web game (open in browser)
├── hangman.cpp ← The C++ console version (compile & run)
├── start_server.bat ← Windows launcher (double-click to start)
└── game_data.txt ← Documentation / example exported player data
Note: All four files should be kept in the same folder.
3. How to Run the Game
Web Version (Recommended)
Option A — Windows (easiest):
1. Put all files in one folder.
2. Double-click start_server.bat .
3. A black terminal window opens and starts a local Python web server.
4. Your browser opens automatically at http://localhost:8000/hangman.html .
5. Press Ctrl+C in the black window to stop the server when done.
What start_server.bat does step by step:
1. Checks if Python is installed (python --version)
2. Changes directory to the folder containing the files (cd /d "%~dp0")
3. Opens the browser after a 2-second delay (start "" cmd /c "timeout /t 2 &&
start http://...")
4. Starts Python's built-in HTTP server (python -m http.server 8000)
Option B — Any OS (manual):
bash
# Navigate to your project folder
cd path/to/your/project
# Start Python server
python -m http.server 8000
# Then open in browser:
# http://localhost:8000/hangman.html
Option C — Direct open (may work): Simply double-click hangman.html . Works in most
browsers, but some browsers block localStorage for file:// URLs, so the server method is
safer.
C++ Console Version
bash
# Compile
g++ hangman.cpp -o hangman
# Run on Linux/Mac
./hangman
# Run on Windows
hangman.exe
4. Architecture
The system has two completely independent versions that share the same game concept and
word bank, but use entirely different technologies.
┌─────────────────────────────────────────────────────────────┐
│ HANGMAN — MONTHS EDITION │
├────────────────────────────┬────────────────────────────────┤
│ WEB VERSION │ CONSOLE VERSION │
│ hangman.html │ hangman.cpp │
│ │ │
│ ┌──────────────────────┐ │ ┌─────────────────────────┐ │
│ │ Auth Screen │ │ │ main() loop │ │
│ │ (Login / Register) │ │ │ - srand() for random │ │
│ └──────────┬───────────┘ │ │ - while(play again) │ │
│ │ │ └──────────┬──────────────┘ │
│ ┌──────────▼───────────┐ │ │ │
│ │ App Screen │ │ ┌──────────▼──────────────┐ │
│ │ - Game Page │ │ │ Game Loop │ │
│ │ - Dashboard Page │ │ │ - drawHangman() │ │
│ └──────────┬───────────┘ │ │ - displayWord() │ │
│ │ │ │ - playerWon() │ │
│ ┌──────────▼───────────┐ │ └──────────┬──────────────┘ │
│ │ localStorage │ │ │ │
│ │ hangman_users │ │ ┌──────────▼──────────────┐ │
│ │ hangman_session │ │ │ Session Stats │ │
│ └──────────────────────┘ │ │ (in memory only) │ │
│ │ └─────────────────────────┘ │
└────────────────────────────┴────────────────────────────────┘
Shared: Word Bank (12 months + hints)
5. HTMLWeb Version
The entire web game lives inside one single HTML file ( hangman.html ). It is divided into three
sections:
hangman.html
├── <style> ← All CSS (visual design, ~630 lines)
├── <body> ← HTML structure (~260 lines)
└── <script> ← All JavaScript logic (~620 lines)
5.1 Authentication System
The game has a full login/register system before you can play.
How it works — two "screens"
Registration ( doRegister() )
Validation checks performed (in order):
1. Full name must not be empty.
2. Username must not be empty and must not contain spaces.
3. Username must not already exist in localStorage.
4. Password must be at least 6 characters.
5. Confirm password must match password.
If all pass:
This is saved into localStorage["hangman_users"]["priya"] .
Password Strength Meter (visual bar on register page):
Browser loads → checkSession() runs
│
├── Session found in localStorage?
│ YES → skip login → enterApp()
│ NO → show #auth-screen
│
└── #auth-screen has two tabs:
"Login" → #form-login (shown by default)
"Register" → #form-register (hidden by default)
javascript
const newUser = {
fullName: "Priya Sharma",
username: "priya",
password: "abc123", // stored as plain text
joinedDate: "09/04/2026",
stats: {
played: 0, wins: 0, losses: 0,
streak: 0, bestStreak: 0, history: []
}
};
Score Criteria Met Label Bar Width Colour
0 nothing (empty) 0% —
1 length ≥ 6 Weak 30% Red
2 length ≥ 10 Fair 55% Orange
3 has uppercase Good 80% Yellow
4 has number/symbol Strong 100% Green
The score is calculated in checkPasswordStrength() using four regex tests.
Login ( doLogin() )
1. Looks up the username in getAllUsers() .
2. If not found → shows error under username field.
3. Compares entered password with stored password.
4. If wrong → shows error under password field.
5. If correct → saves username to localStorage["hangman_session"] → calls enterApp() .
Enter key support: The password field has onkeydown="if(event.key==='Enter') doLogin()"
so students don't have to click the button.
Session Persistence ( checkSession() )
This self-invoking function runs immediately when the page loads:
javascript
If the player closed the tab without logging out, they are automatically re-logged in the next time.
Logout ( doLogout() )
1. Asks for confirmation ( confirm() popup).
2. Saves current stats to localStorage.
3. Removes hangman_session from localStorage.
4. Resets currentUser and stats variables to null .
5. Hides #app-screen , shows #auth-screen .
6. Clears the password field for security.
5.2 Game Engine
After login, the game page is shown. A new game starts automatically via startGame() .
Word Selection
Math.random() returns a float from 0 to <1. Multiplying by 12 and flooring gives a random index
0–11.
(function checkSession() {
const savedUsername = localStorage.getItem(SESSION_KEY);
if (savedUsername) {
const users = getAllUsers();
if (users[savedUsername]) {
currentUser = users[savedUsername];
stats = currentUser.stats;
enterApp(); // skip login screen entirely
return;
}
}
document.getElementById('auth-screen').style.display = 'flex';
})();
javascript
const idx = Math.floor(Math.random() * wordBank.length);
currentEntry = wordBank[idx];
secretWord = currentEntry.word;
Word Bank (12 entries)
Game State Variables
The Hangman (SVG)
The gallows is drawn using SVG (Scalable Vector Graphics) — an XML-based image format:
 
javascript
const wordBank = [
{ word:"january", hint:" named after Janus, the Roman god with two faces." },
{ word:"february", hint:"Shortest month — 28 days normally, 29 in a leap year." }
{ word:"march", hint:" spring starts here. Named after Mars, god of war." },
{ word:"april", hint:" April Fools Day is on the 1st!" },
{ word:"may", hint:" flowers bloom, exams begin. Named after goddess Maia."
{ word:"june", hint:" summer solstice. Named after goddess Juno." },
{ word:"july", hint:" named after Julius Caesar." },
{ word:"august", hint:" named after Emperor Augustus." },
{ word:"september", hint:" school restarts. 'Septem' means 7 in Latin." },
{ word:"october", hint:" Halloween on the 31st! 'Octo' means 8." },
{ word:"november", hint:" Diwali often falls here. 'Novem' means 9." },
{ word:"december", hint:" last month of the year. 'Decem' means 10." }
];
javascript
let currentEntry = null; // the chosen word+hint object
let secretWord = ""; // e.g. "october"
let guessedLetters = []; // e.g. ['o', 't', 'x']
let wrongGuesses = 0; // 0 to 6
let hintUsed = false; // was hint revealed?
html
All .body-part elements start with opacity: 0 in CSS. Each wrong guess reveals the next one:
The reveal order is: Head → Body → Left Arm → Right Arm → Left Leg → Right Leg.
Word Display ( renderWord() )
<svg id="gallows" viewBox="0 0 200 200">
<!-- Static structure (always visible) -->
<line x1="20" y1="190" x2="180" y2="190"/> <!-- base -->
<line x1="60" y1="190" x2="60" y2="10"/> <!-- pole -->
<line x1="60" y1="10" x2="130" y2="10"/> <!-- top arm -->
<line x1="130" y1="10" x2="130" y2="40"/> <!-- rope -->
<!-- Body parts (start invisible, appear on each wrong guess) -->
<circle class="body-part" id="bp-head" cx="130" cy="55" r="15"/>
<line class="body-part" id="bp-body" x1="130" y1="70" x2="130" y2="120"/>
<line class="body-part" id="bp-larm" x1="130" y1="80" x2="105" y2="105"/>
<line class="body-part" id="bp-rarm" x1="130" y1="80" x2="155" y2="105"/>
<line class="body-part" id="bp-lleg" x1="130" y1="120" x2="105" y2="155"/>
<line class="body-part" id="bp-rleg" x1="130" y1="120" x2="155" y2="155"/>
</svg>
javascript
const bodyParts = ["bp-head","bp-body","bp-larm","bp-rarm","bp-lleg","bp-rleg"];
// On a wrong guess:
const part = document.getElementById(bodyParts[wrongGuesses - 1]);
part.style.opacity = "1";
javascript
function renderWord() {
const display = document.getElementById('word-display');
display.innerHTML = '';
for (const letter of secretWord) {
const box = document.createElement('div');
box.className = 'letter-box';
box.textContent = guessedLetters.includes(letter) ? letter.toUpperCase() : '';
display.appendChild(box);
}
}
Each letter gets its own styled <div> with a bottom border. Letters are shown when guessed,
otherwise the box is empty (underscore effect achieved via CSS border).
On-Screen Keyboard ( renderKeyboard() )
Keys are coloured green (correct) or red (wrong) after being pressed, and disabled so they can't
be clicked again.
Guess Handler ( handleGuess(letter) )
javascript
function renderKeyboard() {
const kb = document.getElementById('keyboard');
kb.innerHTML = '';
for (let i = 65; i <= 90; i++) { // ASCII 65=A, 90=Z
const letter = String.fromCharCode(i).toLowerCase();
const btn = document.createElement('button');
btn.className = 'key';
btn.textContent = letter.toUpperCase();
btn.onclick = () => handleGuess(letter);
if (guessedLetters.includes(letter)) {
btn.disabled = true;
btn.classList.add(secretWord.includes(letter) ? 'correct' : 'wrong');
}
kb.appendChild(btn);
}
}
Player clicks a key
│
▼
Is letter already in guessedLetters[]?
YES → do nothing (button disabled)
NO →
push letter into guessedLetters[]
Is letter in secretWord?
YES → renderWord() (reveal letter)
NO → wrongGuesses++ → show next body part
renderKeyboard()
checkGameOver()
updateStatus()
Win/Lose Check ( checkGameOver() )
[...secretWord] spreads the string into an array of characters. .every() returns true only if
every letter has been guessed.
Win: Banner shows "🎉YOU WIN!", coloured green. recordResult('win') is called.
Lose: wrongGuesses >= 6 . Banner shows "💀GAME OVER — OCTOBER", coloured red.
recordResult('lose') is called.
Hint System
The hint button is disabled after first use. hintUsed is recorded with the game result.
5.3 Dashboard & Stats
Clicking the Dashboard tab shows the current player's statistics.
Stats Object Structure
javascript
const won = [...secretWord].every(l => guessedLetters.includes(l));
javascript
function useHint() {
document.getElementById('hint-display').textContent = currentEntry.hint;
document.getElementById('hint-btn').disabled = true;
hintUsed = true;
}
javascript
Recording a Result ( recordResult() )
stats = {
played: 10,
wins: 7,
losses: 3,
streak: 3, // current win streak
bestStreak: 5, // all-time best streak
history: [ // newest first, max 10 entries
{
word: "october",
result: "win",
wrong: 2,
hintUsed: false,
timestamp: "09/04/2026, 14:32:05"
},
// ...
]
}
javascript
Dashboard Rendering ( renderDashboard() )
The function reads from the stats object and injects values into the HTML:
4 stat cards: Played, Win Rate, Wins, Losses
Win rate bar: A CSS-animated horizontal bar ( <div> with width: winRate% )
History table: A <table> built dynamically with one <tr> per game
Data textarea: Shows the raw JSON of stats for manual copy/paste
5.4 Data Management
The dashboard has four action buttons:
function recordResult(result) {
stats.played++;
if (result === 'win') {
stats.wins++;
stats.streak++;
if (stats.streak > stats.bestStreak) stats.bestStreak = stats.streak;
} else {
stats.losses++;
stats.streak = 0; // streak resets on any loss
}
stats.history.unshift({ // add to FRONT of array
word: secretWord,
result: result,
wrong: wrongGuesses,
hintUsed: hintUsed,
timestamp: new Date().toLocaleString()
});
if (stats.history.length > 10) stats.history.pop(); // keep max 10
persistCurrentUserStats(); // auto-save immediately
showToast("✅ Game saved for " + currentUser.username + "!");
}
Button Function What it does
💾Save saveData() Writes stats to localStorage and refreshes textarea
📥Export .txt exportData() Downloads a formatted .txt file to your computer
📂Load loadData() Parses JSON from textarea and restores stats
🗑 Reset resetStats() Resets all stats to zero after confirmation
Export creates a Blob object from a text string and triggers a download:
Load validates the JSON before applying it:
6. C++ Console Version
The C++ version ( hangman.cpp ) is a terminal game with the same word bank and hint system.
Key Concepts
Struct ( MonthEntry )
javascript
const blob = new Blob([content], { type:'text/plain' });
const link = document.createElement('a');
link.href = URL.createObjectURL(blob);
link.download = 'game_data_priya.txt';
link.click();
 
javascript
const parsed = JSON.parse(text);
if (typeof parsed.played === 'undefined') { showToast("❌ Invalid format!"); return;
stats = parsed;
cpp
A struct groups related data. Each MonthEntry holds one month's name and its hint. This is
equivalent to a JavaScript object like { word: "july", hint: "..." } .
Vector (Dynamic Array)
vector is C++'s dynamic list — it can hold any number of items and is accessed by index (e.g.
wordBank[3] ).
Random Selection
srand() seeds the random number generator. Adding totalGames ensures a different seed
each round even if the player is fast.
Game Loop
struct MonthEntry {
string word;
string hint;
};
cpp
vector<MonthEntry> wordBank = {
{ "january", "named after Janus..." },
{ "february", "Shortest month..." },
// ... 12 total
};
cpp
srand(time(0) + totalGames); // seed based on current time
int randomIndex = rand() % wordBank.size();
MonthEntry entry = wordBank[randomIndex];
Key Functions
drawHangman(int wrongGuesses) — prints ASCII art showing the current state:
displayWord(const string& secretWord, const vector<char>& guessed) — prints each
letter or _ using find() :
playerWon() — checks if every letter in secretWord is in guessed :
while (wrongGuesses < 6) {
drawHangman(wrongGuesses)
displayWord(secretWord, guessedLetters)
show guessed letters and wrong count
get input from player (cin >> input)
if input == "hint" → show hint, continue
if input not a single letter → error, continue
if already guessed → error, continue
add to guessedLetters
if correct → continue
if wrong → wrongGuesses++
if playerWon() → celebrate, goto nextGame
}
// if loop ends naturally → player lost
+---+
| |
| O ← appears at wrongGuesses >= 1
| -|- ← arms appear at >= 2 (body), >= 3 (one), >= 4 (both)
| | | ← legs appear at >= 5 (one), >= 6 (both)
===
cpp
for (char letter : secretWord) {
bool found = find(guessed.begin(), guessed.end(), letter) != guessed.end();
cout << (found ? letter : '_') << " ";
}
cpp
Session Stats
These persist across rounds (within one program run) and are printed after each game as a "mini
dashboard". They are lost when the program exits (no file saving in the C++ version).
7. UI/UX Design System
The web version uses a carefully crafted dark-theme design system defined via CSS custom
properties (variables).
Colour Palette
for (char letter : secretWord) {
if (find(guessed.begin(), guessed.end(), letter) == guessed.end())
return false;
}
return true;
cpp
int totalGames = 0;
int totalWins = 0;
int totalLosses = 0;
css
:root {
--bg: #0a0a0f; /* Page background — very dark navy */
--paper: #13131c; /* Card backgrounds */
--card2: #1a1a28; /* Inputs and secondary cards */
--accent: #7eb8f7; /* Sky blue — primary brand colour */
--gold: #e8c87a; /* Hint button, win rate text */
--red: #c0392b; /* Losses, wrong keys, errors */
--green: #27ae60; /* Wins, correct keys */
--purple: #9b59b6; /* Avatar gradient */
--text: #dde4f0; /* Main body text */
--muted: #555e78; /* Labels, secondary text */
--border: #252535; /* Card borders */
}
Typography
Two Google Fonts are imported:
Font Usage Style
VT323 Titles, headings, the hangman banner Retro pixel/terminal feel
Special Elite Body text, buttons, labels Typewriter feel
Layout
The page uses flexbox throughout:
Body is a vertical flexbox, centering all content.
Auth screen is max-width: 420px , centered.
App screen cards are max-width: 580px , centered.
All pages use display: none / display: flex toggling to switch between views.
Key UI Components
Auth Screen
Tab switcher: Two buttons that toggle active state and show/hide the correct form card.
Field groups: Label + input + error span — the error span is always present (min-height:
1em) so layout doesn't jump when an error appears.
Inputs: Dark background ( --card2 ), border changes colour on :focus .
Game Screen
Gallows (SVG): Vector drawing, no pixelation, scales cleanly.
Letter boxes: Each letter is a <div> with a bottom border — blank when not guessed.
Keyboard: 26 <button> elements in a flex-wrap grid. Colour changes on guess (green/red),
then disabled.
Hint row: Gold text on a darker card, button disabled after use.
Status bar: Dynamic text updates (e.g. "⚠️Last chance!").
Dashboard
Stat grid: 4 equally-spaced cards in a CSS grid (2×2).
Win rate bar: An animated CSS width transition from 0% to the win rate %.
History table: Styled <table> with alternating visual cues via badge colours.
Data textarea: Monospace-style textarea for JSON display and paste.
Micro-interactions
Buttons lift slightly on hover: transform: translateY(-2px) .
Opacity drops on hover: opacity: 0.88 .
Border glows on input focus: border-color: var(--accent) .
Toast notification slides in from bottom ( opacity: 0 → 1 ) and auto-fades after 2.5
seconds.
Hangman body parts fade in: transition: opacity 0.35s .
Responsive Design
max-width on all containers prevents over-stretching on wide screens.
flex-wrap on the keyboard allows it to reflow on narrow screens.
viewport meta tag: <meta name="viewport" content="width=device-width, initial￾scale=1.0"/> .
8. Data Storage — localStorage Explained
localStorage is a browser built-in key-value store. Keys and values are always strings. JSON is
used to store complex objects.
Keys Used
Key Type Contents
hangman_users JSON string Object of all player accounts
hangman_session plain string Username of currently logged-in player
Full Data Shape
javascript
Helper Functions
// localStorage["hangman_users"] stores:
{
"priya": {
fullName: "Priya Sharma",
username: "priya",
password: "abc123",
joinedDate: "09/04/2026",
stats: {
played: 5,
wins: 3,
losses: 2,
streak: 1,
bestStreak: 3,
history: [
{
word: "october",
result: "win",
wrong: 2,
hintUsed: false,
timestamp: "09/04/2026, 15:00:00"
}
// ... up to 10 entries
]
}
},
"rahul": { /* same shape */ }
}
javascript
localStorage Limits & Behaviour
Capacity: ~5MB per origin (more than enough for this game).
Scope: Only available in the same browser on the same computer.
Persistence: Survives page refresh and browser close, but is wiped if the user clears
browser data/cookies.
Transfer: Use the Export → Load workflow to move data between computers.
9. JavaScript Concepts Used
Concept Where Used Example
const / let All variables const wordBank = [...]
Array methods Checking
guesses
guessedLetters.includes(letter)
Spread operator Win check [...secretWord].every(...)
Arrow functions Keyboard
buttons
btn.onclick = () => handleGuess(letter)
// Read all users
function getAllUsers() {
const raw = localStorage.getItem('hangman_users');
return raw ? JSON.parse(raw) : {};
}
// Write all users back
function saveAllUsers(users) {
localStorage.setItem('hangman_users', JSON.stringify(users));
}
// Save just the current player's stats
function persistCurrentUserStats() {
const users = getAllUsers();
users[currentUser.username].stats = stats;
saveAllUsers(users);
}
Concept Where Used Example
Template literals Dynamic
HTML
`Wrong: ${wrongGuesses}/6`
JSON.parse /
JSON.stringify
localStorage Convert objects ↔ strings
localStorage Data
persistence
localStorage.setItem(key, value)
DOM manipulation UI updates document.getElementById() , .textContent ,
.classList
Math.random() Word selection Random index into wordBank
Date() Timestamps new Date().toLocaleString()
IIFE Session check (function checkSession() { ... })()
Blob +
URL.createObjectURL
File export Trigger .txt download
try/catch JSON load Handle invalid paste
Regex Password
strength
/[A-Z]/.test(pw)
SVG manipulation Hangman
reveal
element.style.opacity = "1"
String.fromCharCode Keyboard gen Loop A–Z (65–90)
10. C++ Concepts Used
Concept Where Used
struct MonthEntry — grouping word + hint
vector<T> wordBank , guessedLetters
string All text handling
Concept Where Used
find() from <algorithm> Check if letter already guessed
rand() / srand() / time() Random month selection
tolower() Convert input to lowercase
Range-based for loop Iterating over characters and lists
goto Jump to next game on win
continue Skip current loop iteration on invalid input
Functions with parameters drawHangman() , displayWord() , playerWon()
cin / cout Terminal input/output
References ( const string& ) Pass-by-reference to avoid copying
11. Feature Comparison Table
Feature Web ( hangman.html ) Console ( hangman.cpp )
Word bank 12 months with hints 12 months with hints
Hint system Button (one use per game) Type "hint" command
Hangman drawing SVG vector art (animated) ASCII text art
Letter input Click on-screen keyboard Type letter and press Enter
Multiple players Yes (login system) No (single session)
Per-player stats Yes (localStorage) Session totals only
Streak tracking Yes (win streak + best) No
Game history Last 10 games per player No
Data export Yes (.txt file download) No
Data import Yes (JSON paste + load) No
Persistent data Yes (survives browser close) No (lost when program exits)
Feature Web ( hangman.html ) Console ( hangman.cpp )
Wrong guess limit 6 6
12. Known Limitations & Notes for Students
Security
Passwords are stored as plain text in localStorage. This is acceptable for a school project
but never appropriate in real applications. Production apps use password hashing
algorithms like bcrypt .
There is no server-side validation. Any player could open browser DevTools and edit
localStorage directly.
Data Safety
Clearing browser history/cookies deletes all player accounts. Always export your data if
you switch computers or clear the browser.
There is no backup mechanism beyond the manual Export/Load system.
C++ goto
The C++ version uses goto nextGame to jump out of the game loop on a win. In professional
C++ this is considered bad practice — a bool gameWon flag and a break statement would be
cleaner.
No Sound orAnimation in C++
The console version has no sound, colours, or animations. These require platform-specific
libraries. The web version uses CSS transitions for smooth reveal effects.
Browser Compatibility
The HTML game uses modern JavaScript features ( const , arrow functions, spread operator,
template literals). It works in all browsers released after 2015. Internet Explorer is not supported.
Running Without a Server
Some browsers (especially Chrome) block localStorage when opening HTML files directly
from disk ( file:// URLs). Always use the start_server.bat or python -m http.server to
avoid this.
Documentation generated for Hangman — Months Edition · First Year Students · April 2026
Author:- Yogesh kumar saini 
