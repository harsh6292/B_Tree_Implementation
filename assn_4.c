#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct
{
	int noOfKeys;
	int *nodeKeys;
	long *nodeChildOffsets;

}bTreeNode;



typedef struct
{
	long offset;
	int newLevel;
	int space;
	struct printQueue *next;
}printQueue;




void initializeNode_Read(bTreeNode *, int, FILE *, long);
long searchForKey(int, long, int, FILE *, long*);
void insertIntoBTree(long *, int, int, FILE *);
void writeToFile(bTreeNode *, long *, FILE *, int);
void insertIntoBTree_SplitNode(bTreeNode *, int, int, long *, FILE *, int, int*);
void insertIntoBtree_NonSplitNode(bTreeNode*, long, int, int, FILE*, long*);
void printBtree(long rootNodeOffset, FILE *inputFile, int bTreeOrder, int val);
int insertIntoBTree_Node(int , long *, int , long *, long *, int *, int , FILE *);


int count = 0;
int main(int argc, char *argv[])
{
	 char *inputFileName, *readLine, *funcToPerform = NULL, *primaryKeyInString = NULL;
	 int bTreeOrder = 0, primaryKeyInInt = 0, getVal = 0;
	 long rootNodeOffset = 0, tempNodeOffset = 0;
	 long LOffset = 0, ROffset = 0;
	 int keyToUpLevel = 0;
	 
	 FILE *inputFile = NULL;


	inputFileName = argv[1];
	bTreeOrder = atoi(argv[2]);

	
	funcToPerform = 	(char*)malloc(sizeof(char)*100);
	primaryKeyInString= (char*)malloc(sizeof(char)*100);
	readLine = (char*)malloc(sizeof(char)*100);
	

	if( (inputFile = fopen(inputFileName, "r+b")) == NULL )
	{
		//File doesn't exist, Open in write mode
		inputFile = fopen(inputFileName, "w+b");
		rootNodeOffset = -1;
		// bTreeNode rootNode;
		// initializeNode(&rootNode, bTreeOrder);
		// rootNodeOffset = (long)&rootNode;
	}
	else
	{
		//read the index file data into btreeNode
		//but first read root node offset
		fread(&rootNodeOffset, sizeof(long), 1, inputFile);
	}


	while( (fgets(readLine, 100, stdin)) != NULL )
	{
		funcToPerform = strtok(readLine, " \n");
		printf("\n\n*******************   COUNT  %d   ****************************\n", count);
		printf("Function to Perform: %s\n", funcToPerform);
		if( strcmp(funcToPerform, "add") == 0 )
		{
			primaryKeyInString = strtok(NULL, "\n");
			primaryKeyInInt = (int)strtoll(primaryKeyInString, NULL, 10);

			tempNodeOffset = 0;
			printf("Searching for key: %d\n", primaryKeyInInt);
			getVal = searchForKey(primaryKeyInInt, rootNodeOffset, bTreeOrder, inputFile, &tempNodeOffset);
			//printf("Returned from search\n");
			if( (getVal == -1) && (tempNodeOffset != -2))
			{
				
				
				
				insertIntoBTree_Node(primaryKeyInInt, &rootNodeOffset, bTreeOrder, &LOffset, &ROffset, &keyToUpLevel, 1, inputFile);
				
				
				//means key not found or pointer to next level is being returned
				//printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Inserting into B-Tree, rootNodeOffset: %d\n", rootNodeOffset);
				
				//insertIntoBTree(&rootNodeOffset, primaryKeyInInt, bTreeOrder, inputFile);
				
				//printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Returned from Insert, rootNodeOffset-: %d\n", rootNodeOffset);
				
				printBtree(rootNodeOffset, inputFile, bTreeOrder, 1);
			}
			else
			{
				printf("Entry with key=%d already exists\n", primaryKeyInInt);
			}
		}
		else if( strcmp(funcToPerform, "find") == 0 )
		{
			primaryKeyInString = strtok(NULL, "\n");
			primaryKeyInInt = (int)strtoll(primaryKeyInString, NULL, 10);

			tempNodeOffset = 0;
			printf("Searching for key: %d\n", primaryKeyInInt);
			getVal = searchForKey(primaryKeyInInt, rootNodeOffset, bTreeOrder, inputFile, &tempNodeOffset);
			//printf("\nReturned from search\n");

			if( (getVal == -1) && (tempNodeOffset != -2))
			{
				//means key not found or pointer to next level is being returned
				printf("Entry with key=%d does not exist\n", primaryKeyInInt);
			}
			else
			{
				printf("Entry with key=%d exists\n", primaryKeyInInt);
			}
		}
		else if( strcmp(funcToPerform, "print") == 0)
		{
			printBtree(rootNodeOffset, inputFile, bTreeOrder, 1);
		}

		
		
		if(count == 31)
		{
			//printf("\nBREAKING");
			break;
		}
		count++;
	}

	printf("\nHURRAY FINISH()\n\n");

}





