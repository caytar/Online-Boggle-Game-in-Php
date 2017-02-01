// Definition of the ServerConnection class

#ifndef searchDictionary_class
#define searchDictionary_class

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class node{
	public:
		node();
		~node();
		void expandTrie(char *word);
		node *step(char letter);
		void setVisited();
		void clearVisited();
		int getVisited();
		int isWord();
	private:
		int wordEnd,visited;
		node *children[26];
};

class wordList{
	public:
		wordList(int numberOfWords);
		~wordList();
		int binarySearch(char *word);
		void setWord(char *word,int position);
		void printSorted();
	private:
		int listSize;
		char **list;
		int search(char *word,int left,int right);
};

class searchDictionary{
	public:
		searchDictionary();
		~searchDictionary();
		int linearSearch(char *word);
		void findAllWords(wordList *result,int pm,int pn,char *pboard);
	private:
		FILE *dictionary,*allWords;
		node root;
		int m,n;
		char **board,**mark;
		void createTrie();
		char** createBoard();
		void destroyBoard(char **board);
		void dfs(int a,int b,node *c);
		int printWords(node *c,char *w,int depth);
};

#endif
