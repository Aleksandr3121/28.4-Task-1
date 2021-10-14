#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <string>
#include <chrono>
#include <utility>
#include <algorithm>
#include <iomanip>

using namespace std;

bool CheckingInputStream(istream& stream) {
    if (stream)return true;
    stream.clear();
    stream.ignore(32756, '\n');
    cerr << "Error of input" << endl;
    return false;
}

struct Sportsman {
    string name;
    double speed;
};

class Tournament {
public:
    explicit Tournament(vector<Sportsman> inAthletes) :
            athletes(move(inAthletes)),
            results(vector<int>(6, 0)),
            length(vector<double>(6, 0)),
            countOfCompletedThreads(0) {}

    void Start() {
        start = chrono::steady_clock::now();
        for (size_t i = 0; i < 6; ++i) {
            threads.push_back(thread(&Tournament::StartOneThread, this, i));
        }
        for (auto& f: threads)f.detach();
        chrono::steady_clock::time_point timerOfCout = start;
        cout<<fixed<<setprecision(2);
        while (countOfCompletedThreads < 6) {
            if (chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - timerOfCout).count() < 1000) {
                continue;
            }
            timerOfCout = chrono::steady_clock::now();
            for (int i = 0; i < 6; ++i) {
                cout << athletes[i].name << " swam " << length[i] << " meter\n";
            }
            cout << endl;
        }

        vector<pair<int, Sportsman>> v;
        for (int i = 0; i < 6; ++i) {
            v.push_back({results[i], athletes[i]});
        }
        std::sort(v.begin(), v.end(), [](auto& lhs, auto& rhs) { return lhs.first < rhs.first; });
        cout << "Result of competitions:\n";
        for (const auto&[time, sportsman]: v) {
            cout << sportsman.name << ' ' << time << " ms\n";
        }
    }

private:
    void StartOneThread(size_t index) {
        double len = 0;
        chrono::steady_clock::time_point finish;
        while (len < 100) {
            finish = chrono::steady_clock::now();
            double dur = chrono::duration_cast<chrono::milliseconds>(finish - start).count();
            len = dur * athletes[index].speed / 1000;
            length[index] = len;
        }
        results[index] = chrono::duration_cast<chrono::milliseconds>(finish - start).count();
        m.lock();
        countOfCompletedThreads++;
        m.unlock();
    }

    vector<thread> threads;
    vector<Sportsman> athletes;
    vector<int> results;
    vector<double> length;
    chrono::steady_clock::time_point start;
    int countOfCompletedThreads;
    mutex m;
};

Tournament CreateTournament() {
    vector<Sportsman> athletes;
    for (int i = 0; i < 6; ++i) {
        Sportsman person;
        cout << "Enter athlete's name: ";
        cin >> person.name;
        cout << "Enter athlete's speed: ";
        do {
            cin >> person.speed;
        } while (!CheckingInputStream(cin) || person.speed <= 0);
        athletes.push_back(move(person));
    }

    return Tournament(move(athletes));
}

int main() {
    Tournament t = CreateTournament();
    t.Start();
}
