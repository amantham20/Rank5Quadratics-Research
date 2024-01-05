#include <unordered_map>
#include <set>
#include <unordered_set>
#include <iostream>

using namespace std;
int main() {

    std::unordered_set<int> map;

    for (int i = 0; i < 150; ++i) {
        map.insert(rand() % 1000);
    }

    for (auto &i : map) {
        cout << i << endl;
    }

    cout << endl;

    for (auto &i : map) {
        cout << i << endl;
    }

    cout << endl;

    for (auto it = map.begin(); it != map.end(); ++it) {
        cout << *it << endl;
    }

    cout << endl;

    for (auto it = map.begin(); it != map.end(); ++it) {
        cout << *it << endl;
    }


}