void initializeNode_Read(bTreeNode *node, int bTreeOrder, FILE *inputFile, long rootNodeOffset)
{
	node->noOfKeys = 0;
	node->nodeKeys = (int*)calloc((bTreeOrder-1), sizeof(int));
	node->nodeChildOffsets = (long*)calloc(bTreeOrder, sizeof(long));

	if( rootNodeOffset != -1)
	{
		fseek(inputFile, rootNodeOffset, SEEK_SET);
		fread( &(node->noOfKeys), sizeof(int), 1, inputFile);
		fread( node->nodeKeys, sizeof(int), (bTreeOrder-1), inputFile);
		fread( node->nodeChildOffsets, sizeof(long), bTreeOrder, inputFile);
	}
}




void printBtree(long rootNodeOffset, FILE *inputFile, int bTreeOrder, int val)
{
	int  i = 0, j = 0;
	//bTreeNode rootNode;
	bTreeNode	node;
	//initializeNode_Read(&rootNode, bTreeOrder, inputFile, rootNodeOffset);
	
	
	printQueue *head = NULL, *temp = NULL;
	
	printQueue *first = (printQueue*)malloc(sizeof(printQueue));
	first->offset = rootNodeOffset;
	first->newLevel = 1;
	first->space = 0;
	first->next = NULL;
	
	head = first;
	
	while(head)
	{
		
		initializeNode_Read(&node, bTreeOrder, inputFile, head->offset);
		if( head->newLevel == 1)
		{
			printf("\n%d: ", val++, head->offset);
		}
		
		for( i = 0; i < (node.noOfKeys - 1); i++ )
		{
			printf("%d,", node.nodeKeys[i]);
		}
		if(node.noOfKeys > 0)
			printf( "%d", node.nodeKeys[(node.noOfKeys - 1)]);
		
		if( (head->space == 1) && (node.noOfKeys > 0))
		{
			printf(" ");
		}
		
		for( i = 0; i <= node.noOfKeys; i++ )
		{
			if(node.nodeChildOffsets[i]!=0)
			{
				printQueue *new = (printQueue*)malloc(sizeof(printQueue));
				new->offset = node.nodeChildOffsets[i];
				//printf("\nAdding offset to queue, node[%d]: %d", i, node.nodeChildOffsets[i]);
				if(head->newLevel == 1 && i == 0)
				{
					new->newLevel = 1;
				}
				else
				{
					new->newLevel = 0;
				}
				
				if( i<node.noOfKeys)
					new->space = 1;
				else
					new->space = 0;
				
				new->next = NULL;
				
				temp = head;
				while(temp->next !=NULL)
				{
					temp = temp->next;
				}
				temp->next = new;
				
			}
			
		}
		
		temp = head;
		head = head->next;
		if( temp!=NULL)
			free(temp);
	
	}
	
	
	
	
	//printf("\nPRINTING(): rootNodeOffset: %ld", rootNodeOffset);
	
/*	if( val!= 0 )
		printf("\n %d: ", val);
	else
		printf("  -  ");
	
	
	for( i = 0; i<rootNode.noOfKeys; i++)
	{
		if( i<(rootNode.noOfKeys-1) )
			printf("%d,",rootNode.nodeKeys[i]);
		else
			printf("%d",rootNode.nodeKeys[i]);
	}
	
	for( i =0; i<=rootNode.noOfKeys; i++)
	{
		if(rootNode.nodeChildOffsets[i]!=0)
		{
			if(i==0)
			{
				if( count == 14) printf("\nReading left child from pos: %ld", rootNode.nodeChildOffsets[i]);
				printBtree(rootNode.nodeChildOffsets[i], inputFile, bTreeOrder, (val+1));
			}
			else
			{
				if( count == 14) printf("\nReading RIGHT child from pos: %ld", rootNode.nodeChildOffsets[i]);
				printBtree(rootNode.nodeChildOffsets[i], inputFile, bTreeOrder, 0);
			}
		}
			
	}*/

}







long searchForKey(int primaryKeyInInt, long rootNodeOffset, int bTreeOrder, FILE *inputFile, long* nodeOffset)
{
	
	int s = 0;
	bTreeNode node;

	if(rootNodeOffset == -1)
	{
		*nodeOffset = -1;
		return(-1);
	}

	initializeNode_Read(&node, bTreeOrder, inputFile, rootNodeOffset);
	

	while( s< (node.noOfKeys) )
	{
		if( primaryKeyInInt == node.nodeKeys[s] )
		{
			*nodeOffset = -2;
			return(node.nodeChildOffsets[s]);
		}
		else if( primaryKeyInInt < node.nodeKeys[s])
			break;
		else
			s++;
	}

	if (node.nodeChildOffsets[s] != 0 )
		return(searchForKey(primaryKeyInInt, node.nodeChildOffsets[s], bTreeOrder, inputFile, nodeOffset));
	else
	{
		*nodeOffset = (long)&node;
		return(-1);
	}

}




