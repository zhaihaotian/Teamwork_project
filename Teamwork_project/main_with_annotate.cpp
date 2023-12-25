/*
读入的过程中会逐步读入坐标，名字，所属的线路
在输出方案的时候需要考虑到换乘的输出 我们的程序需要知道怎么一会在这条线 一会在哪条线 
所以实际上所有点都有一个所属的线路 对于同一个名字的站点实际上由于其有多条线路，所以我们会把它变成多个点 
并且在这些点之间连接我们的换乘时间 那么最后记录路径的时候我们发现前面的点所属的路线和现在的不一样了 就可以输出要在这里换乘了 
那么建模建好了之后 我们就可以完成导航的最基础功能 最短时间以及路线的输出 使用dijkstra算法 在松弛的时候记录一下pre 就可以输出路径了
因此功能1就完成了 至于少换乘 其实我们只需要把换乘时间乘上一个系数 那么在找最短路的时候就会少换乘了 最后输出时间的时候记得减掉（系数-1）*换乘时间 功能2完成
功能3 我们记录一个val值=两人一起走的路程*a + 分别走的路程*b 然后我们枚举分离点 使得val值最小 也就满足了既有一起走的路又有分开的路 并且当a/b越大 一起走的路的时间越少 
功能4 依次递归调用功能1 
功能5 以全排列枚举中间循序 选取时间最短的一种方案
功能6 在dijkstra的预处理截断提前让哪些停运的站的vis数组=1 ， 这样这些点永远都不会进队，也不会被遍历 
功能7 计算费用 还是最短路 但是不需要考虑换乘 但是为了写的方面 图其实和别的形状一样 不过把换乘时间换成0了
完成代码文件夹在https://github.com/zhaihaotian/Teamwork_project 
命令: git clone git@github.com:zhaihaotian/Teamwork_project.git 
*/
#include<bits/stdc++.h>
using namespace std;
#define fer(i , a , b , c) for(int i = a ; i <= b ; i+= c)
#define fe(i , a, b , c) for(int i = a ; i >= b ; i -= c) 
const int maxn = 10000 ; 
struct node
{
    double x, y;       // 类似坐标，每两点之间的欧几里得距离为两站之间直线距离，相邻站点可以用来近似计算边权
	string nam ;       //  这个站点的名字   
    string route; // 所在线路 
}mpp[maxn];

unordered_map<string, vector<int> > mp;       // 存了同一个名字的所有的站点 比如a站 在1号线的a站的点的编号   在2号线的a站的点的编号          
unordered_map<int, unordered_map<int, double>> dis , tim; // 存了两点之间的时间 距离 通过坐标和速度计算得到 
string s1, s2, temp = "0";
int last;
unordered_map<string, double> v , change_tim; // v存了路线的配速 change_tim存了这个站换乘的时间 
double a, b, xx, yy;
int id_cnt = 481; // 总点数 

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
}G , GG , Graph_computing_cost ;
// 建了三个图 一个是正常的 另一个是少换乘的图 一个是换乘时间为0单纯算距离的图


double change_k = 8  ; 

void build_in(string inp , double x ){ // 这个表示对于这个名字的站点 在他们之间建立换乘时间的边 两两建边  x是换乘时间 
	change_tim[inp] = x ; 
	for(int j = 0 ; j < mp[inp].size() ; j++)
			for(int k = j + 1 ; k < mp[inp].size(); k++){
				G.add(mp[inp][j] , mp[inp][k] , x) ; 
				G.add(mp[inp][k] , mp[inp][j] , x) ; 
				GG.add(mp[inp][j] , mp[inp][k] , x * change_k) ; 
				GG.add(mp[inp][k] , mp[inp][j] , x * change_k) ; 
				Graph_computing_cost.add(mp[inp][j] , mp[inp][k] , 0) ; 
				Graph_computing_cost.add(mp[inp][k] , mp[inp][j] , 0) ; 
			}
	return ; 
}

