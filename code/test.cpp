#include <iostream>
#include <vector>

using namespace std;

int main()
{
    int count = 0;

    while (++count <= 3)
    {
        cout << count
             << ",";
    }
    cout << "\n";

    count = 0;
    while (count++ <= 3)
    {
        cout << count
             << ",";
    }
    cout << "\n";

    vector<vector<int>> v{{0, 1, 2}, {3, 4, 5}};
    vector<int> v0 = v[0];
    v0.erase(v0.end());
    v[0] = v0;
    cout << v[0][0]
         << ", "
         << v[0][1]
         << ", "
         << v[1][0]
         << ", "
         << v[1][1]
         << ", "
         << v[1][2]
         << "\n";
}