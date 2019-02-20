
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

double Uniform(void);

int main(void){
  int i;
  
  //シード値入力
	srand((unsigned)time(NULL));
  
  //関数の引数
  int node_number=8;
  double f_gamma=2;
  int k_min=1;
  
  double N=0;             //次数分布の規格化定数
  double P[node_number];  //P[i]は字数がi以下の確率
  
  int degree[node_number];
  double rd;
  
  int k_sum=0;
  int k_rem[node_number];
  
  int *E;
  int edges=0:
  int k_rem_max;  //残り次数の最大値
  int repeat=0;
  int repeat_max;
  
  int v_s,v_e,ri;
  int sum;
  int mult;

//確率変数決定
  for(i=k_min;i<node_number;i++)
    N+=exp(-log(i)*f_gamma);    //i^(-gamma)

  P[k_min]=exp(-log(k_min)*f_gamma)/N;
    
  for(i=k_min+1;i<node_number;i++)
    P[i]=P[i-1]+exp(-log(i)*f_gamma)/N;
    
//各ノードの次数決定  
  for(i=0;i<node_number;i++){
    do{
      rd=Uniform();
    }while(rd==1);
    
    degree[i]=k_min;
    while(rd>P[degree[i]])
      degree[i]++;
    
    //printf("degree[%d]=%d\n",i,degree[i]);
  }

//次数和を求めて、モデル化
  for(i=0;i<N;i++){
    k_sum+=degree[i];
    k_rem[i]=degree[i]; //初期化
  }
  
  if(k_sum%2==1){
    k_rem[node_number-1]++;
    k_sum++;
    puts("最後の頂点の次数を1増やして、次数和を偶数にした。");
  }

//
  E=malloc(sizeof(double)*k_sum);
  repeat_max=100*sum;
  
  while(k_sum>0 && repeat<repeat_max){
    k_rem_max=0;
    
    for(i=0;i<node_number;i++){
      if(k_rem[i]>k_rem_max){
        k_rem_max=k_rem[i];
        v_s=i;
      }
    }
  
    ri=(int)(Uniform()*(k_sum-k_rem_max));
    v_e=-1;
    sum=0;
  
    while(ri>=sum){
      v_e++;
      if(v_e!=v_s)
        sum+=k_rem[v_e];
    }
  
    mult=0;
    i=0;
  
    while(i<edges && mult==0){
      if()
        mult=1;
      i++;
    }
  
    if(mult==0){
      E[2*edges]=v_s; E[2*edges+1]=v_e;
      k_sum-=2;
      edges++;
      k_rem[v_s]--; k_rem[v_e]--;
    }
  
    repeat++;
  }
  if(repeat==repeat_max)
    edges=0;
    
  return 1;

}

//0~1の乱数を発生
double Uniform(void){
  return ((double)rand()+1.0)/((double)RAND_MAX+2.0);
}

