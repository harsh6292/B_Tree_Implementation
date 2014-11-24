#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct
{
	int noOfKeys;
	int *nodeKeys;
	long *nodeChildOffsets;

}bTreeNode;

long searchForKey(int primaryKeyInInt, long rootNodeInLong, int bTreeOrder, long* nodeOffset);
void insertIntoBTree(long rootNodeInLong, int primaryKeyInInt, int bTreeOrder);
void initializeNode(bTreeNode *node, int bTreeOrder);
void insertIntoBTree_SplitNode(long nodeInLong, int position, int bTreeOrder);
void insertIntoBtree_NonSplitNode(long nodeInLong, int primaryKeyInInt, int bTreeOrder);


int main(int argc, char *argv[])
{
	 char *inputFileName, *readLine, *funcToPerform = NULL, *primaryKeyInString = NULL;
	 int bTreeOrder = 0;
	 int primaryKeyInInt = 0, getVal = 0;
	 bTreeNode rootNode;
	 FILE *inputFile = NULL;
	 long rootNodeOffset = 0, tempNodeOffset = 0;


	inputFileName = argv[1];
	bTreeOrder = atoi(argv[2]);

	initializeNode(&rootNode, bTreeOrder);

	funcToPerform = 	(char*)malloc(sizeof(char)*100);
	primaryKeyInString= (char*)malloc(sizeof(char)*100);
	readLine = (char*)malloc(sizeof(char)*100);
	

	if( (inputFile = fopen(inputFileName, "r+b")) == NULL )
	{
		//File doesn't exist, Open in write mode
		inputFile = fopen(inputFileName, "w+b");
		//rootNodeOffset = -1;
		rootNodeOffset = (long)&rootNode;

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
		
		if( strcmp(funcToPerform, "add") == 0 )
		{
			primaryKeyInString = strtok(NULL, "\n");

			primaryKeyInInt = (int)strtoll(primaryKeyInString, NULL, 10);

			tempNodeOffset = 0;
			getVal = searchForKey(primaryKeyInInt, rootNodeOffset, bTreeOrder, &tempNodeOffset);

			if( (getVal == -1) && (tempNodeOffset != -2))
			{
				//means key not found or pointer to next level is being returned
				insertIntoBTree(rootNodeOffset, primaryKeyInInt, bTreeOrder);
			}
			else
			{
				printf("Entry with key=%d already exists\n", primaryKeyInInt);
			}
		}
		else if( strcmp(funcToPerform, "find") == 0 )
		{
			tempNodeOffset = 0;
			getVal = searchForKey(primaryKeyInInt, rootNodeOffset, bTreeOrder, &tempNodeOffset);

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

		
	}


}


void initializeNode(bTreeNode *node, int bTreeOrder)
{
	node->noOfKeys = 0;
	node->nodeKeys = (int*)calloc((bTreeOrder-1), sizeof(int));
	node->nodeChildOffsets = (long*)calloc(bTreeOrder, sizeof(long));
}



long searchForKey(int primaryKeyInInt, long rootNodeInLong, int bTreeOrder, long* nodeOffset)
{
	
	int s = 0;

	bTreeNode *rootNode = (bTreeNode*)rootNodeInLong;

	while( s< (rootNode->noOfKeys) )
	{
		if( primaryKeyInInt == rootNode->nodeKeys[s] )
		{
			*nodeOffset = -2;
			return(rootNode->nodeChildOffsets[s]);
		}
		else if( primaryKeyInInt < rootNode->nodeKeys[s])
			break;
		else
			s++;
	}

	if (rootNode->nodeChildOffsets[s] != 0 )
		return(searchForKey(primaryKeyInInt, rootNode->nodeChildOffsets[s], bTreeOrder, nodeOffset));
	else
	{
		*nodeOffset = (long)rootNode;
		return(-1);
	}

}




void insertIntoBTree(long rootNodeInLong, int primaryKeyInInt, int bTreeOrder)
{

	bTreeNode *rootNode = (bTreeNode*)rootNodeInLong;

	if( rootNode->noOfKeys == (bTreeOrder-1) )
	{
		bTreeNode newRootNode;
		initializeNode(&newRootNode, bTreeOrder);

		newRootNode.nodeChildOffsets[0] = (long)rootNode;
		//make newrightchild node as root node

		insertIntoBTree_SplitNode( (long)&newRootNode, 1, bTreeOrder);
		insertIntoBtree_NonSplitNode( (long)&newRootNode, primaryKeyInInt, bTreeOrder);

	}
	else
		insertIntoBtree_NonSplitNode( (long)&rootNode, primaryKeyInInt, bTreeOrder);
}



void insertIntoBTree_SplitNode(long nodeInLong, int position, int bTreeOrder)
{
	bTreeNode *node = (bTreeNode*)nodeInLong;

	int midValue = 0, i = 0;
	bTreeNode *childNode = NULL;
	bTreeNode newRightNode;
	
	initializeNode(&newRightNode, bTreeOrder);
	midValue = (bTreeOrder/2);
	newRightNode.noOfKeys = midValue;

	childNode = (bTreeNode*)node->nodeChildOffsets[position];

	for( i = 0; i<midValue; i++)
	{
		newRightNode.nodeKeys[i] = childNode->nodeKeys[midValue+i+1];
	}

	if(childNode->nodeChildOffsets[0] != 0)
	{
		for( i = 0; i<midValue; i++)
		{
			newRightNode.nodeChildOffsets[i] = childNode->nodeChildOffsets[midValue+i];
		}
	}

	childNode->noOfKeys = midValue;

	for( i = (node->noOfKeys+1); i>position; i--)
	{
		node->nodeChildOffsets[i+1] = node->nodeChildOffsets[i];
	}

	node->nodeChildOffsets[i+1] = (long)&newRightNode;

	for( i =(node->noOfKeys-1); i>=position; i-- )
	{
		node->nodeKeys[i+1] = node->nodeKeys[i];
	}

	node->nodeKeys[i] = childNode->nodeKeys[midValue];
	node->noOfKeys = (node->noOfKeys + 1);

}

void insertIntoBtree_NonSplitNode(long nodeInLong, int primaryKeyInInt, int bTreeOrder)
{

	int i = 0;
	bTreeNode *childOffset = NULL;
	bTreeNode *node = (bTreeNode*)nodeInLong;

	if( node->nodeChildOffsets[0] == 0 )	//means the node is a leaf, it has no child
	{
		i = (node->noOfKeys - 1);
		while( (i >= 0 ) && (node->nodeKeys[i] > primaryKeyInInt ) ) //shifting values > primarykey to one position right
		{
			node->nodeKeys[i+1] = node->nodeKeys[i];
			i--;
		}

		node->nodeKeys[i+1] = primaryKeyInInt;
		node->noOfKeys = (node->noOfKeys + 1);

		//write to file
	}
	else
	{
		i = (node->noOfKeys - 1);
		while( (i >= 0 ) && (node->nodeKeys[i] > primaryKeyInInt ) ) //finding the right offset to put key value into
		{
			i--;
		}
		i++;

		//read from file the offset of node at i
		childOffset = (bTreeNode*)node->nodeChildOffsets[i];

		if( childOffset->noOfKeys == (bTreeOrder-1) )
		{
			insertIntoBTree_SplitNode( (long)node, i, bTreeOrder);
			if( primaryKeyInInt > node->nodeKeys[i])
				i++;
		}

		insertIntoBtree_NonSplitNode( (long)childOffset, primaryKeyInInt, bTreeOrder);
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