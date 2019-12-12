#include<iostream>
#include<unistd.h>
#include<cstdlib>
#include<fstream>
#include<cstdio>
#include<string>
#include<stack>
#include<vector>
#include<tuple>
#include<set>
#include<sstream>
#include<cmath>
#include<algorithm>
#include<ctime>
#include<cstdlib>
#include<cstring>
#include<sys/time.h>
#include<unordered_map>

using namespace std;

struct node{
	int id;
	string name;
	int w, h;
	float x, y;
};

struct CustomCompare
{

    bool operator()(const node& lhs, const node& rhs)
    {
        return lhs.x < rhs.x;
    }
};
int main(){
	set<node, CustomCompare> s;
	unordered_map<int, node*> m;
	for(int i=0; i<10; i++){
		node* n = new node();
		n->x = 10-i;
		m[i] = n;
	} 
	for(auto i=m.begin(); i!=m.end(); i++){
		s.insert(*i->second);
	}
	for(auto i=s.begin(); i!=s.end(); i++){
		cout<<(*i).x<<endl;
	}
	return 0;
}