void insertIntoBTree(long *rootNodeOffset, int primaryKeyInInt, int bTreeOrder, FILE *inputFile)
{
	int getVal = 0;
	long tempNodeOffset = 0;
	bTreeNode rootNode;

	if( *rootNodeOffset == -1)
	{	
		//printf("\nHere  ------------------------");
		initializeNode_Read(&rootNode, bTreeOrder, inputFile, -1);

		rootNode.noOfKeys = 1;
		rootNode.nodeKeys[0] = primaryKeyInInt;
		fwrite(rootNodeOffset, sizeof(long), 1, inputFile);
		*rootNodeOffset = ftell(inputFile);
		fseek(inputFile, 0, SEEK_SET);
		fwrite(rootNodeOffset, sizeof(long), 1, inputFile);
		writeToFile(&rootNode, rootNodeOffset, inputFile, bTreeOrder);
		//fwrite(&rootNode.noOfKeys, sizeof(int), 1, inputFile);
		//fwrite(&rootNode.nodeKeys, sizeof(int), (bTreeOrder-1), inputFile);
		//fwrite(&rootNode.nodeChildOffsets, sizeof(long), bTreeOrder, inputFile);
		return;
	}

	int totalKeys = 0;
	//printf("\nWhere  ++++++++++++++++++++++++++, rootNodeOffset: %d\n", *rootNodeOffset);
	fseek(inputFile, *rootNodeOffset, SEEK_SET);
	fread(&totalKeys, sizeof(int), 1, inputFile);

	if( totalKeys == (bTreeOrder-1) )
	{
		//printf("\nTotal Keys == BtreeOrder -1");
		
		bTreeNode newRootNode;
		initializeNode_Read(&newRootNode, bTreeOrder, inputFile, -1);

		newRootNode.nodeChildOffsets[0] = *rootNodeOffset;
		//*rootNodeOffset = newRootNode;
		//make newrightchild node as root node
		if( count == 13) printf("\nCalling Split Node");
		insertIntoBTree_SplitNode( &newRootNode, 0, bTreeOrder, rootNodeOffset, inputFile, 1, &primaryKeyInInt);
		//if( count == 13) printf("\nReturned from Splitting");
		//if( count == 13) printBtree(*rootNodeOffset, inputFile, bTreeOrder, 1);
		//if( count == 13) printf("\nCalling Non-Full");
		
		
		getVal = searchForKey(primaryKeyInInt, *rootNodeOffset, bTreeOrder, inputFile, &tempNodeOffset);

		if( (getVal == -1) && (tempNodeOffset != -2))
		{
			insertIntoBtree_NonSplitNode( &newRootNode, *rootNodeOffset, primaryKeyInInt, bTreeOrder, inputFile, rootNodeOffset);
			//if( count == 13) printf("\nReturned from Non-Full");
			if( count == 13) printBtree(*rootNodeOffset, inputFile, bTreeOrder, 1);
		}
			
			
			
		

	}
	else
	{
		initializeNode_Read(&rootNode, bTreeOrder, inputFile, *rootNodeOffset);
		//printf("\nROOT NO SPlit: .....");
		insertIntoBtree_NonSplitNode( &rootNode, *rootNodeOffset, primaryKeyInInt, bTreeOrder, inputFile, rootNodeOffset);
	}
}



void writeToFile(bTreeNode *node, long *rootNodeOffset, FILE *inputFile, int bTreeOrder)
{
	fseek(inputFile, *rootNodeOffset, SEEK_SET);
	//printf("\nWRITING(): offset: %ld,  node no of keys: %d", *rootNodeOffset, node->noOfKeys);
	fwrite(&(node->noOfKeys), sizeof(int), 1, inputFile);
	fwrite(node->nodeKeys, sizeof(int), (bTreeOrder-1), inputFile);
	fwrite(node->nodeChildOffsets, sizeof(long), bTreeOrder, inputFile);
}





