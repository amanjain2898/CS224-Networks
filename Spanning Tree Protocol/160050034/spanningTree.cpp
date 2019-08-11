#include <bits/stdc++.h>
using namespace std;

#define max 10

class message{
public:
	int sender;
	char l;
	int considers_Root;
	int distance;
	pair<char,int> send_port;
	message(int x,char l1,int y,int z,pair<char,int> m){
		sender = x;
		l = l1;
		considers_Root = y;
		distance = z;
		send_port = m;
	}
	message(){
		sender = -1;
		considers_Root = -1;
		distance = -1;
	}
};
	
class bridge{
public:
	int root_distance;
	int considered_root;
	int id;
	message best_config;
	pair<char,int> root_port;
	vector<message> m;
	vector<pair<char,int> > adj_bridges;
	vector<pair<char,int> > d_ports;
	vector<pair<char,int> > n_ports;
	bridge(){
		id = -1;
		root_distance = 0;
		considered_root = -1;
		root_port.first = '\0';
	}
};

class lan{
public:
	char id;
	vector<pair<char,int> > dp;
	vector<pair<char,int> > np;
	vector<pair<char,int> > rp;
	vector<string> hosts;
	vector<int> adj_bridges;
	lan(){
		id = '0';
	}
};

// functions

bool member(int m,vector<int> l){
	if(l.size() == 0) return false;
	for(int i=0;i<l.size();i++){
		if(m == l[i]){
			return true;
		}
		else continue;
	}
	return false;
}


