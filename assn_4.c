#include<stdio.h>

typdef struct
{
	int noOfKeys;
	int *nodeKeys;
	long *nodeChildOffsets;

}bTreeNode;

long searchForKey(int primaryKeyInInt, bTreeNode node, int bTreeOrder, long* nodeOffset);
int insertIntoBTreeLeaf(int primaryKeyInInt, long *rootNode, int bTreeOrder);


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



int insertIntoBTreeLeaf(int primaryKeyInInt, long *rootNode, int bTreeOrder)
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
		rightNode->nodeKeys[i] = tempKeyList[midValue+i];
	}

	initializeNode(rootNode, bTreeOrder);
	rootNode->noOfKeys = midValue;


}