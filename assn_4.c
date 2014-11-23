#include<stdio.h>

typdef struct
{
	int noOfKeys;
	int *nodeKeys;
	long *nodeChildOffsets;

}bTreeNode;

long searchForKey(int primaryKeyInInt, bTreeNode node, int bTreeOrder, long* nodeOffset);
int insertIntoBTreeLeaf(int primaryKeyInInt, long *rootNode, int bTreeOrder);
void initializeNode(bTreeNode *node, int bTreeOrder);
void reInitializeNode(bTreeNode *node, int bTreeOrder)


int main(int argc, char *argv[])
{
	 char *inputFileName, readLine[100], *funcToPerform = NULL, *primaryKeyInString = NULL;
	 int bTreeOrder = 0;
	 int primaryKeyInInt = 0, getVal = 0;
	 bTreeNode rootNode;
	 FILE *inputFile = NULL;
	 long rootNodeOffset = 0, tempNodeOffset = 0;


	inputFileName = argv[1];
	bTreeOrder = argv[2];

	initializeNode(rootNode, bTreeOrder);

	funcToPerform = 	(char*)malloc(sizeof(char)*100);
	primaryKeyInString= (char*)malloc(sizeof(char)*100);
	

	if( (inputFile = fopen(inputFileName, "r+b")) == NULL )
	{
		//File doesn't exist, Open in write mode
		inputFile = fopen(inputFileName, "w+b");
		//rootNodeOffset = -1;
		rootNodeOffset = &rootNode;

	}
	else
	{
		//read the index file data into btreeNode
		//but first read root node offset
		fread(&rootNodeOffset, sizeof(long), 1, inputFile);
	}


	while( (fgets(&readLine, 100, stdin)) != NULL )
	{
		funcToPerform = strtok(readLine, " \n");
		primaryKeyInString = strtok(NULL, "\n");

		primaryKeyInInt = (int)strtoll(primaryKeyInString, NULL, 10);

		getVal = searchForKey(primaryKeyInInt, &rootNode, bTreeOrder, &tempNodeOffset);

		if( (getVal == -1) && (tempNodeOffset != -2))
		{
			//means key not found or pointer to next level is being returned
			insertIntoBTree(primaryKeyInInt, &rootNode, bTreeOrder);
		}
		else
		{
			printf("Entry with key=%d already exists\n", primaryKeyInInt);
		}
		
	}


}


void initializeNode(bTreeNode *node, int bTreeOrder)
{
	node->noOfKeys = 0;
	node->nodeKeys = (int*)calloc((bTreeOrder-1), sizeof(int));
	node->nodeChildOffsets = (long*)calloc(bTreeOrder, sizeof(long));
}


void reInitializeNode(bTreeNode *node, int bTreeOrder)
{
	int i = 0;
	node->noOfKeys = 0;


	for( i = 0; i < (bTreeOrder-1); i++)
	{
		node->nodeKeys[i] = 0;
		node->nodeChildOffsets[i] = 0;
	}
	node->nodeChildOffsets[i] = 0;
}



long searchForKey(int primaryKeyInInt, long* rootNode, int bTreeOrder, long* nodeOffset)
{
	
	int s = 0;

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
		return(searchForKey(primaryKeyInInt, node.nodeChildOffsets[s], bTreeOrder));
	else
	{
		*nodeOffset = rootNode;
		return(-1);
	}

}




void insertIntoBTree(long *rootNode, int primaryKeyInInt)
{

	if( rootNode->noOfKeys == (bTreeOrder-1) )
	{
		bTreeNode newRootNode;
		initializeNode(newRootNode, bTreeOrder);

		newRootNode.nodeChildOffsets[0] = rootNode;
		//make newrightchild node as root node

		insertIntoBTree_SplitNode(&newRootNode, 1);
		insertIntoBtree_NonSplitNode(&newRootNode, primaryKeyInInt);

	}
	else
		insertIntoBtree_NonSplitNode(&rootNode, primaryKeyInInt);
}



void insertIntoBTree_SplitNode(long *node, int position)
{

}

void insertIntoBtree_NonSplitNode(long *node, int primaryKeyInInt)
{

	int i = 0;
	if( node->nodeChildOffsets[0] == 0 )	//means the node is a leaf, it has no child
	{
		i = (node->noOfKeys - 1);
		while( (i >= 0 ) && (node->nodeKeys[i] > primaryKeyInInt ) )
		{
			node->nodeKeys[i+1] = node->nodeKeys[i];
			i--;
		}

		node->nodeKeys[i+1] = primaryKeyInInt;
		node->noOfKeys = (node->noOfKeys + 1);
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