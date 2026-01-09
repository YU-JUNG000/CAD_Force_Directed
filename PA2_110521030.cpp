# include <iostream>
# include <math.h>
# include <stdio.h>
# include <algorithm>
# include <string.h>
# include <fstream>
# define MUL 3
# define ADD 1

using namespace std;


struct Node{
	int index;
	char type;	
	int latency;
	
	int in_1 = 0;
	int in_2 = 0;
	int out_num = 0;
	int *out_arr;
	
	int time_asap=0;
	int time_alap=0;
	int decision_pst = -1;	
	
	float mobility;
	float probability;
	float *expected_arr;
};
typedef struct Node node;



void ASAP(node *tree, int node_num, int latency_max){
	int time = 0;
	int print[node_num];
	int print_pst = 0;
//	cout << endl << "----------------------------------" << endl << "ASAP : " << endl;
	while (time < latency_max){
//		cout << endl << "time=" << time+1 << " ";
		for (int k=0; k<node_num; k++){
			if ((tree+k)->type=='+' || (tree+k)->type=='*'){
				if ((tree+(tree+k)->in_1-1)->type=='i' && (tree+(tree+k)->in_2-1)->type=='i'){
					print[print_pst] = k+1;
					print_pst ++;
				}
			}
		}
		
		for (int pst=0; pst<print_pst; pst++){
//			cout << (tree+print[pst]-1)->index << " ";
			(tree+print[pst]-1)->latency --;
			if ((tree+print[pst]-1)->time_asap == 0){
				(tree+print[pst]-1)->time_asap = time+1;
			}			
			if ((tree+print[pst]-1)->latency == 0)
				(tree+print[pst]-1)->type = 'i';
		}		
		time ++;
		print_pst = 0;
	}
}



void ALAP(node *tree, int node_num, int latency_max){
//	cout << "(tree+5)->type = " << (tree+5)->type << endl;
	int time = latency_max-1;
	int print[node_num];
	int print_pst = 0;
//	cout << endl << "--------------------------------" << endl << "ALAP : " << endl;
	while (time >= 0){
//		cout << endl << endl << "time=" << time+1;
		for (int k=0; k<node_num; k++){
			if ((tree+k)->type=='+' || (tree+k)->type=='*'){
				
				// check if all the output of the node transfered to 'o' state (ready to be calculated)
				int checker = (tree+k)->out_num-1;
				while (checker >= 0) {
					if ( (tree-1+ *((tree+k)->out_arr+checker))->type != 'o' )
						break;
					checker --;
				}
				
				// the node is ready
				if (checker == -1){
					print[print_pst] = k+1;
					print_pst ++;
				}
			}
		}	
		
		// print
//		cout << ", print : ";
		for (int pst=0; pst<print_pst; pst++){
//			cout << (tree+print[pst]-1)->index << " ";
			(tree+print[pst]-1)->latency --;
			if ((tree+print[pst]-1)->latency == 0)
				(tree+print[pst]-1)->type = 'o';
			
			(tree+print[pst]-1)->time_alap = time+1;
		}			
		time --;
		print_pst = 0;
	}
}

		

void expected_add(node *tree, float *expected_value, int node_num, int latency_max) {
//	cout << endl << "-------------------------" << endl << "expected value calculation : " << endl;
	for (int i=0; i<latency_max; i++){
		*(expected_value+i) = 0;
	}
	
	for (int idx_1=0; idx_1<node_num; idx_1++){
		if ((tree+idx_1)->type=='+'){
			// possible pst
			for (int poss=(tree+idx_1)->time_asap; poss<=(tree+idx_1)->time_alap; poss++){
				*(expected_value+poss-1) += (tree+idx_1)->probability;
			}
		}
	}
}



void expected_mul(node *tree, float *expected_value, int node_num, int latency_max) {
	for (int i=0; i<latency_max; i++){
		*(expected_value+i) = 0;
	}
	
	for (int idx_1=0; idx_1<node_num; idx_1++){
		if ((tree+idx_1)->type=='*'){
			// possible pst
			for (int poss=(tree+idx_1)->time_asap; poss<=(tree+idx_1)->time_alap; poss++){
				*(expected_value+poss-1) += (tree+idx_1)->probability;
			}
		}
	}
}