void insertIntoBTree_SplitNode(bTreeNode *node, int position, int bTreeOrder, long *rootNodeOffset, FILE *inputFile, int isNewRoot, int *primaryKeyInInt)
{
	//bTreeNode *node = (bTreeNode*)nodeInLong;

	int midValue = 0, i = 0, newRightChildArrayPosition = 0, temp = 0 ,newKeyPosition = -1, s =0, j =0;
	long tempOffsetRightChild = 0, tempOffsetRootNode = 0;
	
	int tempKeyList[bTreeOrder];

	bTreeNode newRightNode, childNode;
	
	initializeNode_Read(&newRightNode, bTreeOrder, inputFile, -1);
	initializeNode_Read(&childNode, bTreeOrder, inputFile, node->nodeChildOffsets[position]);

	
	
	
	for( i = 0; i<childNode.noOfKeys; i++)
	{
		   
		if( (*primaryKeyInInt < childNode.nodeKeys[i] ) && (newKeyPosition == -1))
		{
			newKeyPosition = s;
			tempKeyList[s] = *primaryKeyInInt;
			s++;
		}
		
		
		tempKeyList[s] = childNode.nodeKeys[i];
		s++;		
	}
	//printf("\nS value--------: %d", s);
	
	/*for( i = 0; i<childNode.noOfKeys; i++)
	{
		if(*primaryKeyInInt < childNode.nodeKeys[i])
		{
			newKeyPosition = i;
			break;
		}		
	}
	
	
	if( (newKeyPosition!= -1) )//&& (childNode.nodeChildOffsets[0] == 0) )
	{
		temp = *primaryKeyInInt;
		*primaryKeyInInt = childNode.nodeKeys[newKeyPosition];
		childNode.nodeKeys[newKeyPosition] = temp;
	}
	*/

	midValue = (bTreeOrder/2);
	//if( count == 13) printf("\nMid-Value: %d", midValue);
	newRightNode.noOfKeys = ((bTreeOrder)- midValue -1);//((bTreeOrder-1)- midValue -1);
	if( count == 13) printf("\nNo of keys in right node: %d", newRightNode.noOfKeys);
	
	
	/*for( i = 0; i<(newRightNode.noOfKeys); i++)
	{
		newRightNode.nodeKeys[i] = tempKeyList[midValue+i+1];//childNode.nodeKeys[midValue+i+1];
		if( count == 13) printf("\nKeys in right node[%d]: %d", i, newRightNode.nodeKeys[i]);
	}*/
	for( i = 0; i<s; i++)
	{
		printf("\nTemp key array[%d]: %d", i, tempKeyList[i]);
	}
	
	j = 0;
	for( i = (midValue+1); i<(s); i++)
	{
		newRightNode.nodeKeys[j++] = tempKeyList[i];//childNode.nodeKeys[midValue+i+1];
		if( count == 13) printf("\nKeys in right node[%d]: %d", i, newRightNode.nodeKeys[i]);
	}
	newRightNode.noOfKeys = j;
	printf("\nNo of keys in right node: %d", newRightNode.noOfKeys);
	
	
	if(childNode.nodeChildOffsets[0] != 0)
	{
		for( i = 0; i<=(newRightNode.noOfKeys); i++)
		{
			newRightNode.nodeChildOffsets[i] = childNode.nodeChildOffsets[midValue+i];
		}
	}

	
	s = 0;
	for( i = 0; i<midValue; i++)
	{
		if(*primaryKeyInInt!= tempKeyList[i])//childNode.nodeKeys[i])
			s++;
	}
	
	childNode.noOfKeys = s;//midValue;
	//if( count == 13) printf("\nLeft Node has keys: %d", childNode.noOfKeys);
	
	
	for( i = (node->noOfKeys); i>position; i--)
	{
		if( count == 13) printf("\nCopying node->nodeChildOffsets[%d]: %d    to node->nodeChildOffsets[%d]: %d", i, node->nodeChildOffsets[i], (i+1), node->nodeChildOffsets[(i+1)]);
		node->nodeChildOffsets[i+1] = node->nodeChildOffsets[i];
		if( count == 13) printf("\nAFTER ---Copying node->nodeChildOffsets[%d]: %d    to node->nodeChildOffsets[%d]: %d", i, node->nodeChildOffsets[i], (i+1), node->nodeChildOffsets[(i+1)]);
		
	}

	//node->nodeChildOffsets[i+1] = (long)&newRightNode;
	newRightChildArrayPosition = (i+1);

	for( i =(node->noOfKeys-1); i>=position; i-- )
	{
		node->nodeKeys[i+1] = node->nodeKeys[i];
		if( count == 13) printf("\nRoot node keys shifting [%d]: %d,   noOfKeys: %d", (i+1), node->nodeKeys[i+1], node->noOfKeys);
	}
	i++;
	
	if( count == 13) printf("\nAfter shifting i: %d,   childNode.nodeKeys[midValue]: %d", i, childNode.nodeKeys[midValue]);
	node->nodeKeys[i] = tempKeyList[midValue];//childNode.nodeKeys[midValue];
	node->noOfKeys = (node->noOfKeys + 1);
	if( count == 13) printf("\nRoot Node key[%d]: %d, and noOfKeys: %d", (i), node->nodeKeys[i], node->noOfKeys);
	
	
	if( count == 13) printf("\nWriting left node at pos: %ld", node->nodeChildOffsets[position]);
	writeToFile(&childNode, &(node->nodeChildOffsets[position]), inputFile, bTreeOrder);
	
	fseek(inputFile, 0, SEEK_END);
	tempOffsetRightChild = ftell(inputFile);
	node->nodeChildOffsets[newRightChildArrayPosition] = tempOffsetRightChild;
	if( count == 13) printf("\nRoot Node has offset[%d]: %d", newRightChildArrayPosition, node->nodeChildOffsets[newRightChildArrayPosition]);

	
	if( count == 13) printf("\nWriting RIGHT node at pos: %ld", tempOffsetRightChild);
	if( count == 13) printf("\nRight node values with total keys: %d, ", newRightNode.noOfKeys);
	
	for(i=0; i<newRightNode.noOfKeys; i++)
	{
		if( count == 13) printf("\nKey[%d]: %d", i, newRightNode.nodeKeys[i]);
	}
	writeToFile(&newRightNode, &tempOffsetRightChild, inputFile, bTreeOrder);
	tempOffsetRootNode = ftell(inputFile);

	if( isNewRoot == 1)
	{
		*rootNodeOffset = tempOffsetRootNode;
		if( count == 13) printf("\nWriting NEWWWWW......_ RooT node at pos: %ld", tempOffsetRootNode);
		writeToFile(node, &tempOffsetRootNode, inputFile, bTreeOrder);
	}
	else
	{
		if( count == 13) printf("\nWriting OLD  .. RooT node at pos: %ld", *rootNodeOffset);
		writeToFile(node, rootNodeOffset, inputFile, bTreeOrder);
	}
	
	if( count == 13) printf("\nReturning from Split()");

}