int main(){
	int trace_flag,n;
	cin >> trace_flag;
	cin >> n;
	char k;
	bridge b[max];
	vector<int> l1[26];// bridges connected to each lan
	lan ln[26]; // stores the ports


	// defining the bridges initially
	for(int i=1;i<n+1;i++){
		b[i].id = i;
		b[i].considered_root = i;
		b[i].root_distance = 0;
	}

	for(int i=1;i<n+1;i++){
		string trash;
		cin >> trash;
		while(getchar() != '\n'){
			char lan_id;
			cin >> lan_id;
			if(!member(i,l1[lan_id-65])){				
				l1[lan_id-65].push_back(i);
				ln[lan_id-65].adj_bridges.push_back(i);
			}
			b[i].d_ports.push_back(make_pair(lan_id,i));
		}
	}

	// taking input.. for now excluding B1,B2 in the input format
	// for(int i=0;i<n+1;i++){
	// 	string line;
	// 	getline(cin,line);
	// 	for(int j=0;j<line.length();j++){
	// 		if(j%2 == 0){ 										// take only non blank spaces as input
	// 			if(!member(i,l1[line[j]-65])){				
	// 				l1[line[j]-65].push_back(i);
	// 				ln[line[j]-65].adj_bridges.push_back(i);
	// 			}
	// 			b[i].d_ports.push_back(make_pair(line[j],i));
	// 		}
	// 	}
	// }

	// filling lan states
	for(int i=0;i<26;i++){
		for(int j=0;j<l1[i].size();j++){
			ln[i].dp.push_back(make_pair(i+65,l1[i][j]));
			ln[i].id = i+65;
		}
	}

	// cout << ln[4].np.size() << endl;

	for(int i=0;i<26;i++){
		for(int j=0;j<l1[i].size();j++){
			for(int k=0;k<l1[i].size();k++){
				if(k!=j){
					int br = l1[i][k];
					char c = i+65;
					b[l1[i][j]].adj_bridges.push_back(make_pair(c,br));
				}
			}
		}
	}

	// cout << b[4].n_ports.size() << " " << b[4].d_ports.size() << endl;

	message temp[10];
	for(int t=0;t<10;t++){
		temp[t].sender = t;
		temp[t].distance = 0;
		temp[t].considers_Root = t;
	}

	for(int i=0;i<10;i++){
		for(int j=0;j<10;j++){

			// if(j == 4){
			// 	for(int k=0;k<b[j].m.size();k++){
			// 		cout << b[j].m[k].sender << " " << b[j].m[k].considers_Root << " " << b[j].m[k].l << " " << b[j].m[k].distance << " " << b[j].m[k].send_port.first << b[j].m[k].send_port.second << endl;
			// 	}
			// }

			for(int k=0;k<b[j].m.size();k++){
				if((b[j].m[k].considers_Root < b[j].considered_root) || ((b[j].m[k].considers_Root == b[j].considered_root) && (b[j].m[k].distance < b[j].root_distance)) || ((b[j].m[k].considers_Root == b[j].considered_root) && (b[j].m[k].distance == b[j].root_distance) && (b[j].m[k].sender < temp[j].sender))){


					b[j].considered_root = b[j].m[k].considers_Root;
					// cout << j << ": " << b[j].considered_root << endl;
					b[j].root_distance = b[j].m[k].distance;


					vector<pair<char,int> >::iterator position3 = find(ln[b[j].m[k].l-65].dp.begin(),ln[b[j].m[k].l-65].dp.end(),b[j].root_port);
					vector<pair<char,int> >::iterator position4 = find(b[j].d_ports.begin(),b[j].d_ports.end(),b[j].root_port);
					if (position3 == ln[b[j].m[k].l-65].dp.end() && b[j].root_port.first != '\0'){
					    ln[b[j].m[k].l-65].dp.push_back(b[j].root_port);
					}
					if(position4 == b[j].d_ports.end() && b[j].root_port.first != '\0'){
							b[j].d_ports.push_back(b[j].root_port);
					}

					for(int n_s=0;n_s<b[j].n_ports.size();n_s++){
						b[j].d_ports.push_back(b[j].n_ports[n_s]);
					}
					b[j].n_ports.clear();

					for(int n_s=0;n_s<ln[b[j].m[k].l-65].np.size();n_s++){
						ln[b[j].m[k].l-65].dp.push_back(ln[b[j].m[k].l-65].np[n_s]);
					}
					ln[b[j].m[k].l-65].np.clear();


					b[j].root_port = b[j].m[k].send_port;




					// cout << "ab " << j << " -- " <<  b[j].root_port.first << " " << b[j].root_port.second <<endl;

					vector<pair<char,int> >::iterator position1 = find(ln[b[j].m[k].l-65].dp.begin(),ln[b[j].m[k].l-65].dp.end(),b[j].root_port);
					if (position1 != ln[b[j].m[k].l-65].dp.end()){
					    ln[b[j].m[k].l-65].dp.erase(position1);
					}
					vector<pair<char,int> >::iterator position2 = find(b[j].d_ports.begin(),b[j].d_ports.end(),b[j].root_port);
					if(position2 != b[j].d_ports.end()){
							b[j].d_ports.erase(position2);
					}

					temp[j] = b[j].m[k];

				}
				if(trace_flag == 1){
					cout << i << " " << "B" << j << " r" << " (B" << b[j].m[k].considers_Root << ", " << b[j].m[k].distance - 1 << ", B" << b[j].m[k].sender << ")" << endl;
				}
			}

			// if(j == 4){
			// 	for(int k=0;k<b[j].m.size();k++){
			// 		cout << temp[j].sender << " " << temp[j].considers_Root << " " << temp[j].l << " " << temp[j].distance << " " << temp[j].send_port.first << temp[j].send_port.second << endl;
			// 	}
			// }			

			// cout << endl;
			// for(auto it = b[4].n_ports.begin();it!= b[4].n_ports.end();it++){
			// 	auto pClass1 = it->first;
			//     auto pClass2 = it->second;
			//     cout << pClass1 << " " << pClass2 << endl;
			// }
			// if(j==4) cout << b[j].m.size() << " " << b[j].root_distance << endl;
			// cout << endl;

			// cout << b[4].considered_root << " " << b[4].root_distance << endl;


			for(int k=0;k<b[j].m.size();k++){
				if(b[j].m[k].send_port != b[j].root_port && b[j].considered_root == b[j].m[k].considers_Root){
					if((b[j].m[k].distance < (b[j].root_distance+1)) || ((b[j].m[k].distance == (b[j].root_distance + 1)) && (b[j].m[k].sender < j))){

						vector<pair<char,int> >::iterator position1 = find(b[j].d_ports.begin(),b[j].d_ports.end(),b[j].m[k].send_port);
						if(position1 != b[j].d_ports.end()){
							b[j].d_ports.erase(position1);
							b[j].n_ports.push_back(b[j].m[k].send_port);
						}

						vector<pair<char,int> >::iterator position2 = find(ln[b[j].m[k].l-65].dp.begin(),ln[b[j].m[k].l-65].dp.end(),b[j].m[k].send_port);
						if (position2 != ln[b[j].m[k].l-65].dp.end()){
						    ln[b[j].m[k].l-65].dp.erase(position2);
						    ln[b[j].m[k].l-65].np.push_back(b[j].m[k].send_port);
						}

					}
				}
			}

			// cout << j << ": " << b[j].d_ports.size() << endl;

			
			b[j].m.clear();

		}



		for(int j=0;j<10;j++){
			if(b[j].considered_root == j){
				for(auto it = b[j].adj_bridges.begin();it!= b[j].adj_bridges.end();it++){
					auto pClass1 = it->first;
				    auto pClass2 = it->second;
				    if(b[j].root_port.first != pClass1){
					    message m1 = message(j,pClass1,j,1,make_pair(pClass1,pClass2));
					    b[pClass2].m.push_back(m1);
					    // cout << pClass1 << " " << pClass2 << endl; // testing line	
					}
				}
				if(trace_flag == 1){
					cout << i << " " << "B" << j << " s" << " (B" << j << ", " << b[j].root_distance << ", B" << j << ")" << endl; 
				}
			}

			else if(b[j].considered_root > 0){
				for(auto it = b[j].adj_bridges.begin();it!= b[j].adj_bridges.end();it++){
					auto pClass1 = it->first;
				    auto pClass2 = it->second;
				    if(b[j].root_port.first != pClass1){
				    	// cout << "m2: " << j << " " << m2.distance << endl;
					    message m3;
					    m3.distance = temp[j].distance + 1;
					    m3.sender = j;
					    m3.send_port = make_pair(pClass1,pClass2);
					    m3.l = pClass1;
					    m3.considers_Root = temp[j].considers_Root;
					    b[pClass2].m.push_back(m3);
					}
				}
				if(trace_flag == 1){
					cout << i << " " << "B" << j << " s" << " (B" << temp[j].considers_Root << ", " << temp[j].distance << ", B" << j << ")" << endl;
				}
			}
		}

		// for(int j=0;j<10;j++){
		// 	cout << j <<" :  ";
		// 	for(int k=0;k<b[j].m.size();k++){
		// 		cout << b[j].m[k].considers_Root << " ";				
		// 	}
		// 	cout << endl;
		// }

	}

	for(int p = 1; p < n+1; p++){
		cout << "B" << p << ": ";
		for(int k=0;k<26;k++){
			char c = k+65;
			vector<pair<char,int> >::iterator position1 = find(b[p].d_ports.begin(),b[p].d_ports.end(),make_pair(c,p));
			vector<pair<char,int> >::iterator position2 = find(b[p].n_ports.begin(),b[p].n_ports.end(),make_pair(c,p));
			if(position1 != b[p].d_ports.end()){
				char c = k+65;
				cout << c << "-DP ";
			}
			if(position2 != b[p].n_ports.end()){
				char c = k+65;
				cout << c << "-NP ";
			}
			if(b[p].root_port.first == k+65){
				char c = k+65;
				cout << c << "-RP ";
			}
		}
		cout << endl;
	}


	/* second part  */

	int count = 0;
	int n_hosts = 0;
	map<string,char> m1;

	for(int i=0;i<26;i++){
		if(ln[i].id != '0'){
			count++;
		}
	}


	for(int i=0;i<count;i++){

		bool flag = false;
		while(getchar() != '\n'){
			if(flag = true){
				string host;
				cin >> host;
				n_hosts++;
				m1.insert(pair<string,char>(host,i+65));
				ln[i].hosts.push_back(host);
			}
			else {
				string trash;
				cin >> trash;
				flag = true;
			}
		}
	}


	map<string,pair<char,int> > fwd[20];


	int n_transfers;
	cin >> n_transfers;


	for(int L=0;L<n_transfers+1;L++){
		if(L==0){
			string line;
			string host;
			getline(cin,line);
			continue;
		}
		bool visited[max] = {false};
		string line;
		string host;
		getline(cin,line);
		string host1,host2;
		bool flag = false;
		for(int j=0;j<line.length();j++){
			if(line[j] == ' ' && !flag) {
				host1 = host;
				host = "";
				flag = true;
				continue;
			}
			host = host+line[j];
		}
		host2 = host;

		// cout << host1 << " -- " << host2 << endl;

		queue<lan> q;
		char s_lan = m1.find(host1)->second;
		char d_lan = m1.find(host2)->second;

		// cout << s_lan <<  endl;

		q.push(ln[s_lan-65]);
		while(!q.empty()){
			// cout << "yeah" << L << endl;
			lan l = q.front();
			// cout << l.id << " " << q.size() << endl;
			q.pop();
			// cout << "yo" << endl;
			// cout << l.adj_bridges.size() << endl;
			for(int i=0;i<l.adj_bridges.size();i++){
				vector<pair<char,int> >::iterator position = find(ln[l.id - 65].np.begin(),ln[l.id - 65].np.end(),make_pair(l.id,l.adj_bridges[i]));
				if(position == ln[l.id - 65].np.end()){
					if(visited[l.adj_bridges[i]] == false){
						if(fwd[l.adj_bridges[i]].find(host2) == fwd[l.adj_bridges[i]].end()){
							visited[l.adj_bridges[i]] = true;
							fwd[l.adj_bridges[i]].insert(pair<string,pair<char,int> >(host1,make_pair(l.id,l.adj_bridges[i])));
							for(int j=0;j<b[l.adj_bridges[i]].adj_bridges.size();j++){
								if(visited[(b[l.adj_bridges[i]].adj_bridges[j]).second] == false){
									vector<pair<char,int> >::iterator position1 = find(b[l.adj_bridges[i]].n_ports.begin(),b[l.adj_bridges[i]].n_ports.end(),make_pair((b[l.adj_bridges[i]].adj_bridges[j]).first,l.adj_bridges[i]));
									
									if(position1 == b[l.adj_bridges[i]].n_ports.end()){
										lan l1 = ln[b[l.adj_bridges[i]].adj_bridges[j].first - 65];
										if(l1.id != l.id)	q.push(l1);
									}
								}
							}
						}
						else{
							visited[l.adj_bridges[i]] = true;
							pair<char,int> port = (fwd[l.adj_bridges[i]].find(host2))->second;
							fwd[l.adj_bridges[i]].insert(pair<string,pair<char,int> >(host1,make_pair(l.id,l.adj_bridges[i])));
							if(port.first != l.id) q.push(ln[port.first - 65]);
						}
					}
				}	

			}
		}

		map<string,pair<char,int> >::iterator itr;

		for(int p = 1;p < n+1; p++){
			cout << "B" << p << ":" << endl;
			cout << "HOST ID | FORWARDING PORT" << endl;
			for(itr = fwd[p].begin(); itr != fwd[p].end(); itr++){
				cout << itr->first << " | " << (itr->second).first << endl;
			}
		}



	}

	// cout << (fwd[5].find("H4")->second).first << " " <<  (fwd[5].find("H4")->second).second << endl;




	// for(int i=0;i<ln[1].hosts.size();i++){
	// 	cout << ln[1].hosts[i] << " ";
	// }


    /* end */




	/*  testing   */

	// for(auto it = ln[4].dp.begin();it!= ln[4].dp.end();it++){
	//     auto pClass1 = it->first;
	//     auto pClass2 = it->second;
	//     cout << pClass1 << " " << pClass2 << endl;
	// }


	// for(auto it = b[5].d_ports.begin();it!= b[5].d_ports.end();it++){
	// 	auto pClass1 = it->first;
	//     auto pClass2 = it->second;
	//     cout << pClass1 << " " << pClass2 << endl;
	// }

	// cout << b[4].root_port.first << " " << b[4].root_port.second << endl;



	// for(int i=0;i<26;i++){
	// 	for(int j=0;j<l1[i].size();j++){
	// 		cout << l1[i][j] << " ";
	// 	}
	// 	cout << endl;	
	// }


	// cout << b[1].considered_root << endl;
	// cout << b[2].considered_root << endl;
	// cout << b[3].considered_root << endl;
	// cout << b[4].considered_root << endl;
	// cout << b[5].considered_root << endl;
	// cout << b[0].considered_root << endl;
	// cout << b[6].considered_root << endl;


	/* testing */ 
}