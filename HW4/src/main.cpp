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

struct cluster{
	vector<int> cells;
	int idx;
	int e;
	float x;
	int w;
	float q;
};

struct row{
	int coordinate;
	int height;
	int sitewidth, sitespacing;
	int siteorient, sitesymmetry;
	int subroworigin;
	int numsite;
	int w=0;
	vector<cluster> clusters;
};



struct CustomCompare
{

    bool operator()(const node& lhs, const node& rhs)
    {
        if(lhs.x != rhs.x){
        	return lhs.x < rhs.x;
		}else if(lhs.y != rhs.y){
			return lhs.y < rhs.y;
		}else{
			return lhs.id < rhs.id;
		}
		
    }
};

//gloabal var
string nodeFile, plFile, sclFile;
string testcase_path = "../testcase/adaptec1/";
int maxDisplacement;
int numNode, numTerminal;
int numRow;
int height, buttom=1000000;
unordered_map<int, node*> blocks, terminals;
vector<row> rows, subrows;

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
		if(buttom>r.coordinate) buttom = r.coordinate;
		height = r.height;
		rows.push_back(r);
	}
	
}

void generate_subrows(){
	int rowcnt = 0;
	height = rows[0].height;
	set<node, CustomCompare> obstacle;
	for(auto i=terminals.begin(); i!=terminals.end(); i++){
		obstacle.insert(*i->second);
		
	}
	//cout<<"o: "<<obstacle.size()<<endl;
	for(int i=0; i<rows.size(); i++){
		int tmpx = rows[i].subroworigin, endx = rows[i].subroworigin + rows[i].numsite;
		int tmpy = rows[i].coordinate;
		//cout<<"row "<<i<<":\n";
		for(auto o=obstacle.begin(); o!=obstacle.end();o++){
			if(o->x == tmpx && tmpy >= o->y && tmpy < o->y+o->h){
				tmpx = tmpx + o->w;
			}else if(o->x > tmpx && o->x < endx && tmpy >= o->y && tmpy < o->y+o->h){
				row r = rows[i];
				r.subroworigin = tmpx;
				r.coordinate = tmpy;
				r.numsite = o->x - tmpx;
				tmpx = o->x + o->w;
				rowcnt++;
				subrows.push_back(r);
				//cout<<"("<<r.subroworigin<<", "<<r.coordinate<<", "<<r.numsite<<") ";
			}else if(tmpx >= endx){
				break;
			}
		}
		if(tmpx < rows[i].subroworigin + rows[i].numsite){
			row r = rows[i];
			r.subroworigin = tmpx;
			r.coordinate = tmpy;
			r.numsite = rows[i].subroworigin + rows[i].numsite - tmpx;
			rowcnt++;
			subrows.push_back(r);
			//cout<<"("<<r.subroworigin<<", "<<r.coordinate<<", "<<r.numsite<<") ";
		}
		//cout<<endl;
	}
	//cout<<"r: "<<rowcnt<<endl;
	cout<<"buttom: "<<buttom<<" row height: "<<height<<endl;
	return;
}

cluster add_cell(cluster c, node n){
	c.cells.push_back(n.id);
	c.e++;
	c.q += n.x - c.w;
	c.w += n.w;
	return c;
}

cluster add_cluster(cluster c, cluster c_prev){
	for(int i=0; i<c.cells.size();i++){
		c_prev.cells.push_back(c.cells[i]);
	}
	c_prev.e += c.e;
	c_prev.q += c.q - c.e*c_prev.w;
	c_prev.w += c.w;
	return c_prev;	
}

void collapse(cluster c, int r){
	c.x = (int)1.0*c.q/c.e;
	if(c.x < subrows[r].subroworigin){
		c.x = subrows[r].subroworigin;
	}
	if(c.x+c.w > subrows[r].subroworigin+subrows[r].numsite){
		c.x = subrows[r].subroworigin+subrows[r].numsite-c.w;
	}
	if(c.idx>=1){
		cluster c_prev = subrows[r].clusters[c.idx-1];
		if(c_prev.x+c_prev.w > c.x){
			c_prev = add_cluster(c, c_prev);
			subrows[r].clusters[c_prev.idx] = c_prev;
			subrows[r].clusters.erase(subrows[r].clusters.begin() + c.idx);
			for(int i=0; i<subrows[r].clusters.size();i++){
				subrows[r].clusters[i].idx = i;
			}
		}
		collapse(c_prev, r);
		
	}
	return;
}