void insertIntoBtree_NonSplitNode(bTreeNode *node, long offsetOfNode, int primaryKeyInInt, int bTreeOrder, FILE *inputFile, long *rootNodeOffset)
{
	
	
	int i = 0, getVal = 0;
	long tempNodeOffset = 0;
	long temp = -1;

	bTreeNode childNode, newNode;
	
	
	initializeNode_Read(&newNode, bTreeOrder, inputFile, offsetOfNode);



	if( newNode.nodeChildOffsets[0] == 0 )	//means the node is a leaf, it has no child
	{
		i = (newNode.noOfKeys - 1);
		
		//if( count == 13) printf("\nNON_SPLIT LEAF(): i = %d,   node->noOfKeys:  %d", i, newNode.noOfKeys);
		
		
		while( (i >= 0 ) && (newNode.nodeKeys[i] > primaryKeyInInt ) ) //shifting values > primarykey to one position right
		{
			if( count == 13) printf("\nInside While");
			newNode.nodeKeys[i+1] = newNode.nodeKeys[i];
			i--;
		}

		newNode.nodeKeys[i+1] = primaryKeyInInt;
		if( count == 13) printf("\nnode->nodeKeys[%d]: %d,   primaryKeyInInt: %d", (i+1), newNode.nodeKeys[i+1], primaryKeyInInt);
		newNode.noOfKeys = (newNode.noOfKeys + 1);
		if( count == 13) printf("\nNow node no of keys: %d", newNode.noOfKeys);

		//write to file
		//fseek(inputFile, offsetOfNode, SEEK_SET);
		if( count == 13) printf("\nWriting to file at pos: %ld", offsetOfNode);
		writeToFile(&newNode, &offsetOfNode, inputFile, bTreeOrder);

	}
	else
	{
		i = (node->noOfKeys - 1);
		while( (i >= 0 ) && (node->nodeKeys[i] > primaryKeyInInt ) ) //finding the right offset to put key value into
		{
			i--;
		}
		i++;

		if( count == 13) printf("\nNON_Split: NON LEAF()-----> node->nodeChildOffsets[%d]: %d", i, node->nodeChildOffsets[i]);
		//read from file the offset of node at i
		initializeNode_Read(&childNode, bTreeOrder, inputFile, node->nodeChildOffsets[i]);

		//childOffset = (bTreeNode*)node->nodeChildOffsets[i];

		if( childNode.noOfKeys == (bTreeOrder-1) )
		{
			if( count == 13) printf("\nNON_SPLIT_NONLEAF()------> Insert SPLIT() CALLING %%%%%%%");
			insertIntoBTree_SplitNode( node, i, bTreeOrder, rootNodeOffset, inputFile, 0, &primaryKeyInInt);
			if( count == 13) printf("\nNON_SPLIT_NONLEAF()------> Insert SPLIT() Returning ____----______()");
			//printBtree(*rootNodeOffset, inputFile, bTreeOrder, 1);
			
			if( count == 13) printf("\nprimaryKeyInInt: %d,   node->nodeKeys[%d]: %d", primaryKeyInInt, i, node->nodeKeys[i]);
			if( primaryKeyInInt > node->nodeKeys[i])
			{
				i++;
				if( count == 13) printf("\nIncreasing i = %d...., node->nodeKeys[%d]:%d,   node->nodeChildOffsets[i]: %d", i, i, node->nodeKeys[i], node->nodeChildOffsets[i]);
			}
		}
		
		if( count == 13) printf("\nNON_SPLIT_NONLEAF()------> CallING   Insert NON- Split()");
		
		getVal = searchForKey(primaryKeyInInt, *rootNodeOffset, bTreeOrder, inputFile, &tempNodeOffset);
		
		if( (getVal == -1) && (tempNodeOffset != -2))
		{
		
			insertIntoBtree_NonSplitNode( &childNode, node->nodeChildOffsets[i], primaryKeyInInt, bTreeOrder, inputFile, rootNodeOffset);
			//if( count == 13) printf("\nNON_SPLIT_NONLEAF()------>  RETURNING from   Insert NON- Split()");
		}
		
		
		
	}
}













