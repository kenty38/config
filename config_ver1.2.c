
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define NODE_NUMBER 5000		//ノード数
#define F_GAMMA 2						//
#define K_MIN 1							//最低次数

#define FILENAME "/home/calc/config/network.csv"

enum EDGE{
	NON_INCLUDED,
	INCLUDED
};

enum V_S_STATUS{
	NON_CHOICED,
	CHOICED
};

struct Node{
  int id;       //各ノードの番号、0からNODE_NUMBER-1番まで
  int degree;   //次数
  int rem;      //残りの次数
  int *ad_list;  //隣接リスト
};

FILE *fp;

//--------------<プロトタイプ宣言>------------------------------------
double Uniform(void);	//0~1の乱数
void decide_probabirity(double p[NODE_NUMBER]);
void decide_degree(struct Node n[NODE_NUMBER],double p[NODE_NUMBER]);
void rem_init_and_degree_sum(int *sum,struct Node n[NODE_NUMBER]);
void create_edge(int *deg_sum,struct Node n[NODE_NUMBER],int *M);
void free_ad_list(struct Node n[NODE_NUMBER]);
void create_csv(struct Node n[NODE_NUMBER]);
void display(struct Node n[NODE_NUMBER],int M,int deg_sum);

//create_edge内で使用
void ad_list_confirm(struct Node nn[NODE_NUMBER],int list_side,int neighbor,char *judge);
void ad_list_sort(struct Node nn[NODE_NUMBER]);

void Qsort(int x[], int left, int right);
void Swap(int x[], int i, int j);

//--------------<Main>---------------------------------------------
int main(void){
  
  struct Node node[NODE_NUMBER];  
  int k_sum;							//次数和
  
  int edges=0;						//辺の数
  
  double P[NODE_NUMBER];	//確率分布
  
  //シード値入力
  srand((unsigned)time(NULL));
  
	while(edges==0){	
		
		k_sum=0;
		
		//確率変数決定
		decide_probabirity(P);
    
		//各ノードの次数決定  
  	decide_degree(node,P);
	
		//次数和を求めて、モデル化
  	rem_init_and_degree_sum(&k_sum,node);

  	//枝の作成と枝の数の算出
  	create_edge(&k_sum,node,&edges);
  	
  	//隣接リストや各点の次数を表示
  	display(node,edges,k_sum);
  	
  	//csvファイル作成
  	if(edges!=0)
  		create_csv(node);
  	
  	//メモリ開放
  	free_ad_list(node);
  	
  	//break;
  }
    
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
    n[NODE_NUMBER-1].degree++;
    n[NODE_NUMBER-1].rem++;
    (*sum)++;
    //puts("最後の頂点の次数を1増やして、次数和を偶数にした。");
  }
  
  //次数和の調整後、隣接リスト形成
  for(i=0;i<NODE_NUMBER;i++)
	  n[i].ad_list=malloc(sizeof(int)*(n[i].degree));
  
  //printf("次数和=%d\n",*sum);
}

//枝の作成と枝の数の算出
void create_edge(int *deg_sum,struct Node n[NODE_NUMBER],int *M){//Mは枝数
	int i,j;
	int repeat=0,repeat_max;
	int k_rem_max;										//残りの次数の中での最大の次数
	int v_s,v_e;
	int ri=0;
	int sum;
	char mult=NON_INCLUDED,v_s_neighbor=NON_CHOICED;
	int confirm_k_sum=*deg_sum;
	
	//反復回数決定
	repeat_max=100*(*deg_sum);
	
	//枝の割り当て,次数和が0になるまで繰り返す
	while(*deg_sum>0 && repeat<repeat_max){
    
    //残った点の中から次数最大の点を選出,これが始点となる
    if(v_s_neighbor==NON_CHOICED){
    	k_rem_max=0;
    	
    	for(i=0;i<NODE_NUMBER;i++){
      	if(n[i].rem>k_rem_max){
      	  k_rem_max=n[i].rem;
      	  v_s=i;
      	}
    	}
    	v_s_neighbor=CHOICED;
    }
  	
  	//k_rem_max分の次数を引いて、残りの次数を選ぶ
    ri=(int)(Uniform()*((*deg_sum)-k_rem_max));
    
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
    mult=NON_INCLUDED;
    i=0;
  	
  	//枝の組み合わせが既存か確認(高速化可能)
  	if(n[v_s].degree-n[v_s].rem < n[v_e].degree-n[v_e].rem){
    	ad_list_confirm(n,v_s,v_e,&mult);
		}	
		else{
			ad_list_confirm(n,v_e,v_s,&mult);
		}
  	
  	//枝の組み合わせがなければ、次数和から2引いて各点の次数を1ずつ減らす
    if(mult==NON_INCLUDED){
      n[v_s].ad_list[n[v_s].degree-n[v_s].rem]=v_e;
			n[v_e].ad_list[n[v_e].degree-n[v_e].rem]=v_s;
      (*deg_sum)-=2;
      (*M)++;
      n[v_s].rem--; n[v_e].rem--;
      v_s_neighbor=NON_CHOICED;
    }
    repeat++;
  }
  
  //最大値に到達したら失敗
  if(repeat==repeat_max){
    *M=0;
  }
  
  ad_list_sort(n);
  
}

