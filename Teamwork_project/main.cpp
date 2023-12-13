#include<bits/stdc++.h>
using namespace std;
#define fer(i , a , b , c) for(int i = a ; i <= b ; i+= c)
#define fe(i , a, b , c) for(int i = a ; i >= b ; i -= c) 
const int maxn = 5000 ; 
struct node
{
    double x, y;       // 类似坐标，每两点之间的欧几里得距离为两站之间直线距离，相邻站点可以用来近似计算边权
	string nam ;       //  这个站点的名字   
    string route; 
}mpp[maxn];

unordered_map<string, vector<int> > mp;                           
unordered_map<int, unordered_map<int, double>> dis , tim; 
string s1, s2, temp = "0";
int last;
unordered_map<string, double> v; 
double a, b, xx, yy;
int id_cnt = 477; 

struct Graph{
	double head[maxn] , to[maxn] , tim[maxn] , nxt[maxn]; 
	int cnt ; 
	void add(int u , int v , double t){
		to[++cnt] = v; nxt[cnt] = head[u] , head[u] = cnt , tim[cnt] = t ;
	}
}G;

void build_in(string inp , double x ){
	for(int j = 0 ; j < mp[inp].size() ; j++)
			for(int k = j+ 1 ; k < mp[inp].size(); k++){
				G.add(mp[inp][j] , mp[inp][k] , x) ; 
				G.add(mp[inp][k] , mp[inp][j] , x) ; 
			}
	return ; 
}

void Changetim(){
	freopen("change_time.txt","r",stdin);
	int get_num[8] = {0,5,21,23,10,12,9,1} ; 
	fer(i,1,7,1){
		int x ; cin >> x ; x *= 60 ; 
		fer(j,1,get_num[i],1){
			string inp;
			cin>>inp;
			build_in(inp , x) ; 
		}
	}	
	fclose(stdin);
	cin.clear();
}

void Pace(){
	freopen("pace.in","r",stdin);
	for(int i=1;i<=25;i++){
		string a;
		double b;
		cin>>a>>b;
		v[a]=b*0.277777778;
	}
	fclose(stdin);
	cin.clear();
}

void read_Undergound(){
	freopen("Undergound.in", "r", stdin);
	for (int i = 1; i <= 477; i++)
    {
        cin >> s1 >> s2 >> a >> b;
		mpp[i] = (node){a , b , s2 , s1} ;  
        mp[s2].push_back(i) ; 
        if (s1 == temp){
        	dis[last][i] = dis[i][last] = sqrt((a - xx) * (a - xx) + (b - yy) * (b - yy));
        	tim[last][i] = tim[i][last] = 1.0*dis[last][i]/(v[s1]); 
        	G.add(i , last , tim[last][i]) ;
			G.add(last , i , tim[last][i]) ;  
		}
        else
            temp = s1;
        last = i , xx = a, yy = b;
    }
	fclose(stdin);
	cin.clear();
}

struct nod{
	int num ; 
	double tim ; 
	bool operator < (const nod &x ) const{
		return tim > x.tim ; 
	}
};

deque<int> path[25] ; 
double las_tim[25] ; 
int lasans[maxn] ; 

bool isOnlyDigits(const std::string &str) {
    for (char ch : str) {
        if (!isdigit(ch)) {
            return false; 
        }
    }
    return true; 
}

string change(string route){
	if(isOnlyDigits(route)){
		return route + "号线" ; 
	}
	else return route ; 
}

void query_type_1(string st , string ed){
	double to_tim[maxn] ; int vis[maxn] , pre[maxn] ; 
	for(int i = 0 ; i < mp[st].size() ; i++){
		path[i].clear() ; 
		priority_queue<nod> Q ; 
		fer(j,1,id_cnt,1){
			to_tim[j] = 99999999 , vis[j] = 0 ;
		}
		int st_id = mp[st][i] ; 
		to_tim[st_id] = 0 ; 
		Q.push((nod){st_id , 0}) ; 
		while(!Q.empty()){
			nod now = Q.top() ; 
			Q.pop() ; 
			if(vis[now.num]) continue ;  
			vis[now.num] = 1; 
			for(int ii = G.head[now.num] ; ii ; ii = G.nxt[ii]){
				int y = G.to[ii] ; 
				if(to_tim[y] > now.tim + G.tim[ii] ){
					to_tim[y] = now.tim + G.tim[ii] ; 
					if(!vis[y]){
						Q.push((nod){y , to_tim[y]}) ; 
					}
					pre[y] = now.num ; 
				}
			}
 		}
 		double minn_tim = 99999999 ; int minn_id  ;
 		for(int j = 0 ; j < mp[ed].size() ; j++){
 			if(minn_tim > to_tim[mp[ed][j]]){
 				minn_id = mp[ed][j] ; 
 				minn_tim = to_tim[mp[ed][j]] ; 
			 }
		 }
		 int noww = minn_id ; 
		 path[i].push_front(noww) ; 
		 while(noww != st_id){
		 	noww = pre[noww] ; 
		 	path[i].push_front(noww) ; 
		 }
		las_tim[i] = minn_tim ; 
	}
	int min_id ; double min_tim = 99999999 ; 
	for(int i = 0 ; i < mp[st].size() ; i++){
		if(las_tim[i] < min_tim){
			min_tim = las_tim[i]; 
			min_id = i ; 
		}
	}
	int H=min_tim/3600;
	int M=(min_tim-H*3600)/60;
	cout << "预计花费" << H << "h" << M << 'm' << endl ; 
	int zz = 0 ; 
	while(path[min_id].size()){
		lasans[++zz] = path[min_id].front() ; 
		path[min_id].pop_front() ; 
	}
	cout <<  "从" << change(mpp[lasans[1]].route) << "的" << mpp[lasans[1]].nam<< "开始" << endl ;  
	for(int i = 2 ; i <= zz ; i++){
		if(i == zz){
			cout <<  "最终到达" <<  mpp[lasans[i]].nam<< endl ; 
			break ; 
		}
		if(mpp[lasans[i]].nam == mpp[lasans[i-1]].nam){
			cout << "在" << mpp[lasans[i]].nam << "换乘" <<  change(mpp[lasans[i]].route) << endl ; 
		}
		else cout << "途径" <<  mpp[lasans[i]].nam << endl ; 
	}
	printf("\n") ; 
	return ; 
}
void get_query(){

}
int main()
{
//	freopen("demo.out", "w", stdout);
	Pace();
	read_Undergound() ; 
	Changetim();
//	freopen("read.txt" , "r" , stdin) ; 
	string q1 , q2 ; 
	cin >> q1 >> q2 ; 
	query_type_1(q1 , q2) ; 
    return 0;
}