void Force_Directed_alg(node *tree, int node_num, int latency_max) {
	for (int no=0; no<node_num; no++){
		if ((tree+no)->mobility == 1){
			// fixed position
			(tree+no)->decision_pst = (tree+no)->time_asap;
		}else if((tree+no)->type == 'i'){
			// input port
			(tree+no)->decision_pst = 0;
		}else if ((tree+no)->type == 'o'){
			// output port
			(tree+no)->decision_pst = -2;
		}
	}
	
	
	int test = 10;
	int brk_ctrl = 0;
	while (test>0){
//		cout << test << " : the decision pst : " << endl;
//		for (int i=0; i<node_num; i++){
//			cout << i+1 << " : " << (tree+i)->decision_pst << endl;
//		} 
		test --;
		brk_ctrl = 1;
		for (int idx=0; idx<node_num; idx++){
			if ((tree-1+(tree+idx)->in_1)->decision_pst==-1 || (tree-1+(tree+idx)->in_2)->decision_pst==-1){
				if ((tree+idx)->type=='+' || (tree+idx)->type=='*'){
//					cout << "idx=" << idx+1 << ", " << (tree-1+(tree+idx)->in_1)->decision_pst << "  " << (tree-1+(tree+idx)->in_2)->decision_pst << endl;
					brk_ctrl = 0;
				}
			}else {
				if ((tree+idx)->decision_pst == -1){
					node *group_top = tree+idx;
					float self_force;
					float self_neg = 0;
					float successor_force = 0;
					// the negtive force in self_force
					for (int neg_force=group_top->time_asap; neg_force<=group_top->time_alap; neg_force++){
						self_neg += *(group_top->expected_arr+(neg_force-1)) * group_top->probability;
					}
					
			
					int limit_in1 = (tree-1+group_top->in_1)->decision_pst + (tree-1+group_top->in_1)->latency;
					int limit_in2 = (tree-1+group_top->in_2)->decision_pst + (tree-1+group_top->in_2)->latency;
					int start_pst = max({limit_in1, limit_in2, group_top->time_asap});
					for (int pst=start_pst; pst<=group_top->time_alap; pst++){					
						// self force for each position
						self_force = *(group_top->expected_arr+pst-1) - self_neg;
						// successor_force
						for (int out_cnt=0; out_cnt<group_top->out_num; out_cnt++){
							node *group_bottom = tree-1+*(group_top->out_arr+out_cnt);
							if (group_bottom->type != 'o'){
								int pos_beg = pst+group_top->latency;
								int pos_end = group_bottom->time_alap;
								int neg_beg = group_bottom->time_asap;
								if (pst == group_top->time_asap)
									successor_force = 0;
								else{
									for (int force_neg=neg_beg; force_neg<=pos_end; force_neg++){
										successor_force -= *(group_bottom->expected_arr + force_neg-1) * group_bottom->probability;
									}
									for (int force_pos=pos_beg; force_pos<=pos_end; force_pos++){
										successor_force += *(group_bottom->expected_arr + force_pos-1) * 1/float(pos_end-pos_beg+1);
									}
								}
							}
						}					
//						cout << idx+1 << "   self_force = " << self_force << ", successor_force = " << successor_force << endl;
						if (self_force>=successor_force || pst==start_pst)
							group_top->decision_pst = pst;
						else
							break;						
					} 
				}
			}	
//			cout << test << " : idx = " << idx << endl << endl;
		}
//		cout << test << " : break_ctrl = " << brk_ctrl << endl;
		if (brk_ctrl)
			break;	
	}
}
	


