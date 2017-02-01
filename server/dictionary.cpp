#include "dictionary.h"

int trieSize = 1;

node::node()
{
    int i;
    wordEnd = visited = 0;
    for(i=0;i<26;i++) children[i] = NULL;
}

node::~node()
{
    int i;
    for(i=0;i<26;i++)
     if(children[i])
      delete children[i];
}

void node::expandTrie(char *word)
{
    int letter = word[0] - 'a';
    if(word[0] == 0)
    {
        wordEnd = 1;
        return;
    }
    if(children[letter]==NULL)
    {
        children[letter] = new node;
        trieSize++;
    }
    children[letter]->expandTrie(word+1);
}

node* node::step(char letter)
{
    return(children[letter-'a']);
}

void node::setVisited()
{
        visited = 1;
}

void node::clearVisited()
{
        visited = 0;
}

int node::getVisited()
{
        return visited;
}

int node::isWord()
{
        return wordEnd;
}

wordList::wordList(int numberOfWords)
{
    int i;
    listSize = numberOfWords;

	
	printf("size :%d\n", listSize);

    list = (char**)malloc(sizeof(char*)*listSize);

	printf("11111111111111111111111111111\n");
	printf("%d\n", list);

    for(i=0;i<listSize;i++) list[i] = NULL;
printf("22222222222222222222222222222222222222\n");
}

wordList::~wordList()
{
	int i;
    for(i=0;i<listSize;i++)
     if(list[i] != NULL)
      free(list[i]);
    free(list);
	list = NULL;
}

void wordList::setWord(char *word,int position)
{
    int length = strlen(word);
    if(list[position] != NULL) free(list[position]);
	
	

    list[position] = (char*)malloc(sizeof(char)*(length+1));

	//printf("yer aldik!!!!!\n");

    memcpy(list[position],word,sizeof(char)*length+1);

	//printf("kelime: %s\n", list[position]);
}

void wordList::printSorted()
{
    FILE *f;
    int i,j,b,t,*index = (int*)malloc(sizeof(int)*listSize);
    for(i=0;i<listSize;i++) index[i] = i;
    for(i=0;i<listSize;i++)
    {
        b = i;
        for(j=i+1;j<listSize;j++)
         if(strlen(list[index[j]]) > strlen(list[index[b]])) b=j;
        t = index[b];
        index[b] = index[i];
        index[i] = t;
    }
    f = fopen("allSorted.list","wt");
    for(i=0;i<listSize;i++)
     fprintf(f,"%s\n",list[index[i]]);
    fclose(f);
}

int wordList::binarySearch(char *word)
{
    return(search(word,0,listSize-1));
}

int wordList::search(char *word,int left,int right)
{
    int result,middle = (left+right)/2;
    if(left > right) return 0;
    result = strcmp(word,list[middle]);
    if(result < 0) return(search(word,left,middle-1));
    if(result > 0) return(search(word,middle+1,right));
    return 1;
}

searchDictionary::searchDictionary()
{
    dictionary = fopen("word.list","rt");
    createTrie();
}

searchDictionary::~searchDictionary()
{
    fclose(dictionary);
}

int searchDictionary::linearSearch(char *word)
{
    char w[110];
    rewind(dictionary);
    while(fgets(w,100,dictionary)!= NULL)
    {
        w[strlen(w)-1] = 0;
     	if(strcmp(w,word)==0) return 1;
    }
    return 0;
}

char** searchDictionary::createBoard()
{
    int i,j,a=m+2,b=n+2;
    char **board = (char**)malloc(sizeof(char*)*a);
    for(i=0;i<a;i++) board[i] = (char*)malloc(sizeof(char)*b);
    for(i=0;i<b;i++) board[0][i] = 1;
    for(i=0;i<b;i++) board[a-1][i] = 1;
    for(i=0;i<a;i++) board[i][0] = 1;
    for(i=0;i<a;i++) board[i][b-1] = 1;
    return board;
}

void searchDictionary::destroyBoard(char **board)
{
    int i,a=m+2,b=n+2;
    for(i=0;i<a;i++) free(board[i]);
    free(board);
}

void searchDictionary::dfs(int a,int b,node *c)
{
    int i,j;
    if(mark[a][b] == 1 || c == NULL) return;
    mark[a][b] = 1;

    c->setVisited();
    for(i=-1;i<=1;i++)
     for(j=-1;j<=1;j++)
      if(mark[a+i][b+j] == 0)
       dfs(a+i,b+j,c->step(board[a+i][b+j]));
    mark[a][b] = 0;
}

int searchDictionary::printWords(node *c,char *w,int depth)
{
    int i,sum=0;
    if(c==NULL || c->getVisited()==0) return 0;
    if(c->isWord()==1)
    {
        w[depth] = 0;
        fprintf(allWords,"%s\n",w);
        sum++;
    }
    for(i=0;i<26;i++)
    {
        w[depth] = i+'a';
        sum += printWords(c->step(i+'a'),w,depth+1);
    }
    c->clearVisited();
    return sum;
}

void searchDictionary::findAllWords(wordList *result,int pm,int pn,char *pboard)
{
	char w[110];
    int i,j,solutionCount;
   
    allWords = fopen("all.list","wt");
    m = pm;
    n = pn;

	

	board = createBoard();
    mark = createBoard();
    for(i=1;i<=m;i++)
     for(j=1;j<=n;j++)
     {
        board[i][j] = pboard[(i-1)*n+(j-1)];
        mark[i][j] = 0;
     }

	

    for(i=1;i<=m;i++)
     for(j=1;j<=n;j++)
      dfs(i,j,root.step(board[i][j]));
	
	

    root.setVisited();
    solutionCount = printWords(&root,w,0);
    destroyBoard(board);
    destroyBoard(mark);
    fclose(allWords);

	

    result = new wordList(solutionCount); // 

	printf("11111111111111111111111111111111\n");

    allWords = fopen("all.list","rt");
    i=0;

	printf("222222222222222222222222222\n");

    while(fgets(w,100,allWords)!= NULL)
    {
	//printf("strlen: %d\n", strlen(w));

        w[strlen(w)-1] = 0;

	

        result->setWord(w,i);
        i++;
	//printf("%d ", i);
    }
	printf("66666666666666666666666666666666666666666666\n");

    fclose(allWords);
    //return result;
}

void searchDictionary::createTrie()
{
    char w[110];
    rewind(dictionary);
    while(fgets(w,100,dictionary)!= NULL)
    {
        w[strlen(w)-1] = 0;
     	root.expandTrie(w);
    }
}

/*
int main()
{
    searchDictionary words;
    printf("ali: %d\n",words.linearSearch("ali"));
    printf("veli: %d\n",words.linearSearch("veli"));
    printf("legitimate: %d\n",words.linearSearch("legitimate"));
    printf("goes: %d\n",words.linearSearch("goes"));

    return 0;
}*/