void Changetim(){  // 读入站点 换乘时间  
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

void Pace(){ // 读入每个线路的配速
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

void read_Undergound(){ // 读入这些地铁站的坐标 名字 线路 
	freopen("Undergound.in", "r", stdin);
	for (int i = 1; i <= id_cnt ; i++)
    {
        cin >> s1 >> s2 >> a >> b;
		mpp[i] = (node){a , b , s2 , s1} ;  
        mp[s2].push_back(i) ; 
        if (s1 == temp){
        	dis[last][i] = dis[i][last] = sqrt((a - xx) * (a - xx) + (b - yy) * (b - yy));
        	tim[last][i] = tim[i][last] = 1.0*dis[last][i]/(v[s1]); 
        	G.add(i , last , tim[last][i]) ; GG.add(i , last , tim[last][i]) ; Graph_computing_cost.add(i , last , dis[last][i]) ; 
			G.add(last , i , tim[last][i]) ; GG.add(last , i , tim[last][i]) ; Graph_computing_cost.add(last , i , dis[last][i]) ;
		}
        else
            temp = s1;
        last = i , xx = a, yy = b;
    }
	fclose(stdin);
	cin.clear();
}


deque<int> path[25] ; // 存一种最短路的方案，也就是经过的点
double las_tim[25] ; // 存多种方案的最短时间 最后比较的时候用
int lasans[maxn] ; // 最终选定的最短路的方案  

bool isOnlyDigits(const std::string &str) {
    for (char ch : str) {
        if (!isdigit(ch)) {
            return false; 
        }
    }
    return true; 
}

string change(string route){  // 这个是输出的时候由于1号线读入的时候是“1” 必须要＋上“号线” 
	if(isOnlyDigits(route)){
		return route + "号线" ; 
	}
	else return route ; 
}

double query_type_1(string st , string ed , bool print_ ){  // 读入三个参数 起点终点 是否输出方案 最后返回时间 
	if(st == ed){
		if(print_){
			cout << "从"  << st << "到"  << st << endl ; 
			cout << "耗时0h0min" <<endl <<endl ; 
		}
		return 0 ; 
	}
	double to_tim[maxn] ; int vis[maxn] , pre[maxn] ; 
	for(int i = 0 ; i < mp[st].size() ; i++){ // 实际上 由于我们只知道起点和终点的名字 但其实同一个名字是有很多个点的 所以我们需要枚举谁为起点 也就是以几号线上的起点站来出发 最后还要看到达几号线上的终点时间最短
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
			 } // 找出时间最短的终点站 
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
		las_tim[i] = minn_tim ; // 记录当前方案 
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
	int zz = 0 ; 
	while(path[min_id].size()){
		lasans[++zz] = path[min_id].front() ; 
		path[min_id].pop_front() ; 
	}// 选择最短的并记录 最后输出 
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
	return min_tim ; 
}

void query_type_2(string st , string ed){ //  与1类似 不同之处在于换了个图跑 
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
	cout << "从" << change(mpp[lasans[1]].route) << "的" << mpp[lasans[1]].nam<< "开始" << endl ;  
	for(int i = 2 ; i <= zz ; i++){
		if(i == zz){
			cout <<  "最终到达" <<  mpp[lasans[i]].nam<< endl ; 
			break ; 
		}
		if(mpp[lasans[i]].nam == mpp[lasans[i-1]].nam){
			cout << "在" << mpp[lasans[i]].nam << "换乘" <<  change(mpp[lasans[i]].route) << endl ; 
			min_tim -= change_tim[mpp[lasans[i]].nam] * (change_k - 1) ; 
		}
		else cout << "途径" <<  mpp[lasans[i]].nam << endl ; 
	}
	int H=min_tim/3600;
	int M=(min_tim-H*3600)/60;
	cout << "预计花费" << H << "h" << M << 'm' << endl ; 
	printf("\n") ; 
	return ; 
}
void query_type_3(string q1 , string q2 , string q3 , double ka , double kb){// 计算val寻找中间点 
	double minn_val = 99999999 ; 
	string zhong ; 
	for(int i = 1 ; i <= id_cnt ; i++){
		double now_val = ka * query_type_1(q1 , mpp[i].nam , 0) + kb * query_type_1( mpp[i].nam  ,q2  , 0) + kb * query_type_1( mpp[i].nam  ,q3, 0) ; 
		if(now_val < minn_val) 
			minn_val = now_val , zhong = mpp[i].nam ; 
	}
	cout << "一起走的路程： " << endl ; 
	query_type_1(q1 , zhong , 1) ; 
	cout << "两人以后分别走的路程： " << endl ; 
	query_type_1(zhong , q2 , 1) ;
	query_type_1(zhong , q3 , 1) ;
}
void query_type_5(){
	string q1 , q2 ; 
	cin >> q1 >> q2 ; 
	string xulie[10] ; 
	int order[10] ; 
	int x ; cin >> x ; 
	int max_num = 1 ; int min_id = 0 ; double min_tim = 9999999999 ; 
	fer(i,1,x,1){
		cin >> xulie[i] ;
		max_num *= i ;  
		order[i] = i ; 
	}
	fer(i,0,max_num-1,1){
		double sum = 0  ; 
		string las = q1 ; 
		fer(j,1,x,1){
			sum += query_type_1(las , xulie[order[j]] , 0) ; 
			las = xulie[order[j]]; 
		}
		sum += query_type_1(las , q2 , 0) ; 
		if(min_tim > sum){
			min_tim = sum ; 
			min_id = i ; 
		}
		next_permutation(order + 1 , order + 1 + x) ;  // 每次全排列 
	}
	fer(i,1,min_id,1){
		next_permutation(order + 1 , order + 1 + x) ; 
	}
	cout << "以这样的顺序访问 总时间最少 : " <<endl ; 
	cout << q1 << " -> " ; 
	fer(i,1,x,1){
		cout << xulie[order[i]] << " -> " ; 
	} 
	cout << q2 << endl ; 
	min_tim = 0 ; string las  = q1 ; 
	fer(j,1,x,1){
		min_tim += query_type_1(las , xulie[order[j]] , 1) ; 
		las = xulie[order[j]]; 
	}
	min_tim += query_type_1(las , q2 , 1) ; 
	int H=min_tim/3600;
	int M=(min_tim-H*3600)/60;
	cout << "预计总共花费" << H << "h" << M << 'm' << endl ; 
	return ; 
}
void query_type_6(string st , string ed , string route_broke){
	if(st == ed){
		cout << "从"  << st << "到"  << st << endl ; 
		cout << "耗时0h0min" <<endl <<endl ; 
		return ; 
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
		Q.push((nod){st_id , 0}) ; 
		for(int j = 1 ; j <= id_cnt ; j++){ // 只多了这一步 记录哪些站停运了  
			if(mpp[j].route == route_broke){
				vis[j] = 1 ; 
			}				
		}
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
	if(min_tim == 99999999){
		printf("因路线停运 暂时无法通行\n")  ; 
		return ; 
	}
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
	return  ; 
}
void query_type_7(string st , string ed){ // 还是跑最短路即可 换到换乘时间为0的图上 
	int st_id = mp[st][0] , ed_id = mp[ed][0] ; 
	double dis[maxn] ; int vis[maxn] ; 
	fer(j,1,id_cnt,1){
		dis[j] = 99999999 , vis[j] = 0 ;
	}
	priority_queue<nod> Q ; 
	Q.push((nod){st_id , 0}) ; 
	while(!Q.empty()){
		nod now = Q.top() ; 
		Q.pop() ; 
		if(vis[now.num]) continue ;  
		vis[now.num] = 1; 
		for(int ii = Graph_computing_cost.head[now.num] ; ii ; ii = Graph_computing_cost.nxt[ii]){
			int y = Graph_computing_cost.to[ii] ; 
			if(dis[y] > now.tim + Graph_computing_cost.tim[ii] ){
				dis[y] = now.tim + Graph_computing_cost.tim[ii] ; 
				if(!vis[y]){
					Q.push((nod){y , dis[y]}) ; 
				}
			}
		}
	}
	int diss_m = dis[ed_id] / 1000 ; int cost ; 
	// printf("diss : %lf \n" , dis[ed_id]) ; 
	if(diss_m <= 6) {
		cost = 3 ; 
	} 
	else if(diss_m <= 12){
		cost = 4 ; 
	}
	else if(diss_m <= 22){
		cost = 5 ; 
	}
	else if(diss_m <= 32){
		cost = 6 ; 
	}
	else {
		cost = (diss_m - 32) / 20 + (diss_m % 20 == 0 ? 0 : 1) + 6 ; 
	}
	cout << "从" << st << "到"  << ed << "需要" << cost << "元" << endl ; 
	return ; 
}	
void read_query(){
	freopen("read.in" , "r" , stdin) ; 
	int kind ; 
	int cnt = 0 ; 
	while(scanf("%d",&kind) != EOF ){
		++cnt ; 
		cout << "第" << cnt << "个询问的答案："<<endl ; 
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
			query_type_3(q1 , q2 , q3 , 1 , 1) ; 
			query_type_3(q1 , q2 , q3 , 2 , 1) ; 
		}
		if(kind == 4){
			string q1 , q2 ; 
			double min_tim = 0 ; 
			cin >> q1 >> q2 ; 
			int x ; cin >> x ; 
			string las = q1 ; 
			for(int i = 1  ; i <= x ; i++ ){
				string zhong ; cin >> zhong ; 
				min_tim += query_type_1(las , zhong , 1) ; 
				las = zhong ; 
			}
			min_tim += query_type_1(las , q2 , 1) ; 
			int H=min_tim/3600;
			int M=(min_tim-H*3600)/60;
			cout << "预计总共花费" << H << "h" << M << 'm' << endl ; 
		}
		if(kind == 5){
			query_type_5() ; 
		}
		if(kind == 6){
			string q1 , q2 , q3 ; 
			cin >> q1 >> q2 >> q3 ; 
			query_type_6(q1 , q2 , q3) ; 
		}
		if(kind == 7){
			string q1 , q2 ; 
			cin >> q1 >> q2 ; 
			query_type_7(q1 , q2) ; 
		}
		printf("----------------------------------------------------------\n") ; 
	}
}
int main()
{
	freopen("ans.out", "w", stdout);
	Pace();
	read_Undergound() ; 
	Changetim();
	read_query() ; 
    return 0;
}