int insertIntoBTree_Node(int primaryKeyInInt, long *rootNodeOffset, int bTreeOrder, long *LOffset, long *ROffset, int *keyToUpLevel, int isItRoot, FILE *inputFile)
{
	int getVal = 0, newKeyPosition = -1, tempArrCount = 0, tempKeyList[bTreeOrder], midValue = 0, tempPos = 0, i = 0, offsetPos = 0;
	long tempNodeOffset = 0, tempOffsetRightChild = 0;
	bTreeNode rootNode;

	if( *rootNodeOffset == -1)
	{	
		//printf("\nHere  ------------------------");
		initializeNode_Read(&rootNode, bTreeOrder, inputFile, -1);

		rootNode.noOfKeys = 1;
		rootNode.nodeKeys[0] = primaryKeyInInt;
		fwrite(rootNodeOffset, sizeof(long), 1, inputFile);
		*rootNodeOffset = ftell(inputFile);
		fseek(inputFile, 0, SEEK_SET);
		fwrite(rootNodeOffset, sizeof(long), 1, inputFile);
		writeToFile(&rootNode, rootNodeOffset, inputFile, bTreeOrder);
		//fwrite(&rootNode.noOfKeys, sizeof(int), 1, inputFile);
		//fwrite(&rootNode.nodeKeys, sizeof(int), (bTreeOrder-1), inputFile);
		//fwrite(&rootNode.nodeChildOffsets, sizeof(long), bTreeOrder, inputFile);
		return;
	}

	
	initializeNode_Read(&rootNode, bTreeOrder, inputFile, *rootNodeOffset);
	
		
	if( rootNode.nodeChildOffsets[0] == 0 )	//means the node is a leaf, it has no child
	{
		
		if( rootNode.noOfKeys == (bTreeOrder-1) )
		{
		
			bTreeNode newRootNode, newRightNode;
			initializeNode_Read(&newRootNode, bTreeOrder, inputFile, -1);
			initializeNode_Read(&newRightNode, bTreeOrder, inputFile, -1);

			newRootNode.nodeChildOffsets[0] = *rootNodeOffset;
		
			tempArrCount = 0;
			newKeyPosition = -1;
			
			for( i = 0; i<rootNode.noOfKeys; i++)
			{
				   
				if( (primaryKeyInInt < rootNode.nodeKeys[i] ) && (newKeyPosition == -1))
				{
					newKeyPosition = 1;
					tempKeyList[tempArrCount] = primaryKeyInInt;
					tempArrCount++;
				}
				
				
				tempKeyList[tempArrCount] = rootNode.nodeKeys[i];
				tempArrCount++;		
			}
			
			if( newKeyPosition == -1 )
			{
				newKeyPosition = 1;
				tempKeyList[tempArrCount] = primaryKeyInInt;
				tempArrCount++;
			}
				
				
				
			midValue = (bTreeOrder/2);
			newRootNode.nodeKeys[0] = tempKeyList[midValue];
			
			newKeyPosition = 0;
			//printf("\nCopying VALUEEEEEEEEEEEEEEEE.. 1: %d,  %d, root node key cpunt: %d", midValue, tempArrCount, rootNode.noOfKeys);
			for( i = (midValue+1); i<tempArrCount; i++)
			{
				//printf("\nCopying VALUEEEEEEEEEEEEEEEE.. 2 at pos : %d, val: %d", newKeyPosition, tempKeyList[i]);
				newRightNode.nodeKeys[newKeyPosition] = tempKeyList[i];
				newKeyPosition++;
			}
			
			
			for( i = 0; i<midValue; i++)
			{
				rootNode.nodeKeys[i] = tempKeyList[i];
			}
			
			
			//printf("\nCopying VALUEEEEEEEEEEEEEEEE.. 3");
			newRootNode.noOfKeys = 1;
			rootNode.noOfKeys = midValue;
			newRightNode.noOfKeys = newKeyPosition;
			
			
			//Writing original root to original place with less keys as child
			writeToFile(&rootNode, rootNodeOffset, inputFile, bTreeOrder);
			
			//Writing new right child
			fseek(inputFile, 0, SEEK_END);
			tempOffsetRightChild = ftell(inputFile);
			newRootNode.nodeChildOffsets[1] = tempOffsetRightChild;
			writeToFile(&newRightNode, &tempOffsetRightChild, inputFile, bTreeOrder);
			
			//Writing new Root node only if it is Root Node
			if( isItRoot == 1 )
			{
				tempOffsetRightChild = ftell(inputFile);	//treat tempOffsetRightChild as offset for root node
				writeToFile(&newRootNode, &tempOffsetRightChild, inputFile, bTreeOrder);
				*rootNodeOffset = tempOffsetRightChild;
			}
			
			
			
			*LOffset = newRootNode.nodeChildOffsets[0];
			*ROffset = newRootNode.nodeChildOffsets[1];
			*keyToUpLevel = tempKeyList[midValue];
			printf("\nKey to up level: %d", *keyToUpLevel);
			return(1);	//indicates it has split the node
		}
		
		
		//Now, root is not full plus it has no child
		i = (rootNode.noOfKeys - 1);
		
		while( (i >= 0 ) && (rootNode.nodeKeys[i] > primaryKeyInInt ) ) //shifting values > primarykey to one position right
		{
			rootNode.nodeKeys[i+1] = rootNode.nodeKeys[i];
			i--;
		}

		rootNode.nodeKeys[i+1] = primaryKeyInInt;
		
		rootNode.noOfKeys = (rootNode.noOfKeys + 1);
		writeToFile(&rootNode, rootNodeOffset, inputFile, bTreeOrder);
		
		return(0);	//indicates it did not split node
	}





	
	//If it reaches here, it means the root has child, so find position where to insert new key
	i = 0;
	while( i < (rootNode.noOfKeys) )
	{
		if( primaryKeyInInt < rootNode.nodeKeys[i])
			break;
		else
			i++;
	}
	offsetPos = i;
	
	getVal = insertIntoBTree_Node(primaryKeyInInt, &rootNode.nodeChildOffsets[i], bTreeOrder, LOffset, ROffset, keyToUpLevel, 0, inputFile);
	
	
	if( getVal == 0)
	{
		return(0);
	}
	else
	{
		//A return value indicates a child node has been split
		if( rootNode.noOfKeys == (bTreeOrder-1) )
		{
		
			bTreeNode newRootNode, newRightNode;
			initializeNode_Read(&newRootNode, bTreeOrder, inputFile, -1);
			initializeNode_Read(&newRightNode, bTreeOrder, inputFile, -1);

			newRootNode.nodeChildOffsets[0] = *rootNodeOffset;
		
			tempArrCount = 0;
			newKeyPosition = -1;
			for( i = 0; i<rootNode.noOfKeys; i++)
			{
				   
				if( (*keyToUpLevel < rootNode.nodeKeys[i] ) && (newKeyPosition == -1))
				{
					newKeyPosition = tempArrCount;
					tempKeyList[tempArrCount] = *keyToUpLevel;
					tempArrCount++;
				}
				
				
				tempKeyList[tempArrCount] = rootNode.nodeKeys[i];
				tempArrCount++;		
			}
		
			if( newKeyPosition == -1 )
			{
				newKeyPosition = 1;
				tempKeyList[tempArrCount] = *keyToUpLevel;
				tempArrCount++;
			}
			
			
			
			for( i = 0; i<tempArrCount; i++)
			{
				printf("\n temparr[%d]: %d", i, tempKeyList[i]);
			
			}
		
			midValue = (bTreeOrder/2);
			
			newRootNode.nodeKeys[0] = tempKeyList[midValue];
			
			newKeyPosition = 0;
			for( i = (midValue+1); i<tempArrCount; i++)
			{
				newRightNode.nodeKeys[newKeyPosition] = tempKeyList[i];
				newKeyPosition++;
			}
			
			
			for( i = 0; i<midValue; i++)
			{
				rootNode.nodeKeys[i] = tempKeyList[i];
			}
			
			
			tempArrCount = 0;
			for( i = (offsetPos+1); i<=rootNode.noOfKeys; i++)
			{
				if(rootNode.nodeChildOffsets[i]!=0)
				{
					newRightNode.nodeChildOffsets[tempArrCount] = rootNode.nodeChildOffsets[i];
					tempArrCount++;
				}
			}
			
			
			
			
			
			/*
			
			for( i = (node->noOfKeys); i>position; i--)
			{
				node->nodeChildOffsets[i+1] = node->nodeChildOffsets[i];				
			}
			
			newRightChildArrayPosition = (i+1);

			writeToFile(&childNode, &(node->nodeChildOffsets[position]), inputFile, bTreeOrder);			
			fseek(inputFile, 0, SEEK_END);
			tempOffsetRightChild = ftell(inputFile);
			
			node->nodeChildOffsets[newRightChildArrayPosition] = tempOffsetRightChild;
			
			*/
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			
			newRootNode.noOfKeys = 1;
			rootNode.noOfKeys = midValue;
			newRightNode.noOfKeys = newKeyPosition;
			
			
			rootNode.nodeChildOffsets[offsetPos+1] = *ROffset;

			//Writing original root to original place with less keys as child
			writeToFile(&rootNode, rootNodeOffset, inputFile, bTreeOrder);
			
			//Writing new right child
			fseek(inputFile, 0, SEEK_END);
			tempOffsetRightChild = ftell(inputFile);
			newRootNode.nodeChildOffsets[1] = tempOffsetRightChild;
			writeToFile(&newRightNode, &tempOffsetRightChild, inputFile, bTreeOrder);
			
			//Writing new Root node only if it is Root Node
			if( isItRoot == 1 )
			{
				tempOffsetRightChild = ftell(inputFile);	//treat tempOffsetRightChild as offset for root node
				writeToFile(&newRootNode, &tempOffsetRightChild, inputFile, bTreeOrder);
				*rootNodeOffset = tempOffsetRightChild;
			}
			
			
			*LOffset = newRootNode.nodeChildOffsets[0];
			*ROffset = newRootNode.nodeChildOffsets[1];
			*keyToUpLevel = tempKeyList[midValue];
			return(1);	//indicates it has split the node
		}
		
	
	


	
		//Now, node is not full plus it has no child
		i = (rootNode.noOfKeys - 1);
		
		while( (i >= 0 ) && (rootNode.nodeKeys[i] > *keyToUpLevel ) ) //shifting values > primarykey to one position right
		{
			rootNode.nodeKeys[i+1] = rootNode.nodeKeys[i];
			i--;
		}

		rootNode.nodeKeys[i+1] = *keyToUpLevel;
		printf("\nrootNode.nodeKeys[%d]: %d ", (i+1), rootNode.nodeKeys[i+1]);
		tempPos = (i+1);
		
		for( i = rootNode.noOfKeys; i>tempPos; i--)
		{
			rootNode.nodeChildOffsets[i+1] = rootNode.nodeChildOffsets[i];
		}

		rootNode.nodeChildOffsets[i] = *LOffset;
		rootNode.nodeChildOffsets[i+1] = *ROffset;
	
		rootNode.noOfKeys = (rootNode.noOfKeys + 1);
		writeToFile(&rootNode, rootNodeOffset, inputFile, bTreeOrder);
		
		return(0);	//indicates it did not split node
		
	}
	
	
}
	
	
	
	
	
	
	
	

