#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
struct Document {
    int id;
    string title, content;
    int feedback;
    Document(int documentId, const string& documentTitle, const string& documentContent, int documentFeedback)
        : id(documentId), title(documentTitle), content(documentContent), feedback(documentFeedback) {}
};

const string DATA_DIR = "data";
const string USERS_FILE = DATA_DIR + "/users.txt";
const string DOCS_FILE = DATA_DIR + "/documents.txt";
const string HISTORY_FILE = DATA_DIR + "/search_history.txt";
string currentUser;

string cleanField(string s) {
    replace(s.begin(), s.end(), '|', '/');
    replace(s.begin(), s.end(), '\t', ' ');
    replace(s.begin(), s.end(), '\n', ' ');
    replace(s.begin(), s.end(), '\r', ' ');
    return s;
}
string lower(string s) {
    transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c){ return static_cast<char>(tolower(c)); });
    return s;
}
vector<string> tokenize(const string& s) {
    vector<string> words; string word;
    for (unsigned char c : lower(s)) {
        if (isalnum(c)) word += static_cast<char>(c);
        else if (!word.empty()) { words.push_back(word); word.clear(); }
    }
    if (!word.empty()) words.push_back(word);
    return words;
}
vector<string> split(const string& s, char delim) {
    vector<string> out; string item; stringstream ss(s);
    while (getline(ss, item, delim)) out.push_back(item);
    return out;
}
string userFile(const string& suffix) {
    string safe = currentUser;
    for (char& c : safe) if (!isalnum(static_cast<unsigned char>(c))) c = '_';
    return DATA_DIR + "/" + safe + "_" + suffix;
}
vector<Document> loadDocuments() {
    vector<Document> docs;
    ifstream in(userFile("documents.txt"));
    string line;
    while (getline(in, line)) {
        auto p = split(line, '|');
        if (p.size() >= 4) {
            try { docs.push_back({stoi(p[0]), p[1], p[2], stoi(p[3])}); } catch (...) {}
        }
    }
    return docs;
}
void saveDocuments(const vector<Document>& docs) {
    ofstream out(userFile("documents.txt"), ios::trunc);
    for (const auto& d : docs)
        out << d.id << '|' << cleanField(d.title) << '|' << cleanField(d.content) << '|' << d.feedback << '\n';
}
void logSearch(const string& q) {
    ofstream out(userFile("search_history.txt"), ios::app);
    out << cleanField(q) << '\n';
}
bool registerUser() {
    string name, email, password;
    cout << "Full name: "; getline(cin, name);
    cout << "Email/username: "; getline(cin, email);
    cout << "Password (prototype only; do not reuse a real password): "; getline(cin, password);
    if (name.size() < 2 || email.empty() || password.size() < 6) {
        cout << "Please enter a name (2+ chars), username, and password (6+ chars).\n";
        return false;
    }
    ifstream in(USERS_FILE); string line;
    while (getline(in, line)) {
        auto p = split(line, '|');
        if (p.size() >= 3 && lower(p[1]) == lower(email)) {
            cout << "That username already exists.\n"; return false;
        }
    }
    ofstream out(USERS_FILE, ios::app);
    out << cleanField(name) << '|' << cleanField(email) << '|' << cleanField(password) << '\n';
    cout << "Account created. Sign in now.\n";
    return true;
}
bool login() {
    string email, password;
    cout << "Email/username: "; getline(cin, email);
    cout << "Password: "; getline(cin, password);
    ifstream in(USERS_FILE); string line;
    while (getline(in, line)) {
        auto p = split(line, '|');
        if (p.size() >= 3 && lower(p[1]) == lower(email) && p[2] == password) {
            currentUser = p[1];
            cout << "Welcome, " << p[0] << "!\n"; return true;
        }
    }
    cout << "Login failed. Check your details or create an account.\n";
    return false;
}
void addDocument() {
    string title, content;
    cout << "Document title: "; getline(cin, title);
    cout << "Enter document text on one line: "; getline(cin, content);
    if (title.empty() || content.empty()) { cout << "Title and content cannot be empty.\n"; return; }
    auto docs = loadDocuments();
    int nextId = 1;
    for (const auto& d : docs) nextId = max(nextId, d.id + 1);
    docs.push_back({nextId, cleanField(title), cleanField(content), 0});
    saveDocuments(docs);
    cout << "Document saved locally. ID: " << nextId << '\n';
}
void listDocuments() {
    auto docs = loadDocuments();
    if (docs.empty()) { cout << "No documents yet. Add one first.\n"; return; }
    cout << "\n--- MY DOCUMENTS ---\n";
    for (const auto& d : docs)
        cout << "[" << d.id << "] " << d.title << " (" << d.content.size() << " characters)\n";
}
double score(const string& query, const Document& d) {
    auto q = tokenize(query), words = tokenize(d.title + " " + d.content);
    if (q.empty() || words.empty()) return 0;
    double s = 0;
    for (const auto& term : q) {
        int count = static_cast<int>(count_if(words.begin(), words.end(), [&](const string& w){return w==term;}));
        s += min(count, 4) / (1.0 + words.size() / 150.0);
    }
    vector<string> uq = q;
    sort(uq.begin(), uq.end()); uq.erase(unique(uq.begin(), uq.end()), uq.end());
    int overlap = 0;
    for (const auto& term : uq)
        if (find(words.begin(), words.end(), term) != words.end()) ++overlap;
    s += static_cast<double>(overlap) / max<size_t>(1, uq.size());
    s += d.feedback * 0.15;
    return s;
}
void searchDocuments() {
    string query; cout << "Search query: "; getline(cin, query);
    if (query.empty()) { cout << "Query cannot be empty.\n"; return; }
    logSearch(query);
    auto docs = loadDocuments();
    sort(docs.begin(), docs.end(), [&](const Document& a, const Document& b){ return score(query,a) > score(query,b); });
    cout << "\n--- SEARCH RESULTS ---\n";
    bool found = false;
    for (const auto& d : docs) {
        double s = score(query, d);
        if (s <= 0) continue;
        found = true;
        cout << "\nID: " << d.id << " | " << d.title << " | score: " << s << "\n";
        cout << d.content << "\n";
    }
    if (!found) cout << "No matching documents. Try different keywords.\n";
    cout << "\nFeedback: enter a document ID to rate, or 0 to skip: ";
    string choice; getline(cin, choice);
    try {
        int id = stoi(choice);
        if (id != 0) {
            cout << "Enter 1 for useful, -1 for not useful: ";
            string v; getline(cin, v); int vote = stoi(v);
            if (vote != 1 && vote != -1) { cout << "Invalid vote.\n"; return; }
            auto all = loadDocuments(); bool ok = false;
            for (auto& d : all) if (d.id == id) { d.feedback = max(-10, min(10, d.feedback + vote)); ok = true; }
            if (ok) { saveDocuments(all); cout << "Feedback saved and will influence future ranking.\n"; }
            else cout << "Document ID not found.\n";
        }
    } catch (...) { cout << "Feedback skipped.\n"; }
}
void showHistory() {
    ifstream in(userFile("search_history.txt")); string line; int n=0;
    cout << "\n--- RECENT SEARCHES ---\n";
    while (getline(in,line)) { cout << ++n << ". " << line << '\n'; }
    if (!n) cout << "No search history yet.\n";
}
void appMenu() {
    while (true) {
        cout << "\n===== REASON-ACT-REFLECT | PRIVATE KNOWLEDGE SEARCH =====\n"
             << "1. Add document\n2. List documents\n3. Search documents\n"
             << "4. Search history\n5. Log out\n0. Exit\nChoose: ";
        string choice; getline(cin, choice);
        if (choice=="1") addDocument();
        else if (choice=="2") listDocuments();
        else if (choice=="3") searchDocuments();
        else if (choice=="4") showHistory();
        else if (choice=="5") { currentUser.clear(); return; }
        else if (choice=="0") exit(0);
        else cout << "Choose a valid option.\n";
    }
}
int main() {
#ifdef _WIN32
    system("if not exist data mkdir data");
#else
    system("mkdir -p data");
#endif
    while (true) {
        cout << "\n========== REASON-ACT-REFLECT (C++ VERSION) ==========\n"
             << "1. Create account\n2. Sign in\n0. Exit\nChoose: ";
        string choice; getline(cin, choice);
        if (choice=="1") registerUser();
        else if (choice=="2") { if (login()) appMenu(); }
        else if (choice=="0") break;
        else cout << "Choose a valid option.\n";
    }
    cout << "Goodbye.\n";
    return 0;
}
