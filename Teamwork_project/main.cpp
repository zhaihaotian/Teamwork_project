#include<bits/stdc++.h>
using namespace std;
#define fer(i , a , b , c) for(int i = a ; i <= b ; i+= c)
#define fe(i , a, b , c) for(int i = a ; i >= b ; i -= c) 
const int maxn = 10000 ; 
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
unordered_map<string, double> v , change_tim; 
double a, b, xx, yy;
int id_cnt = 481; 

struct nod{
	int num ; 
	double tim ; 
	bool operator < (const nod &x ) const{
		return tim > x.tim ; 
	}
};

struct Graph{
	double head[maxn] , to[maxn] , tim[maxn] , nxt[maxn]; 
	int cnt ; 
	void add(int u , int v , double t){
		to[++cnt] = v; nxt[cnt] = head[u] , head[u] = cnt , tim[cnt] = t ;
	}
}G , GG;

double change_k = 8  ; 
double ka = 1 , kb = 1 ; 

void build_in(string inp , double x ){
	change_tim[inp] = x ; 
	for(int j = 0 ; j < mp[inp].size() ; j++)
			for(int k = j + 1 ; k < mp[inp].size(); k++){
				G.add(mp[inp][j] , mp[inp][k] , x) ; 
				G.add(mp[inp][k] , mp[inp][j] , x) ; 
				GG.add(mp[inp][j] , mp[inp][k] , x * change_k) ; 
				GG.add(mp[inp][k] , mp[inp][j] , x * change_k) ; 
			}
	return ; 
}

