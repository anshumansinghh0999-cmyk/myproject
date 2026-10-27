# Reason-Act-Reflect — Phase III C++ Prototype

This version replaces the Python/FastAPI backend with a **standalone C++11 or newer console application**. Python is not required.

## Included features
- Local account creation and sign-in
- Add and list text documents
- Keyword-based document search with ranked results
- Positive/negative relevance feedback that influences later ranking
- Per-account document files and search history
- Local persistence in the `data/` folder

## Run on Windows
1. Install a C++ compiler that provides `g++` (for example, MSYS2 MinGW-w64 GCC).
2. Ensure `g++` is available in PATH. In VS Code, open a new terminal and check:
   ```powershell
   g++ --version
   ```
3. Extract this folder and open it in VS Code.
4. Double-click `run_windows.bat`, or run:
   ```powershell
   .\run_windows.bat
   ```
5. Follow the console menu to create an account, sign in, add documents, and search.

## Run manually
```powershell
g++ -std=c++11 -O2 -Wall -Wextra main.cpp -o reason_act_reflect.exe
.\reason_act_reflect.exe
```

## Run on Linux/macOS
```bash
chmod +x run_unix.sh
./run_unix.sh
```

## Data and limitations
- Data is stored locally under `data/`, separated by username.
- This is a learning/demo prototype, **not production-ready authentication**. The sample stores passwords in a local text file and does not use secure password hashing or encryption. Do not use a real/reused password or store sensitive documents.
- The search is local keyword ranking; this is not a generative AI model, semantic vector search, or full RAG system.
- Document content is entered as a single line through the console. File upload and the previous website UI are not included in this console-only C++ conversion.
- If you want a browser website later, it can use a C++ HTTP backend, but that requires additional server code and setup.

## Project
`main.cpp` — application, local persistence, ranking, feedback, and menu.
`run_windows.bat` — compiles and launches with g++.
`run_unix.sh` — compiles and launches with g++.