// count the adders and multipliers
void Add_Mul_cnt(node *tree, int *add_mul, int node_num, int latency_max){
	int add_arr[latency_max];
	int mul_arr[latency_max];
	int timing[latency_max];
	for (int i=0; i<latency_max; i++){
		add_arr[i] = 0;
		mul_arr[i] = 0;
	}
	
	for (int idx=0; idx<node_num; idx++){
		if ((tree+idx)->type == '+'){
			add_arr[(tree+idx)->decision_pst-1] ++;	
		}else if ((tree+idx)->type == '*'){
			mul_arr[(tree+idx)->decision_pst-1] ++;
			mul_arr[(tree+idx)->decision_pst] ++;
			mul_arr[(tree+idx)->decision_pst+1] ++;
		}		
	}
	
	// choose the maximum for the requirement
	*(add_mul) = 0;
	*(add_mul+1) = 0;
	for (int k=0; k<latency_max; k++){
		if (add_arr[k] > *(add_mul))
			*(add_mul) = add_arr[k];
		if (mul_arr[k] > *(add_mul+1))
			*(add_mul+1) = mul_arr[k];
	}
}


	
int main()
{
	int  latency_max;
	int  node_num;
	int  edge_num;				
		
	FILE* fp;
	fp = fopen("testcase3","r");
		
	fscanf(fp,"%d",&latency_max);
	fscanf(fp,"%d",&node_num);
	fscanf(fp,"%d",&edge_num);
	
	
	
	// node part
	int type_arr[4][node_num];
	int in_num = 0;
	int add_num = 0;
	int mul_num = 0;
	int out_num = 0;
	node tree[node_num];
	node tree_asap[node_num];
	node tree_alap[node_num];
	for (int p=0; p<4; p++){
		for (int q=0; q<node_num; q++){
			type_arr[p][q] = 0;
		}
	}
	for (int i=0; i<node_num; i++){
		fscanf(fp, "%d %c", &(tree[i].index), &(tree[i].type));
		if (tree[i].type == 'i'){
			tree[i].latency = 0;
			type_arr[0][in_num] = i+1;
			in_num ++;
		}else if(tree[i].type == 'o'){
			tree[i].latency = 0;
			type_arr[3][out_num] = i+1;
			out_num ++;
		}else if (tree[i].type == '+'){
			tree[i].latency = ADD;
			type_arr[1][add_num] = i+1;
			add_num ++;
		}else if (tree[i].type == '*'){
			tree[i].latency = MUL;
			type_arr[2][mul_num] = i+1;
			mul_num ++;
		}
	}
	
	
	// edge part
	int size = floor(node_num/3);
	for (int i=0; i<node_num; i++){
		tree[i].out_arr = new int[size];
	}
	int a, b;	
	for (int j=0; j<edge_num; j++){
		fscanf(fp, "%d %d", &a, &b);
		
		// input of b
		if (tree[b-1].in_1 == 0)
			tree[b-1].in_1 = a;
		else {
			tree[b-1].in_2 = a;
		}
			
				
		// output of a
		tree[a-1].out_arr[tree[a-1].out_num] = b;
		tree[a-1].out_num ++;
	}
	

	
	// copy to the "tree"
	memcpy(tree_asap, tree, sizeof(tree));	
	memcpy(tree_alap, tree, sizeof(tree));
	
	ASAP(tree_asap, node_num, latency_max);
	ALAP(tree_alap, node_num, latency_max);
	for (int cp=0; cp<node_num; cp++){
		tree[cp].time_asap = tree_asap[cp].time_asap;
		tree[cp].time_alap = tree_alap[cp].time_alap;
		tree[cp].mobility = tree[cp].time_alap - tree[cp].time_asap +1;
		tree[cp].probability = 1/tree[cp].mobility;
	}
	
	
	// check the expected value	 
	float add_pt[latency_max];
	float mul_pt[latency_max];
	expected_add(tree, add_pt, node_num, latency_max);
	expected_mul(tree, mul_pt, node_num, latency_max);
	
	for (int idx=0; idx<node_num; idx++){
		if (tree[idx].type == '+'){
			tree[idx].expected_arr = add_pt;
		}else if (tree[idx].type == '*') {
			tree[idx].expected_arr = mul_pt;
		}
	}	
	

	
	Force_Directed_alg(tree, node_num, latency_max);
	
	
	// output file
	ofstream out_file;
	out_file.open("testcase.out");
	
	
	int add_mul[2];
	Add_Mul_cnt(tree, add_mul, node_num, latency_max);
//	cout << endl << "-------------------------------------" << endl << "last print : " << endl;
	out_file << add_mul[0] << endl << add_mul[1] << endl;

	// timing schedule
	for (int lt=1; lt<=latency_max; lt++){
//		if (lt != 1)
//			out_file << endl;
			
		int pst = 0;
		for (int idx=0; idx<node_num; idx++){
			if (tree[idx].decision_pst==lt && tree[idx].type=='+'){
				if (pst == 0)
					out_file << idx+1;
				else
					out_file << " " << idx+1;
				
				pst ++;
			}else if ((tree[idx].decision_pst==lt || tree[idx].decision_pst==lt-1 || tree[idx].decision_pst==lt-2) && tree[idx].type=='*'){
				if (pst == 0)
					out_file << idx+1;
				else
					out_file << " " << idx+1;
				
				pst ++;				
			}				
		}
    out_file << endl;
	}	
}