void free_ad_list(struct Node n[NODE_NUMBER]){
	int i;
	
	//メモリ解放
  for(i=0;i<NODE_NUMBER;i++)
    free(n[i].ad_list);
}

void create_csv(struct Node n[NODE_NUMBER]){
	int i,j;

	if((fp=fopen(FILENAME,"w"))==NULL){
		printf("ファイルをオープンできません\n");
		exit(1);
	}
	
	for(i=0;i<NODE_NUMBER;i++){
		fprintf(fp,"%d,,",n[i].degree);
		
		for(j=0;j<n[i].degree;j++){
			fprintf(fp,"%d",n[i].ad_list[j]);
			if(j!=n[i].degree-1)
				fprintf(fp,",");
			else
				fprintf(fp,"\n");
		}
	}
}

void display(struct Node n[NODE_NUMBER],int M,int deg_sum){
	int i,j;
#if 0
	//各点の次数確認
	for(i=0;i<NODE_NUMBER;i++)
	printf("degree[%d]=%d\n",i,n[i].degree);
	
  //隣接リスト確認  
  for(i=0;i<NODE_NUMBER;i++){
    printf("%d:",i);
    for(j=0;j<n[i].degree;j++){
      printf("%d,",n[i].ad_list[j]);
    }
    puts("");
  }
#endif
  printf("k_sum=%d\n",deg_sum);
  printf("edge=%d\n",M);
}

void ad_list_confirm(struct Node nn[NODE_NUMBER],int list_side,int neighbor,char *judge){
	int i;
	
	for(i=0;i<nn[list_side].degree-nn[list_side].rem;i++){
		if(nn[list_side].ad_list[i]==neighbor || nn[neighbor].rem==0){
			*judge=INCLUDED;
			break;
		}
	}
}

void ad_list_sort(struct Node nn[NODE_NUMBER]){
	int i;
	
	for(i=0;i<NODE_NUMBER;i++)
		Qsort(nn[i].ad_list,0,nn[i].degree-1);
}


void Qsort(int x[], int left, int right)
{
  int i, j;
  int pivot;

  i = left;                      /* ソートする配列の一番小さい要素の添字 */
  j = right;                     /* ソートする配列の一番大きい要素の添字 */

  pivot = x[(left + right) / 2]; /* 基準値を配列の中央付近にとる */

  while (1) {                    /* 無限ループ */

      while (x[i] < pivot)       /* pivot より大きい値が */
          i++;                   /* 出るまで i を増加させる */

      while (pivot < x[j])       /* pivot より小さい値が */
          j--;                   /*  出るまで j を減少させる */
      if (i >= j)                /* i >= j なら */
          break;                 /* 無限ループから抜ける */

      Swap(x, i, j);             /* x[i] と x[j]を交換 */
      i++;                       /* 次のデータ */
      j--;
  }

  if (left < i - 1)              /* 基準値の左に 2 以上要素があれば */
      Qsort(x, left, i - 1);     /* 左の配列を Q ソートする */
  if (j + 1 <  right)            /* 基準値の右に 2 以上要素があれば */
      Qsort(x, j + 1, right);    /* 右の配列を Q ソートする */
}

void Swap(int x[], int i, int j){
  int temp;

  temp = x[i];
  x[i] = x[j];
  x[j] = temp;
}