/*int insertIntoBTreeLeaf(int primaryKeyInInt, long *rootNode, int bTreeOrder)
{
	
	int s = 0, i=0, newKeyPosition = 0, midValue = 0;
	int tempKeyList[bTreeOrder];

	//Create a new list one greater than noOfKeys and sort them
	//and add new key to the new list 
	for( i = 0; i<rootNode->noOfKeys; i++)
	{
		if(primaryKeyInInt < rootNode->nodeKeys[i])
		{
			newKeyPosition = s;
			tempKeyList[s] = primaryKeyInInt;
			s++;

		}
		
		tempKeyList[s] = rootNode->nodeKeys[i];
		s++;		
	}
	

	//if new list is smaller or equal to btreeOrder, then we are fine
	//no need to break
	//copy the new contents to node's keys and update offsets
	if( s < bTreeOrder )
	{
		rootNode->noOfKeys = (rootNode->noOfKeys + 1);
		for(i =0; i<rootNode->noOfKeys; i++)
		{
			rootNode->nodeKeys[i] = tempKeyList[i];
		}
		return(1);
	}
	

	//this means the node is now equal to btree order i.e. one extra node is there

	midValue = bTreeOrder/2;
	bTreeNode rightNode;
	initializeNode(rightNode, bTreeOrder);
	rightNode->noOfKeys = bTreeOrder - (midValue + 1);

	for( i = 0; i<rightNode->noOfKeys; i++)
	{
		rightNode->nodeKeys[i] = tempKeyList[midValue+1+i];
	}

	reInitializeNode(rootNode, bTreeOrder);
	rootNode->noOfKeys = midValue;
	for( i = 0; i < rootNode->noOfKeys; i++)
	{
		rootNode->nodeKeys[i] = tempKeyList[i];
	}


	insertSplitChild(tempKeyList[midValue], &rootNode, &rightchild);
}


void insertSplitChild(int valueToInsert, long *leftChildOffset, long *rightChildOffset)
{

}*/