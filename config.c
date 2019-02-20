
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define NODE_NUMBER 200
#define F_GAMMA 2
#define K_MIN 1

//--------------<プロトタイプ宣言>------------------------------------
double Uniform(void);	//0~1の乱数
void decide_probabirity(double p[NODE_NUMBER]);
void decide_degree(double p[NODE_NUMBER],int degr[NODE_NUMBER]);
void rem_init_and_degree_sum(int *sum,int rem[NODE_NUMBER],int degr[NODE_NUMBER]);
void create_edge(int *arg_sum,int rem[NODE_NUMBER],int *E,int *M);

//--------------<Main>---------------------------------------------
int main(void){
  
  double P[NODE_NUMBER];  //P[i]は字数がi以下の確率
  int degree[NODE_NUMBER];//各ノードの次数
  
  int k_sum;
  int k_rem[NODE_NUMBER];
  
  int *E;
  int edges=0;

	while(edges==0){	
		
		k_sum=0;
		
		//シード値入力
		srand((unsigned)time(NULL));
		
		//確率変数決定
		decide_probabirity(P);
    
		//各ノードの次数決定  
  	decide_degree(P,degree);
	
		//次数和を求めて、モデル化
  	rem_init_and_degree_sum(&k_sum,k_rem,degree);

		//Eの動的割り当
  	E=malloc(sizeof(int)*k_sum);
  
  	//枝の作成と枝の数の算出
  	create_edge(&k_sum,k_rem,E,&edges);
  	
  	free(E);
  }
  
  printf("edge=%d\n",edges);
    
  return 1;

}

//---------------------<function>----------------------

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
void decide_degree(double p[NODE_NUMBER],int degr[NODE_NUMBER]){
	int i;
	double rd;
	
	for(i=0;i<NODE_NUMBER;i++){
    do{
      rd=Uniform();
    }while(rd==1);
    
    degr[i]=K_MIN;
    while(rd>p[degr[i]])
      degr[i]++;
    
    //printf("degree[%d]=%d\n",i,degr[i]);
  }
}

//残りの点の初期化と次数和の算出
void rem_init_and_degree_sum(int *sum,int rem[NODE_NUMBER],int degr[NODE_NUMBER]){
	int i;
	
	for(i=0;i<NODE_NUMBER;i++){
    (*sum)+=degr[i];
    rem[i]=degr[i]; //初期化
  }
  
  if(*sum%2==1){
    rem[NODE_NUMBER-1]++;
    (*sum)++;
    //puts("最後の頂点の次数を1増やして、次数和を偶数にした。");
  }
  
  //printf("次数和=%d\n",*sum);
}

//枝の作成と枝の数の算出
void create_edge(int *arg_sum,int rem[NODE_NUMBER],int *E,int *M){
	int i;
	int repeat=0,repeat_max;
	int k_rem_max;
	int v_s,v_e;
	int ri=0;
	int sum,mult;
	int confirm_k_sum=*arg_sum;
	
	repeat_max=100*(*arg_sum);
	
	while(*arg_sum>0 && repeat<repeat_max){
    k_rem_max=0;
    
    for(i=0;i<NODE_NUMBER;i++){
      if(rem[i]>k_rem_max){
        k_rem_max=rem[i];
        v_s=i;
      }
    }
  	
    ri=(int)Uniform()*((*arg_sum)-k_rem_max);
    //printf("ri=%d\n",ri);
    v_e=-1;
    sum=0;
  
    while(ri>=sum){
      v_e++;
      if(v_e!=v_s)
        sum+=rem[v_e];
    }
  	
  	//printf("(v_s,v_f)=(%d,%d)\n",v_s,v_e);
    mult=0;
    i=0;
  
    while(i<(*M) && mult==0){
      if((E[2*i]==v_s && E[2*i+1]==v_e) || (E[2*i]==v_e && E[2*i+1]==v_s))
        mult=1;
      i++;
    }
  
    if(mult==0){
      E[2*(*M)]=v_s; E[2*(*M)+1]=v_e;
      (*arg_sum)-=2;
      (*M)++;
      rem[v_s]--; rem[v_e]--;
      //printf("rem[v_s],rem[v_e]=%d,%d\n",rem[v_s],rem[v_e]);
    }
  
    repeat++;
  }
  
  if(repeat==repeat_max)
    *M=0;
    
  //printf("edge=%d\n",*M);
}

