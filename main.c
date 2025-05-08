#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node{
	
	char name[20];
	int age;
	int salary;
	int childCount;
	int level;
		
	struct Node* parent;
	struct Node** children;
	
}Node;

typedef struct{
	
	char name[20];
	int age;
	int salary;
	char parentName[20];
	
}TempNode;

void addEdge(Node* parent, Node* child){

	parent->children = (Node**)realloc(parent->children,sizeof(Node*)*(parent->childCount + 1));
	
	parent->children[parent->childCount++] = child;
	child->parent = parent;	
}

void assignLevels(Node* node ,int level, int* totalAge, int* totalSalary){
	int i;
	node->level = level;
	*totalAge += node->age;
	*totalSalary += node->salary;
	
	for(i=0;i<node->childCount;i++){
		assignLevels(node->children[i],level + 1,totalAge,totalSalary);
	}	
}

int findMaxLevel(Node* node, int maxLevel){
	int i;
	
	if(node->level > maxLevel){
		maxLevel = node->level;
	}
	for(i=0;i<node->childCount;i++){
		maxLevel = findMaxLevel(node->children[i],maxLevel);	
	}
	
	return maxLevel;
}

void countPerLevel(Node* node, int* levelCounts){
	int i;
	levelCounts[node->level-1]++;
	for(i=0;i<node->childCount;i++){
		countPerLevel(node->children[i],levelCounts);
	}	
}

void findMostSubordinatesAtLevel(Node* node, Node** most, int targetLevel){
	int i;
	
	if(node->level<targetLevel){
		for(i=0;i<node->childCount;i++){
			findMostSubordinatesAtLevel(node->children[i],most,targetLevel);
		}
	}else{
		if((*most) == NULL){
			(*most) = node;
		}else if((*most)->childCount < node->childCount){
			(*most) = node;
		}
	}
}

void printTree(Node* node, char* prefix, int isLast) {
	int i;
    printf("%s", prefix);
    printf("%s", isLast ? "+-- " : "|-- ");
    printf("%s\n", node->name);

    char newPrefix[100];
    strcpy(newPrefix, prefix);
    strcat(newPrefix, isLast ? "    " : "|   ");

    for (i = 0; i < node->childCount; i++) {
        printTree(node->children[i], newPrefix, i == node->childCount - 1);
    }
}




int main(){
	
	int totalCount, i, j, maxLevel, totalAge=0, totalSalary=0;
	
	FILE* file = fopen("personel.txt","r");
	if(file == NULL){
		printf("Dosya okunamadi!!!");
		return 1;
	}
	
	fscanf(file,"%d",&totalCount);
	
	TempNode* tempNodes = (TempNode*)malloc(totalCount*sizeof(TempNode));
	
	for(i=0;i<totalCount;i++){
		fscanf(file,"%s %d %d %s", tempNodes[i].name, &tempNodes[i].age, &tempNodes[i].salary, tempNodes[i].parentName);
	}
	
	fclose(file);
	
	Node** nodeList = (Node**)malloc(totalCount*sizeof(Node*));
	
	for(i=0;i<totalCount;i++){
		
		Node* newNode = (Node*)malloc(sizeof(Node));
		strcpy(newNode->name, tempNodes[i].name);
		newNode->age = tempNodes[i].age;
		newNode->salary = tempNodes[i].salary;
		newNode->parent = NULL;
		newNode->children = NULL;
		newNode->childCount = 0;
		newNode->level = 0;
		nodeList[i] = newNode;
	}
	
	Node* root = NULL;
	 
	for(i=0;i<totalCount;i++){
		
		Node* child = nodeList[i];
		Node* parent = NULL;
		
		int isRoot = strcmp(tempNodes[i].parentName,"NULL") == 0 ? 1 : 0; 
		
		if(isRoot != 1){
			j=0;
			while( j<totalCount && strcmp(tempNodes[i].parentName,nodeList[j]->name) != 0 ){
				j++;
			}
			if(strcmp(tempNodes[i].parentName,nodeList[j]->name) == 0 ){
				parent = nodeList[j];
			}
		}else{
			root = nodeList[i];
		}
		
		if(parent){
			addEdge(parent,child);
		}
	}
	
	assignLevels(root,1,&totalAge,&totalSalary);
	maxLevel = findMaxLevel(root,0);
	int* levelCounts  = (int*)calloc(maxLevel,sizeof(int));
	countPerLevel(root,levelCounts);
	
	printf("-Ilgili sirketin personel agaci %d seviyeden olusuyor.\n",maxLevel);
	for(i=0;i<maxLevel;i++){
		printf("-Seviye %d: %d\n",i+1,levelCounts[i]);
	}
	
	printf("Alt calisan sayisi en fazla olan kisiyi bulmak icin seviye giriniz: ");
	scanf("%d",&i);
	
	Node* most = NULL;
	
	findMostSubordinatesAtLevel(root,&most,i);
	
	printf("-%d. Seviyede en fazla calisana sahip olan kisi %d kisi ile %s'dir.\n-Tum calisanlarin yas ortalamasi: %.2f\n-Sirketin odedigi aylik personel maas gideri: %dTL\n",i,most->childCount,most->name,(float)totalAge/totalCount,totalSalary);
	
	char prefix[100] = "";
	printTree(root,prefix,1);

	return 0;
}
