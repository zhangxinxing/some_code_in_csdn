/*
ID: abc18711
LANG: C
TASK: heritage
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXN 27

char pre[MAXN];
char mid[MAXN];

FILE *fout ;

typedef struct node{
	char data;
	struct node *left;
	struct node *right;
}NODE;

void postTrav(NODE *root){
	if(root->left != NULL)
		postTrav(root->left);
	if(root->right != NULL)
		postTrav(root->right);
	fprintf(fout, "%c", root->data);
}

void buildTree(char *pre, char *mid, NODE *root){
	int len = strlen(pre);
	if(len == 0)
		return;
	
	char *p = strchr(mid, pre[0]);
	int pos = (int)(p - mid);

	
	root->data = pre[0];
	
	if(pos != 0){
		NODE *left = (NODE*)malloc(sizeof(struct node));
		root->left = left;
		
		left->data = pre[1];
		char *left_pre = (char*)malloc(pos*sizeof(char));
		char *left_mid = (char*)malloc(pos*sizeof(char));
	
		strncpy(left_pre, pre+1, pos);
		strncpy(left_mid, mid, pos);
		
		buildTree(left_pre, left_mid, left);
	}

	if(pos != len-1){
		NODE *right = (NODE*)malloc(sizeof(struct node));
		root->right = right;
		
		right->data = pre[pos+1];
		char *right_pre = (char*)malloc((len-1-pos)*sizeof(char));
		char *right_mid = (char*)malloc((len-1-pos)*sizeof(char));
		
		strncpy(right_pre, pre+pos+1, len-1-pos);
		strncpy(right_mid, mid+pos+1, len-1-pos);

		buildTree(right_pre, right_mid, right);
		
	}
	
	
}


int main(){
	FILE *fin = fopen("heritage.in", "r");
	
	fout = fopen("heritage.out", "w");
	
	fscanf(fin, "%s", mid);
	fscanf(fin, "%s", pre);
	
	
	NODE *root = (NODE*)malloc(sizeof(struct node));
	buildTree(pre, mid, root);
	
	postTrav(root);
	
	fprintf(fout, "\n");
	fclose(fin);
	fclose(fout);

	return 0;
}
