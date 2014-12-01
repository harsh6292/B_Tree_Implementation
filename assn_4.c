#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct
{
	int 	noOfKeys;
	int 	*nodeKeys;
	long 	*nodeChildOffsets;
}bTreeNode;


struct printQueue
{
	long 	offset;
	int 	newLevel;
	int 	space;
	struct 	printQueue *next;
};


void initializeNode_Read(bTreeNode *, int, FILE *, long);
long searchForKey(int, long, int, FILE *, long*);
void writeToFile(bTreeNode *, long *, FILE *, int);
void printBtree(long rootNodeOffset, FILE *inputFile, int bTreeOrder, int val);
int  insertIntoBTree_Node(int , long *, int , long *, long *, int *, int , FILE *);
void create_Init_Temp_Array(int *, int *, bTreeNode* , int );


int count = 0;


int main(int argc, char *argv[])
{
	char 	*inputFileName, *readLine, *funcToPerform = NULL, *primaryKeyInString = NULL;
	int 	bTreeOrder = 0, primaryKeyInInt = 0, getVal = 0, keyToUpLevel = 0, i =0;
	long 	rootNodeOffset = 0, tempNodeOffset = 0, LOffset = 0, ROffset = 0;
	FILE *inputFile = NULL;


	inputFileName 	= argv[1];
	bTreeOrder 		= atoi(argv[2]);

	
	funcToPerform 		= 	(char*)malloc(sizeof(char)*100);
	primaryKeyInString	= 	(char*)malloc(sizeof(char)*100);
	readLine 			= 	(char*)malloc(sizeof(char)*100);
	

	if( (inputFile = fopen(inputFileName, "r+b")) == NULL )
	{
		//File doesn't exist, Open in write mode
		inputFile = fopen(inputFileName, "w+b");
		rootNodeOffset = -1;
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
		
		if( (funcToPerform[0] == 'a') && (funcToPerform[1] == 'd') && (funcToPerform[2] == 'd') )
		{
			primaryKeyInString = strtok(NULL, "\n");
			primaryKeyInInt = (int)strtoll(primaryKeyInString, NULL, 10);

			tempNodeOffset = 0;
			
			//Search for key if it exists, then only insert
			getVal = searchForKey(primaryKeyInInt, rootNodeOffset, bTreeOrder, inputFile, &tempNodeOffset);
			
			//if search returns -1 and simultaneously no offset of child is found, it means key doesn't not exist
			//add key to B-Tree
			if( (getVal == -1) && (tempNodeOffset != -2))
			{				
				
				//Insert primaryKeyInInt into B-Tree stored in file inputFile and send it offset of root in B-tree
				insertIntoBTree_Node(primaryKeyInInt, &rootNodeOffset, bTreeOrder, &LOffset, &ROffset, &keyToUpLevel, 1, inputFile);
			}
			else
			{
				//Search returned a value and offset indicating key exists in file, so no need to add again
				printf("Entry with key=%d already exists\n", primaryKeyInInt);
			}
		}
		else if( (funcToPerform[0] == 'f') && (funcToPerform[1] == 'i') && (funcToPerform[2] == 'n') && (funcToPerform[3] == 'd') )
		{
			primaryKeyInString = strtok(NULL, "\n");
			primaryKeyInInt = (int)strtoll(primaryKeyInString, NULL, 10);

			tempNodeOffset = 0;
			
			//Search for key if it exists, then only insert
			getVal = searchForKey(primaryKeyInInt, rootNodeOffset, bTreeOrder, inputFile, &tempNodeOffset);
			
			
			//if search returns -1 and simultaneously no offset of child is found, it means key doesn't not exist
			//Print that no key is found in file
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
		else if( (funcToPerform[0] == 'p') && (funcToPerform[1] == 'r') && (funcToPerform[2] == 'i') && (funcToPerform[3] == 'n') && (funcToPerform[4] == 't'))
		{
			//Print the B-tree
			printBtree(rootNodeOffset, inputFile, bTreeOrder, 1);
		}
		else if( (funcToPerform[0] == 'e') && (funcToPerform[1] == 'n') && (funcToPerform[2] == 'd') )
		{
			fseek(inputFile, 0, SEEK_SET);
			fwrite(&rootNodeOffset, sizeof(long), 1, inputFile);
			
			return(1);
		}

		count++;
	}
}





void initializeNode_Read(bTreeNode *node, int bTreeOrder, FILE *inputFile, long rootNodeOffset)
{
	node->noOfKeys = 0;
	node->nodeKeys = (int*)calloc((bTreeOrder-1), sizeof(int));
	node->nodeChildOffsets = (long*)calloc(bTreeOrder, sizeof(long));

	//If root node offset is not -1, it means we have to fill the node with values
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
	bTreeNode	node;
	struct printQueue *head = NULL, *temp = NULL;
	struct printQueue *first = (struct printQueue*)malloc(sizeof(struct printQueue));
	
	
	first->offset = rootNodeOffset;
	first->newLevel = 1;
	first->space = 1;
	first->next = NULL;
	
	head = first;
	
	while(head)
	{
		
		initializeNode_Read(&node, bTreeOrder, inputFile, head->offset);
		if(val == 1)
		{
			printf("%2d: ", val++, head->offset);
		}
		else if( (head->newLevel == 1) && (head->space == 1) )
		{
			printf("\n%2d: ", val++, head->offset);
		}
		
		for( i = 0; i < (node.noOfKeys - 1); i++ )
		{
			printf("%d,", node.nodeKeys[i]);
		}
		if(node.noOfKeys > 0)
			printf( "%d ", node.nodeKeys[(node.noOfKeys - 1)]);
		
		for( i = 0; i <= node.noOfKeys; i++ )
		{
			if(node.nodeChildOffsets[i]!=0)
			{
				struct printQueue *newElement = (struct printQueue*)malloc(sizeof(struct printQueue));
				newElement->offset = node.nodeChildOffsets[i];
				
				if(head->newLevel == 1 && i == 0)
				{
					newElement->newLevel = 1;
				}
				else
				{
					newElement->newLevel = 0;
				}
				
				newElement->space = 1;
				newElement->next = NULL;
				
				temp = head;
				while(temp->next !=NULL)
				{
					temp = temp->next;
				}
				temp->next = newElement;	
			}	
		}
		temp = head;
		head = head->next;
		if( temp!=NULL)
			free(temp);
	}
	printf("\n");

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





void writeToFile(bTreeNode *node, long *rootNodeOffset, FILE *inputFile, int bTreeOrder)
{
	fseek(inputFile, *rootNodeOffset, SEEK_SET);
	fwrite(&(node->noOfKeys), sizeof(int), 1, inputFile);
	fwrite(node->nodeKeys, sizeof(int), (bTreeOrder-1), inputFile);
	fwrite(node->nodeChildOffsets, sizeof(long), bTreeOrder, inputFile);
}



void create_Init_Temp_Array(int *tempArray, int *sizeOfArray, bTreeNode* node, int keyToInsert)
{
	int tempArrCount = 0, newKeyPosition = -1, i =0;
			
	for( i = 0; i<node->noOfKeys; i++)
	{
			   
		if( (keyToInsert < node->nodeKeys[i] ) && (newKeyPosition == -1))
		{
			newKeyPosition = 1;
			tempArray[tempArrCount] = keyToInsert;
			tempArrCount++;
		}
				
		tempArray[tempArrCount] = node->nodeKeys[i];
		tempArrCount++;		
	}
			
	if( newKeyPosition == -1 )
	{
		newKeyPosition = 1;
		tempArray[tempArrCount] = keyToInsert;
		tempArrCount++;
	}
	
	*sizeOfArray = tempArrCount;
}







int insertIntoBTree_Node(int primaryKeyInInt, long *rootNodeOffset, int bTreeOrder, long *LOffset, long *ROffset, int *keyToUpLevel, int isItRoot, FILE *inputFile)
{
	int getVal = 0, newKeyPosition = -1, tempArrCount = 0, tempKeyList[bTreeOrder], midValue = 0, tempPos = 0, i = 0, offsetPos = 0;
	int tempOffsetCount = 0;
	long tempNodeOffset = 0, tempOffsetRightChild = 0, tempChildOffsetList[bTreeOrder+1];
	bTreeNode rootNode;

	
	//If root doesn't exist, create it and point root node offset to it
	if( *rootNodeOffset == -1)
	{	
		//Initialize root node i.e allocate memory for its key array and offset array
		initializeNode_Read(&rootNode, bTreeOrder, inputFile, -1);

		rootNode.noOfKeys = 1;
		rootNode.nodeKeys[0] = primaryKeyInInt;

		//write garbage value of root first
		fwrite(rootNodeOffset, sizeof(long), 1, inputFile);
		//get the length of long a system takes
		*rootNodeOffset = ftell(inputFile);
		
		//again seek to start of file and write root offset again, this time correct value
		fseek(inputFile, 0, SEEK_SET);
		fwrite(rootNodeOffset, sizeof(long), 1, inputFile);
		
		//now write the updated root node to file
		writeToFile(&rootNode, rootNodeOffset, inputFile, bTreeOrder);
		return(1);
	}

	
	//If root exists, then initialize the rrot by seeking to root node offset
	initializeNode_Read(&rootNode, bTreeOrder, inputFile, *rootNodeOffset);
	
	
	if( rootNode.nodeChildOffsets[0] == 0 )	//means the node is a leaf, it has no child
	{

		//if the root node itself is full, then split
		if( rootNode.noOfKeys == (bTreeOrder-1) )
		{
		
			//create and initialize two nodes, one root and one right node
			bTreeNode newRootNode, newRightNode;
			initializeNode_Read(&newRootNode, bTreeOrder, inputFile, -1);
			initializeNode_Read(&newRightNode, bTreeOrder, inputFile, -1);

			//make the left child of new root the old root
			newRootNode.nodeChildOffsets[0] = *rootNodeOffset;
		
			tempArrCount = 0;
			newKeyPosition = -1;
			
			
			//create a temp array containing the new key to be inserted
			create_Init_Temp_Array(&tempKeyList[0], &tempArrCount, &rootNode, primaryKeyInInt);
			
			//get a middle value and save the key at that pos in temp array to new root node
			midValue = (bTreeOrder/2);
			newRootNode.nodeKeys[0] = tempKeyList[midValue];
			
			
			//copy the values greater than mid value to right node
			newKeyPosition = 0;
			for( i = (midValue+1); i<tempArrCount; i++)
			{
				newRightNode.nodeKeys[newKeyPosition] = tempKeyList[i];
				newKeyPosition++;
			}
			
			
			//copy the values smaller than mid value to left/original node
			for( i = 0; i<midValue; i++)
			{
				rootNode.nodeKeys[i] = tempKeyList[i];
			}
			
			
			//update no of keys of each node
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
			
			return(1);	//indicates it has split the node
		}
		
		
		//Now, root is not full plus it has no child, so shift its keys to right
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
	
	
	
	//call the proper node child so that new value is inserted into proper child
	getVal = insertIntoBTree_Node(primaryKeyInInt, &rootNode.nodeChildOffsets[i], bTreeOrder, LOffset, ROffset, keyToUpLevel, 0, inputFile);
	
	
	
	
	if( getVal == 0)
	{
		//it means its child has accommodated new key and didn't split, hence this parent node has to do nothing
		return(0);
	}
	else
	{
		//If it reaches here indicates a child node has been split, hence parent has to accommodate a new key sent by child
		if( rootNode.noOfKeys == (bTreeOrder-1) )
		{
			//if parent is also full, then take this path
		
		
			bTreeNode newRootNode, newRightNode;
			initializeNode_Read(&newRootNode, bTreeOrder, inputFile, -1);
			initializeNode_Read(&newRightNode, bTreeOrder, inputFile, -1);

			newRootNode.nodeChildOffsets[0] = *rootNodeOffset;
		
			
			//Create a temporary array of keys so that splitting occurs using the new key value too
			tempArrCount = 0;
			newKeyPosition = -1;
			
			create_Init_Temp_Array(&tempKeyList[0], &tempArrCount, &rootNode, *keyToUpLevel);
			
			
			//create a temp child offset list that will contain all previous offsets plus the new right offset
			tempOffsetCount = 0;
			for( i = 0; i<= offsetPos; i++)
			{
				tempChildOffsetList[tempOffsetCount++] = rootNode.nodeChildOffsets[i];
			}
			tempChildOffsetList[tempOffsetCount++] = *ROffset;
			for( i = (offsetPos+1); i<= rootNode.noOfKeys; i++)
			{
				tempChildOffsetList[tempOffsetCount++] = rootNode.nodeChildOffsets[i];
			}
			
			
		
			//get middle value and insert it into a new node
			midValue = (bTreeOrder/2);	
			newRootNode.nodeKeys[0] = tempKeyList[midValue];
			
			
			//copy keys greater than mid value into new right child from temp array
			newKeyPosition = 0;
			for( i = (midValue+1); i<tempArrCount; i++)
			{
				newRightNode.nodeKeys[newKeyPosition] = tempKeyList[i];
				newKeyPosition++;
			}
			
			
			
			//Copying keys less than mid value into left child from temp array
			for( i = 0; i<midValue; i++)
			{
				rootNode.nodeKeys[i] = tempKeyList[i];
			}
			
			//Now copying child offsets into left child from temp child offsets array
			for( i = 0; i<= midValue; i++)
			{
				rootNode.nodeChildOffsets[i] = tempChildOffsetList[i];
			}
			
		
			//copy child offsets greater than mid value position into new right child offsets
			tempArrCount = 0;
			for( i = (midValue+1); i<tempOffsetCount; i++)
			{
				newRightNode.nodeChildOffsets[tempArrCount++] = tempChildOffsetList[i];
			}
			
			
			//Update all node's key count
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
			return(1);	//indicates it has split the node
		}
		
	

	
		//Now, node is not full plus it has no child
		i = (rootNode.noOfKeys - 1);
		
		//shifting values > primarykey to one position right
		while( (i >= 0 ) && (rootNode.nodeKeys[i] > *keyToUpLevel ) ) 
		{
			rootNode.nodeKeys[i+1] = rootNode.nodeKeys[i];
			i--;
		}

		rootNode.nodeKeys[i+1] = *keyToUpLevel;
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
