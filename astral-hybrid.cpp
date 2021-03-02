#include<iostream>
#include<fstream>
#include<unordered_map>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<string>

using namespace std;

//#define LARGE_DATA
#ifdef LARGE_DATA
typedef long double score_t;

string to_string(long double x){
	return to_string((double) x);
}

score_t from_string(const string s){
	return stold(s);
}

ostream& operator<<(ostream& cout, long double x){
	return cout << to_string(x);
}
#else
typedef double score_t;

score_t from_string(const string s){
	return stod(s);
}
#endif

#include "genetreewithhybridweight.hpp"
#include "algorithms.hpp"


MetaAlgorithm meta;
TripartitionInitializer &tripInit = meta.tripInit;
vector<TripartitionInitializer> &batchInit = meta.batchInit;

unordered_map<string, string> leafname_mapping;
string TEXT;
int pos = 0;
int K = 0;
int part = 0, iBatch = 0;
vector<string> &names = meta.names;
unordered_map<string, int> &name2id = meta.name2id;
score_t maxv = 100, minv = 0, defaultv = 0;

int MAPPING(int begin, int end){
	string s;
	for (int i = begin; i < end && TEXT[i] != ':'; i++){
		if (TEXT[i] != '\"' && TEXT[i] != '\'') s += TEXT[i];
	}
	if (leafname_mapping.count(s)) s = leafname_mapping[s];
	if (name2id.count(s) == 0){
		name2id[s] = names.size();
		names.push_back(s);
		for (int i = 0; i < meta.nThread2; i++){
			tripInit.leafParent[i].emplace_back();
		}
	}
	return name2id[s];
}

score_t WEIGHT_S(int begin, int end){
	int i = begin;
	while (i < end && TEXT[i] != ':') i++;
	if (i == begin || i == end) return max(0.0, (defaultv - minv) / (maxv - minv));
	else return max(0.0, (from_string(TEXT.substr(begin, i - begin)) - minv) / (maxv - minv));
}

score_t WEIGHT_L(int begin, int end){
	int i = begin;
	while (i < end && TEXT[i] != ':') i++;
	if (i == end) return 1;
	else return exp(-from_string(TEXT.substr(i + 1, end - i - 1)));
}

void parse(int parent = -1, bool isLeft = true){
	int cur = tripInit.nodes[part].size();
	tripInit.nodes[part].emplace_back();
	tripInit.nodes[part][cur].up = parent;
	if (parent != -1 && isLeft) tripInit.nodes[part][parent].small = cur;
	if (parent != -1 && !isLeft) tripInit.nodes[part][parent].large = cur;
	
	if (TEXT[pos] == '(') { 
		pos++;
		parse(cur, true); 
		pos++;
		parse(cur, false); 
		while (TEXT[pos] != ')'){
			tripInit.nodes[part][cur].weight = max(0.0, (defaultv - minv) / (maxv - minv));
			tripInit.nodes[part][cur].length = 1;
			int next = tripInit.nodes[part].size();
			tripInit.nodes[part].emplace_back();
			tripInit.nodes[part][cur].up = next;
			tripInit.nodes[part][next].small = cur;
			cur = next;
			tripInit.nodes[part][cur].up = parent;
			if (parent != -1 && isLeft) tripInit.nodes[part][parent].small = cur;
			if (parent != -1 && !isLeft) tripInit.nodes[part][parent].large = cur;
			pos++;
			parse(cur, false);
		}
		int i = ++pos;
		while (TEXT[pos] != ')' && TEXT[pos] != ',' && TEXT[pos] != ';') pos++;
		tripInit.nodes[part][cur].weight = WEIGHT_S(i, pos);
		tripInit.nodes[part][cur].length = WEIGHT_L(i, pos);
	} 
	else {
		int i = pos;
		while (TEXT[pos] != ')' && TEXT[pos] != ',') pos++;
		tripInit.leafParent[part][MAPPING(i, pos)].push_back(cur);
		tripInit.nodes[part][cur].weight = 1;
		tripInit.nodes[part][cur].length = WEIGHT_L(i, pos);
	}
}

void readInputTrees(string input, string mapping) {
	if (mapping != ""){
		ifstream fmap(mapping);
		string gname, sname;
		while (fmap >> gname){
			fmap >> sname;
			leafname_mapping[gname] = sname;
		}
	}
	ifstream fin(input);
	string line;
	while (getline(fin, line)) TEXT += line;
	while (pos < TEXT.size()){
		while (pos < TEXT.size() && TEXT[pos] != '(') pos++;
		if (pos < TEXT.size()) {
			part = K % tripInit.nodes.size();
			parse();
			K++;
		}
	}
}

string HELP = " -a taxonNameMaps -x maxWeight -n minWeight -d defaultWeight";
string HELP_TEXT = R"V0G0N(-a  a list of gene name to taxon name maps, each line contains one gene name followed by one taxon name separated by a space or tab
-x max possible weight in weight scale (default: 100)
-n min possible weight in weight scale (default: 0)
-d min default weight when weight not provided (default: 0)
inputGeneTrees: the path to a file containing all gene trees in Newick format
)V0G0N";

int main(int argc, char** argv){
	string mappingFile;
	meta.initialize(argc, argv, HELP, HELP_TEXT);
	
	for (int i = 1; i < argc; i += 2){
		if (strcmp(argv[i], "-a") == 0) mappingFile = argv[i + 1];
		if (strcmp(argv[i], "-x") == 0) maxv = from_string(argv[i + 1]);
		if (strcmp(argv[i], "-n") == 0) minv = from_string(argv[i + 1]);
		if (strcmp(argv[i], "-d") == 0) defaultv = from_string(argv[i + 1]);
	}
	
	for (int i = 0; i < meta.nThread2; i++){
		tripInit.nodes.emplace_back();
		tripInit.leafParent.emplace_back();
	}
	for (int i = 0; i < meta.nBatch; i++){
		batchInit[i].nodes.emplace_back();
		batchInit[i].leafParent.emplace_back();
	}
	readInputTrees(argv[argc - 1], mappingFile);
	
	cerr << "#Genetrees: " << K << endl;
	
	score_t score = meta.run().first / 2;
	cerr << "Score: " << score << endl;
	return 0;
}
