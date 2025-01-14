#include<vector>
#include<string>
using namespace std;
class Transportation {
private:
    string name;
    vector<int> capacities;
    vector<int> currentResources;
public:
    Transportation(string name);
    Transportation();
    string getName();
    void setName(string name);

    vector<int> getCapacities();
    vector<int> getCurrentResources();
    void setACurrentResource(int resource, int countToSet);
    void setAllResources(vector<int> currentResources);
};
