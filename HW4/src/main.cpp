#include<iostream>
#include<unistd.h>
#include<cstdlib>
#include<fstream>
#include<cstdio>
#include<string>
#include<stack>
#include<vector>
#include<tuple>
#include<list>
#include<sstream>
#include<cmath>
#include<set>
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

struct row{
	int coordinate;
	int height;
	int sitewidth, sitespacing;
	int siteorient, sitesymmetry;
	int subroworigin;
	int numsite;
	vector<row> subrow;
};


struct CustomCompare
{

    bool operator()(const node& lhs, const node& rhs)
    {
        if(lhs.x != rhs.x){
        	return lhs.x < rhs.x;
		}else{
			return lhs.y < rhs.y;
		}
		
    }
};

//gloabal var
string nodeFile, plFile, sclFile;
string testcase_path = "../testcase/adaptec1/";
int maxDisplacement;
int numNode, numTerminal;
int numRow;
unordered_map<int, node*> blocks, terminals;
vector<row> rows;
//

void read_aux(FILE* input){
	char n[30], pl[30], scl[30];
	fscanf(input, " RowBasedPlacement : %s %s %s\n", n, pl, scl);
	nodeFile = n;
	plFile = pl;
	sclFile = scl;
	fscanf(input, " MaxDisplacement : %d\n", &maxDisplacement);
	cout<<nodeFile<<" "<<plFile<<" "<<sclFile<<endl;
	return;
}

void read_node(){
	string path = testcase_path+nodeFile;
	char p[50];
	strcpy(p, path.c_str());
	FILE* input = fopen(p, "r");
	string str;
	char c[100];
	
	fscanf(input, "%[^\n]%*c", c); // skip first row
	while(true){ //skip comment
		fscanf(input, "%s", c);
		if(c[0] == '#'){
			fscanf(input, "%[^\n]%*c", c);
			continue;
		}else{
			break;	
		}	
	}
	fscanf(input, " : %d\n", &numNode);
	fscanf(input, " NumTerminals : %d\n", &numTerminal);
	for(int i=0; i<numNode - numTerminal; i++){
		node* n = new node();
		fscanf(input, " %s %d %d\n", c, &n->w, &n->h);
		n->name = str = c;
		n->id = stoi(str.erase(0,1));
		blocks[n->id] = n;
	}
	for(int i=numNode - numTerminal; i<numNode; i++){
		node* n = new node();
		fscanf(input, " %s %d %d terminal\n", c, &n->w, &n->h);
		n->name = str = c;
		n->id = stoi(str.erase(0,1));
		terminals[n->id] = n;
	}
	return;
} 

void read_pl(){
	string path = testcase_path+plFile;
	char p[50];
	strcpy(p, path.c_str());
	FILE* input = fopen(p, "r");
	char c[100];
	fscanf(input, "%[^\n]%*c", c); // skip first row
	string str;
	float x, y;
	int id;
	for(int i=0; i<numNode - numTerminal; i++){
		node* n = new node();
		fscanf(input, " %s %f %f : N\n", c, &x, &y);
		str = c;
		id = stoi(str.erase(0,1));
		blocks[id]->x = x;
		blocks[id]->y = y;
	}
	for(int i=numNode - numTerminal; i<numNode; i++){
		node* n = new node();
		fscanf(input, " %s %f %f  : N /FIXED\n", c, &x, &y);
		str = c;
		id = stoi(str.erase(0,1));
		terminals[id]->x = x;
		terminals[id]->y = y;
	}
	return;
}

void read_scl(){
	string path = testcase_path+sclFile;
	char p[50], c[100];
	strcpy(p, path.c_str());
	FILE* input = fopen(p, "r");
	fscanf(input, "%[^\n]%*c", c);
	while(true){ //skip comment
		fscanf(input, "%s", c);
		if(c[0] == '#'){
			fscanf(input, "%[^\n]%*c", c);
			continue;
		}else{
			break;	
		}	
	}
	fscanf(input, " : %d", &numRow);
	//cout<<numRow<<endl;
	for(int i=0; i<numRow; i++){
		row r;
		fscanf(input, " CoreRow Horizontal\n");
		fscanf(input, " Coordinate    : %d\n", &r.coordinate);
		fscanf(input, " Height        : %d\n", &r.height);
		fscanf(input, " Sitewidth     : %d\n", &r.sitewidth);
		fscanf(input, " Sitespacing   : %d\n", &r.sitespacing);
		fscanf(input, " Siteorient    : %d\n", &r.siteorient);
		fscanf(input, " Sitesymmetry  : %d\n", &r.sitesymmetry);
		fscanf(input, " SubrowOrigin  : %d NumSites  : %d\n", &r.subroworigin, &r.numsite);
		fscanf(input, " End\n");
		//cout<<r.coordinate<<" "<<r.subroworigin<<endl;
		rows.push_back(r);
	}
	
}

void generate_subrows(){
	int rowcnt = 0;
	set<node, CustomCompare> obstacle;
	for(auto i=terminals.begin(); i!=terminals.end(); i++){
		obstacle.insert(*i->second);
		
	}
	//cout<<"o: "<<obstacle.size()<<endl;
	for(int i=0; i<rows.size(); i++){
		int tmpx = rows[i].subroworigin, endx = rows[i].subroworigin + rows[i].numsite;
		int tmpy = rows[i].coordinate;
		cout<<"row "<<i<<":\n";
		for(auto o=obstacle.begin(); o!=obstacle.end();o++){
			if(o->x == tmpx && tmpy >= o->y && tmpy < o->y+o->h){
				tmpx = tmpx + o->w;
			}else if(o->x > tmpx && o->x < endx && tmpy >= o->y && tmpy < o->y+o->h){
				row r;
				r.subroworigin = tmpx;
				r.coordinate = tmpy;
				r.numsite = o->x - tmpx;
				tmpx = o->x + o->w;
				rowcnt++;
				rows[i].subrow.push_back(r);
				cout<<"("<<r.subroworigin<<", "<<r.subroworigin+r.numsite<<") ";
			}else if(tmpx >= endx){
				break;
			}
		}
		if(tmpx < rows[i].subroworigin + rows[i].numsite){
			row r;
			r.subroworigin = tmpx;
			r.coordinate = tmpy;
			r.numsite = rows[i].subroworigin + rows[i].numsite - tmpx;
			rowcnt++;
			rows[i].subrow.push_back(r);
			cout<<"("<<r.subroworigin<<", "<<r.subroworigin+r.numsite<<") ";
		}
		cout<<endl;
	}
	cout<<"r: "<<rowcnt<<endl;
	return;
}

int main(void){
	FILE* aux = fopen("../testcase/adaptec1/adaptec1.aux", "r");
	read_aux(aux);
	read_node();
	read_pl();
	read_scl();
	
	generate_subrows();
	
	
	return 0;	
}
	