float place_row(node cell, int r, bool write){	
	vector<cluster> backup;
	if(!write) backup = subrows[r].clusters;
	if(subrows[r].clusters.empty()){
		cluster c;
		c.idx = 0;
		c.e = c.q = c.w = 0;
		c.x = (int) cell.x+0.5;
		if(c.x < subrows[r].subroworigin){
			c.x = subrows[r].subroworigin;
		}else if(c.x+c.w > subrows[r].subroworigin+subrows[r].numsite){
			c.x = subrows[r].subroworigin+subrows[r].numsite-c.w;
		}
		c = add_cell(c, cell);
		subrows[r].clusters.push_back(c);
	}else{
		cluster c = subrows[r].clusters.back();
		cluster c2;
		c2.e = c2.q = c2.w = 0;
		c2.idx = c.idx+1;
		c2.x = (int) cell.x+0.5;
		c2 = add_cell(c, cell);
		subrows[r].clusters.push_back(c2);
		collapse(c2, r);
//		if(c.x+c.w <= cell.x){
//			cluster c2;
//			c2.e = c2.q = c2.w = 0;
//			c2.idx = c.idx+1;
//			c2.x = (int) cell.x+0.5;
//			c2 = add_cell(c, cell);
//			if(c2.x < subrows[r].subroworigin){
//				c2.x = subrows[r].subroworigin;
//			}else if(c2.x+c2.w > subrows[r].subroworigin+subrows[r].numsite){
//				c2.x = subrows[r].subroworigin+subrows[r].numsite-c2.w;
//			}
//			subrows[r].clusters.push_back(c2);
//			if(c2.x < c.x+c.w){
//				collapse(c2, r);
//			}
//			
//		}else{
//			c = add_cell(c, cell);
//			subrows[r].clusters[c.idx] = c;
//			collapse(c, r);
//		}
	}
	float cost=0;
	if(!write){
		subrows[r].clusters.clear();
		subrows[r].clusters = backup;	
	}else{
		subrows[r].w += cell.w;
	} 
	return cost;	
}

void Abacus(){
	set<node, CustomCompare> cells;
	for(auto i=blocks.begin(); i!=blocks.end(); i++){
		cells.insert(*i->second);	
	}
	if(cells.size() != blocks.size()) cout<<"Size error: "<<cells.size()<<" "<<blocks.size()<<endl;
	int k=0;
	for(auto c=cells.begin(); c!=cells.end(); c++){
		cout<<"\r"<<k++<<"            ";
		double cost = 10000000.0, tmp_cost=0;
		int rbest = 0;
		node cell = *c;
		//int start = -1, end = -1;
		for(int r=0; r<subrows.size(); r++){
		//	cout<<"r: "<<r<<" "<<subrows[r].w<<endl;
			if( subrows[r].w+cell.w<=subrows[r].numsite){//abs(subrows[r].coordinate - cell.y) < 1.5*cell.h &&
				//tmp_cost = place_row(cell, r, false);	
				tmp_cost = abs(cell.x - subrows[r].subroworigin-subrows[r].w)+ abs(cell.y - subrows[r].coordinate);
			//	cout<<tmp_cost<<endl;
				if(tmp_cost < cost){
				//	cout<<"in"<<endl;
					cost = tmp_cost;
					rbest = r;
				}
			} 
		}
		//cout<<"rbest: "<<rbest<<endl;
		place_row(cell, rbest, true);
	}
	return;
}




int main(void){
	FILE* aux = fopen("../testcase/adaptec1/adaptec1.aux", "r");
	read_aux(aux);
	read_node();
	read_pl();
	read_scl();
	
	
	generate_subrows();
	Abacus();
	
	for(int i=0;i<subrows.size();i++){
		
		for(int j=0; j<subrows[i].clusters.size(); j++){
			int x=0;
			x = subrows[i].clusters[j].x;
			for(auto it=subrows[i].clusters[j].cells.begin(); it!=subrows[i].clusters[j].cells.end();it++){
				blocks[*it]->x = x;
				blocks[*it]->y = subrows[i].coordinate;
				x += blocks[*it]->w;
			}
		}
	}
	
	ofstream output;
	output.open("./out.txt");
	output<<"numBlock: "<<" "<<blocks.size()+terminals.size()<<endl;
	for(auto i=blocks.begin(); i!=blocks.end();i++){
		output<<i->second->name<<" "<<i->second->x<<" "<<i->second->y<<" "<<i->second->w<<" "<<i->second->h<<"\n";
	}
//	output<<"numBlock: "<<" "<<subrows.size()+terminals.size()<<endl;
//	for(auto i=0; i<subrows.size();i++){
//		output<<i<<" "<<subrows[i].subroworigin<<" "<<subrows[i].coordinate<<" "<<subrows[i].numsite<<" "<<subrows[i].height<<"\n";
//	}
	for(auto i=terminals.begin(); i!=terminals.end();i++){
		output<<i->second->name<<" "<<i->second->x<<" "<<i->second->y<<" "<<i->second->w<<" "<<i->second->h<<"\n";
	}
	
	return 0;	
}
	
