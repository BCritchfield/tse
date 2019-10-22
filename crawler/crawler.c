/* crawler.c --- crawls a website
1;5202;0c1;5202;0c1;5202;0c * 
 * 
 * Author: Claire C. Collins
 * Created: Fri Oct 18 13:52:36 2019 (-0400)
 * Edited by: Vlado Vojdanovski, Sat Oct 19
 * Version: 
 * 
 * Description: 
 * 
 */
#define _POSIX_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <webpage.h>
#include <queue.h> 
#include <hash.h>
#include <string.h>

int32_t pagesave(webpage_t *pagep, long int id, char *dirname){
	
	free(webpage_getHTML(pagep));
	webpage_fetch(pagep); // self-evident
	char* url = webpage_getURL(pagep); // self-evident
	char* htmlCode = webpage_getHTML(pagep); // self-evident
	
	int sizeh = 0;
	sizeh = webpage_getHTMLlen(pagep); // we grab the charsize of the html of our page
	int size;
  size = sizeh+300; // we add an extra 100 to store the other data we will be using
	FILE *f; // create a file object pointer
	char webpageData[size]; // the data we will be storing in our webpage file 

	memset(webpageData, 0, size*sizeof(char));
	//	
	strcat(url,"\n"); //concat a newline to our url 
	
	// append everything to our character array
	sprintf(webpageData,"%s%d\n%d\n%s", url, webpage_getDepth(pagep), sizeh, htmlCode);
	
	// save address for the file
	char relSavePath[300] = {0};
	//EX: ../pages/1
	//	printf("%s", relSavePath);
	//	fflush(stdout);
	sprintf(relSavePath,"%s%s%s%ld","../",dirname,"/",id);
	
	//if the file doesn't exist or the file is readable
	if((access(relSavePath, F_OK)!=0) || (access(relSavePath, W_OK)==0)){
		f=fopen(relSavePath,"w");  //opens/creates our file
		if ( f != NULL){ //if we sucessfully open a file
			fprintf(f,"%s",webpageData); // store our data
			printf("%s", relSavePath);
			fflush(stdout);
			fclose(f); //and close the file
		}
		else
			{
				return 1; //function exit failure
			}
	}
	else {
		return 1;
	}
	return 0;	// function exit success
}

// A simple helper function that checks whether our hash has a given URL
bool hashContainsURL(void* currPage, const void* searchAddress){
	char* curr =(char*)currPage;
 	if (strcmp((char *)searchAddress,curr) == 0){ // if the strings are the same
			return true;
	}
	return false;
}

int main(int argc, char* argv[]){
	char* seed = argv[1];   //url to be pointed at
	
	webpage_t *w=webpage_new(seed, 0, NULL);    //creates webpage from url
	queue_t* qOfWebPages = qopen(); // opens a queue for internal pages
	hashtable_t*  hashOfPages = hopen(1000);
	
	if(!webpage_fetch(w)){    //exits if unable to fetch webpage
		exit(EXIT_FAILURE);
	}

	char *content=webpage_getHTML(w); //gets html of website
	//	printf("%s", content);       //prints content of website
	if (IsInternalURL(seed)){    //scans if website is internal
		hput(hashOfPages,seed,seed,strlen(seed)); // add the charr array to our hash table
		qput(qOfWebPages, w); //put the initial page in the queue
	}
	else {
		printf("external");
	}

	void* hashSearch = hsearch(hashOfPages,hashContainsURL,"https://thayer.github.io/engs50/Resources/",strlen("https://thayer.github.io/engs50/Resources/")); //read in the result of the search
	//printf("%s \n", currPageURL);
	//fflush(stdout);
	printf("hash:WTF  %s \n", hashSearch);
	fflush(stdout);
	

	char *strPtr;
	char id[100] = {0};
	strcpy(id, argv[3]);
	long int maxIter = strtol(id, &strPtr, 10);
	long int curr_id = 1;
	char ph[50] = {0};
	strcpy(ph,argv[2]);
	int depth = 0;
	while(w = qget(qOfWebPages))
		{
			webpage_fetch(w);
			// initiating search of all internal hyperlinks
			int pos = 0;
			char *currPageURL; // charr array that stores the URl we are looking at
			// while there are URLs to be read
			while((pos = webpage_getNextURL(w, pos, &currPageURL)) > 0 ) {
				if (IsInternalURL(currPageURL)){    //if the url is internal

					webpage_t* currPage=webpage_new(currPageURL,(webpage_getDepth(w)+1), NULL); // create our web page
					//webpage_fetch(currPage);
					//char currURL[100] = {0}; //store the URL in a char array
					//strcpy(currURL, currPageURL);
					//NormalizeURL(webpage_getURL(currPage));
					void* hashSearch = hsearch(hashOfPages,hashContainsURL,webpage_getURL(currPage),strlen(webpage_getURL(currPage))); //read in the result of the search
					//printf("%s \n", currPageURL);
					//fflush(stdout);
					//printf("hash: %s \n", hashSearch);
					//fflush(stdout);
					if ((!hashSearch) && (webpage_getDepth(w) < maxIter)){ // if we got a NULL pointer
						hput(hashOfPages,webpage_getURL(currPage),webpage_getURL(currPage),strlen(webpage_getURL(currPage))); // add the charr array to our hash table
						printf("we are putting in %s \n", webpage_getURL(currPage));
						qput(qOfWebPages, currPage); //put the site in the queue
					}
					else{
						free(currPage);
					}
				}
				free(currPageURL); //always free the currPageUrl memory				
			}

			
			//char ph[] = "pages";
			pagesave(w,curr_id,ph); // save the page of interest
			webpage_delete(w);         //deletes webpage
			curr_id += 1;
		}
			hclose(hashOfPages); // closes hash table
			qclose(qOfWebPages); // closes queue
			
			exit(EXIT_SUCCESS);
		
}