void Changetim(){
	freopen("change_time.txt","r",stdin);
	int get_num[8] = {0,5,21,25,10,12,9,1} ; 
	fer(i,1,7,1){
		double x ; cin >> x ; x *= 60 ; 
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
	for (int i = 1; i <= id_cnt ; i++)
    {
        cin >> s1 >> s2 >> a >> b;
		mpp[i] = (node){a , b , s2 , s1} ;  
        mp[s2].push_back(i) ; 
        if (s1 == temp){
        	dis[last][i] = dis[i][last] = sqrt((a - xx) * (a - xx) + (b - yy) * (b - yy));
        	tim[last][i] = tim[i][last] = 1.0*dis[last][i]/(v[s1]); 
        	G.add(i , last , tim[last][i]) ; GG.add(i , last , tim[last][i]) ;
			G.add(last , i , tim[last][i]) ; GG.add(last , i , tim[last][i]) ;
			// printf("i : %d last : %d \n" , i , last) ; 
		}
        else
            temp = s1;
        last = i , xx = a, yy = b;
    }
	fclose(stdin);
	cin.clear();
}


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

double query_type_1(string st , string ed , bool print_ ){
	// cout << "St : " << st << "ed : " << ed << endl ; 
	if(st == ed){
		if(print_){
			cout << "从"  << st << "到"  << st << endl ; 
			cout << "耗时0h0min" <<endl <<endl ; 
		}
		return 0 ; 
	}
	double to_tim[maxn] ; int vis[maxn] , pre[maxn] ; 
	for(int i = 0 ; i < mp[st].size() ; i++){
		path[i].clear() ; 
		priority_queue<nod> Q ; 
		fer(j,1,id_cnt,1){
			to_tim[j] = 99999999 , vis[j] = 0 ;
		}
		int st_id = mp[st][i] ; 
		to_tim[st_id] = 0 ; 
		// printf("st_id : %d \n" , st_id ) ; 
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
		fer(j,1,id_cnt,1){
			vis[j] = 0 ; 
		}
		 int noww = minn_id ; 
		 path[i].push_front(noww) ; 
		 vis[noww] = 1 ; 
		 while(noww != st_id){
		 	noww = pre[noww] ; 
			if(noww == 0 || vis[noww]){
				las_tim[i] = 99999999 ; 
				break ; 
			}
			vis[noww] = 1 ; 
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
	if(print_ == 0 || min_tim == 99999999){
		return min_tim ; 
	}
	// printf("min_tim : %lf\n" , min_tim) ; 
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
	int H=min_tim/3600;
	int M=(min_tim-H*3600)/60;
	cout << "预计花费" << H << "h" << M << 'm' << endl ; 
	printf("\n") ; 
	return 0 ; 
}

void query_type_2(string st , string ed){
	double to_tim[maxn] ; int vis[maxn] , pre[maxn] ; 
	for(int i = 0 ; i < mp[st].size() ; i++){
		path[i].clear() ; 
		priority_queue<nod> Q ; 
		fer(j,1,id_cnt,1){
			to_tim[j] = 99999999 , vis[j] = 0 ; pre[j] = 0 ; 
		}
		int st_id = mp[st][i] ; 
		to_tim[st_id] = 0 ; 
		Q.push((nod){st_id , 0}) ; 
 		while(!Q.empty()){
			nod now = Q.top() ; 
			Q.pop() ; 
			if(vis[now.num]) continue ;  
			vis[now.num] = 1; 
			for(int ii = GG.head[now.num] ; ii ; ii = GG.nxt[ii]){
				int y = GG.to[ii] ; 
				if(to_tim[y] > now.tim + GG.tim[ii] ){
					to_tim[y] = now.tim + GG.tim[ii] ; 
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
		 fer(j,1,id_cnt,1){
			vis[j] = 0 ; 
		}
		 int noww = minn_id ; 
		 path[i].push_front(noww) ; 
		 vis[noww] = 1 ; 
		 while(noww != st_id){
		 	noww = pre[noww] ; 
			if(noww == 0 || vis[noww]){
				las_tim[i] = 99999999 ; 
				break ; 
			}
			vis[noww] = 1 ; 
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
	int zz = 0 ; 
	while(path[min_id].size()){
		lasans[++zz] = path[min_id].front() ; 
		path[min_id].pop_front() ; 
	}
	// printf("min_tim : %lf \n"  ,min_tim) ; 
	cout << "从" << change(mpp[lasans[1]].route) << "的" << mpp[lasans[1]].nam<< "开始" << endl ;  
	for(int i = 2 ; i <= zz ; i++){
		if(i == zz){
			cout <<  "最终到达" <<  mpp[lasans[i]].nam<< endl ; 
			break ; 
		}
		if(mpp[lasans[i]].nam == mpp[lasans[i-1]].nam){
			cout << "在" << mpp[lasans[i]].nam << "换乘" <<  change(mpp[lasans[i]].route) << endl ; 
			min_tim -= change_tim[mpp[lasans[i]].nam] * (change_k - 1) ; 
			// cout << mpp[lasans[i]].nam << endl ; 
			// printf("change_tim : %lf \n" , change_tim[mpp[lasans[i]].nam] ) ; 
		}
		else cout << "途径" <<  mpp[lasans[i]].nam << endl ; 
	}
	/**/
	// int cntt = 0 ; 
	// printf("zz : %lld \n" , zz) ; 
	// for(int i = zz ; i >= 2 ; i--){
	// 	if(mpp[lasans[i]].nam == mpp[lasans[i-1]].nam)
	// 		cntt++ , printf("i : %d \n" , i); 
	// 	if(cntt == 2){
	// 		cout << mpp[lasans[i]].nam << endl ; 
	// 		printf("change_tim : %lf \n" , change_tim[mpp[lasans[i]].nam] ) ;  
	// 		min_tim += change_tim[mpp[lasans[i]].nam] * (change_k - 1) ; 
	// 		break ; 
	// 	}
	// } 
	int H=min_tim/3600;
	int M=(min_tim-H*3600)/60;
	cout << "预计花费" << H << "h" << M << 'm' << endl ; 
	printf("\n") ; 
	return ; 
}
void query_type_3(string q1 , string q2 , string q3){
	double minn_val = 99999999 ; 
	string zhong ; 
	for(int i = 1 ; i <= id_cnt ; i++){
		// cout << mpp[i].nam << endl ; 
		// printf("i : %d \n" , i) ; 
		// cout << q1 << " " << q2  << " " << q3 << endl ; 
		// query_type_1(q1 , mpp[i].nam , 1) ;
		// cout << "yes" ;  
		// query_type_1( mpp[i].nam  ,q2  , 1) ; 
		// cout << "yes" ; 
		// query_type_1( mpp[i].nam  ,q3, 1) ; 
		// cout << "yes" ; 
		double now_val = ka * query_type_1(q1 , mpp[i].nam , 0) + kb * query_type_1( mpp[i].nam  ,q2  , 0) + kb * query_type_1( mpp[i].nam  ,q3, 0) ; 
		if(now_val < minn_val) 
			minn_val = now_val , zhong = mpp[i].nam ; 
	}
	cout << "一起走的路程： " << endl ; 
	query_type_1(q1 , zhong , 1) ; 
	cout << "两人以后分别走的路程： " << endl ; 
	query_type_1(zhong , q1 , 1) ;
	query_type_1(zhong , q2 , 1) ;
}
void read_query(){
	freopen("read.in" , "r" , stdin) ; 
	int kind ; 
	while(scanf("%d",&kind) != EOF ){
		if(kind == 1){
			string q1 , q2 ; 
			cin >> q1 >> q2 ; 
			query_type_1(q1 , q2 , 1) ; 
		}
		if(kind == 2){
			string q1 , q2 ; 
			cin >> q1 >> q2 ; 
			query_type_2(q1 , q2) ; 
		}
		if(kind == 3){
			// cout << "yes" << endl ; 
			string q1 , q2 , q3; 
			cin >> q1 >> q2 >> q3 ; 
			query_type_3(q1 , q2 , q3) ; 
		}
	}
}
int main()
{
	freopen("ans.out", "w", stdout);
	Pace();
	read_Undergound() ; 
	Changetim();
	// printf("cnt : %d \n" , G.cnt ) ; 
	// for(int i = 1 ; i <= G.cnt ; i++)
	// 	printf("to : %d \n" , G.to[i]) ; 
	// for(int i = G.head[101] ; i ; i = G.nxt[i]){
	// 	printf("to : %d \n" , i ) ; 
	// }
	read_query() ; 
    return 0;
}
