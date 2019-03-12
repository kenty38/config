
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define NODE_NUMBER 10
#define F_GAMMA 2
#define K_MIN 1

struct Node{
  int id;       //各ノードの番号、0からNODE_NUMBER-1番まで
  int degree;   //次数
  int rem;      //残りの次数
  int *ad_list;  //隣接リスト
};

//--------------<プロトタイプ宣言>------------------------------------
double Uniform(void);	//0~1の乱数
void decide_probabirity(double p[NODE_NUMBER]);
void decide_degree(struct Node n[NODE_NUMBER],double p[NODE_NUMBER]);
void rem_init_and_degree_sum(int *sum,struct Node n[NODE_NUMBER]);
void create_edge(int *deg_sum,struct Node n[NODE_NUMBER],int *M);

//--------------<Main>---------------------------------------------
int main(void){
  
  struct Node node[NODE_NUMBER];  
  int k_sum;							//次数和
  
  int edges=0;
  
  double P[NODE_NUMBER];
  
  //シード値入力
  srand((unsigned)time(NULL));
  
	//while(edges==0){	
		
		k_sum=0;
		
		//確率変数決定
		decide_probabirity(P);
    
		//各ノードの次数決定  
  	decide_degree(node,P);
	
		//次数和を求めて、モデル化
  	rem_init_and_degree_sum(&k_sum,node);

  	printf("k_sum=%d\n",k_sum);
  	//枝の作成と枝の数の算出
  	create_edge(&k_sum,node,&edges);
  	
  	//break;
  //}
  
  //printf("edge=%d\n",edges);
    
  return 1;

}

//---------------------<function>------------------------------------------------------

//0~1の乱数を発生
double Uniform(void){
  return ((double)rand()+1.0)/((double)RAND_MAX+2.0);
}

//確率変数決定
void decide_probabirity(double p[NODE_NUMBER]){
	int i;
	double N=0;             //次数分布の規格化定数
	
  for(i=K_MIN;i<NODE_NUMBER;i++)
    N+=exp(-log(i)*F_GAMMA);    //i^(-gamma)

  p[K_MIN]=exp(-log(K_MIN)*F_GAMMA)/N;
    
  for(i=K_MIN+1;i<NODE_NUMBER;i++)
    p[i]=p[i-1]+exp(-log(i)*F_GAMMA)/N;
    
  //for(i=K_MIN;i<NODE_NUMBER;i++)printf("P[%d]=%f\n",i,p[i]);
}

//各ノードの次数の決定
void decide_degree(struct Node n[NODE_NUMBER],double p[NODE_NUMBER]){
	int i;
	double rd;
	
	for(i=0;i<NODE_NUMBER;i++){
    
		do{
      rd=Uniform();
    }while(rd==1);
    
    n[i].degree=K_MIN;
    while(rd>p[n[i].degree])
      n[i].degree++;

		n[i].ad_list=malloc(sizeof(int)*n[i].degree);
    
  //printf("degree[%d]=%d\n",i,n[i].degree);
  }
}

//残りの点の初期化と次数和の算出
void rem_init_and_degree_sum(int *sum,struct Node n[NODE_NUMBER]){
	int i;
	
	for(i=0;i<NODE_NUMBER;i++){
    (*sum)+=n[i].degree;
    n[i].rem=n[i].degree; //初期化
  }
  
  if(*sum%2==1){
    n[NODE_NUMBER-1].rem++;
    (*sum)++;
    //puts("最後の頂点の次数を1増やして、次数和を偶数にした。");
  }
  
  //printf("次数和=%d\n",*sum);
}

//枝の作成と枝の数の算出
void create_edge(int *deg_sum,struct Node n[NODE_NUMBER],int *M){//Mは枝数
	int i,j;
	int repeat=0,repeat_max;
	int k_rem_max;										//残りの次数の中での最大の次数
	int v_s,v_e;
	int ri=0;
	int sum,mult;
	int confirm_k_sum=*deg_sum;
	
	//反復回数決定
	repeat_max=100*(*deg_sum);
	
	//枝の割り当て,次数和が0になるまで繰り返す
	while(*deg_sum>0 && repeat<repeat_max){
    k_rem_max=0;
    
    //残った点の中から次数最大の点を選出,これが始点となる
    for(i=0;i<NODE_NUMBER;i++){
      if(n[i].rem>k_rem_max){
        k_rem_max=n[i].rem;
        v_s=i;
      }
    }
  	
  	//k_rem_max分の次数を引いて、残りの次数を選ぶ
    ri=(int)(Uniform()*((*deg_sum)-k_rem_max));
    //printf("ri=%d\n",ri);
    
    //初期化
    v_e=-1;
    sum=0;
  	
  	//終点を決める
    while(ri>=sum){
      v_e++;
      if(v_e!=v_s)
        sum+=n[v_e].rem;  //終点と始点が同じじゃなければ,sumに次数を足していく
    }
  	
  	//printf("(v_s,v_f)=(%d,%d)\n",v_s,v_e);
  	
  	//初期化
    mult=0;
    i=0;
  	
  	//枝の組み合わせが既存か確認(高速化可能)
    for(i=0;i<n[v_s].degree-n[v_s].rem;i++){

			if(n[v_s].ad_list[i]==v_e || n[v_e].rem==0){
				mult=1;
				break;
			}
		}
  	
  	//枝の組み合わせがなければ、次数和から2引いて各点の次数を1ずつ減らす
    if(mult==0){
      n[v_s].ad_list[n[v_s].degree-n[v_s].rem]=v_e;
			n[v_e].ad_list[n[v_e].degree-n[v_e].rem]=v_s;
      (*deg_sum)-=2;
      (*M)++;
      n[v_s].rem--; n[v_e].rem--;
      //printf("rem[v_s],rem[v_e]=%d,%d\n",rem[v_s],rem[v_e]);
    }
    repeat++;
  }
  
  //最大値に到達したら失敗
  if(repeat==repeat_max){
    *M=0;
  }
  
  if(*M>0){
    //隣接リスト確認  
    for(i=0;i<NODE_NUMBER;i++){
      printf("%d:",i);
      for(j=0;j<n[i].degree;j++){
        printf("%d,",n[i].ad_list[j]);
      }
      puts("");
    }
  }
  
  //メモリ解放
  for(i=0;i<NODE_NUMBER;i++)
    free(n[i].ad_list);
  
  //printf("edge=%d\n",*M);
}
