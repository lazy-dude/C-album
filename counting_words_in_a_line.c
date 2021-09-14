// Count number of words in a line.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

#define LINE_LEN 100

int count(const char * const s);
bool acceptable_char(int ch);
bool ignore_ch(int ch);

int main(void)
{
    char words[LINE_LEN+1]= {'\0'};
    int words_count=0;

    do
    {
        printf("Enter a line(just enter to exit):");

        char * wo = NULL;
        wo = fgets(words,LINE_LEN-1,stdin);
        if(wo == NULL)
        {
            fprintf(stderr,"fgets failed.\n");
            exit (EXIT_FAILURE);
        }

        //printf("words:%s\n",words);
        words_count=count(words);
        printf("Words count is: %d\n\n",words_count);

    }
    while(words[0]!='\n');

    printf("Finished.\n");

    return EXIT_SUCCESS;
}

int count(const char * const s)
{
    unsigned idx=1;
    int cnt;

    if(ignore_ch(s[0]) )
        cnt=0;

    else
    {
        for(cnt=0; s[idx]!='\0'; idx++)
        {
            if(isspace(s[idx]) && acceptable_char(s[idx-1]))
                cnt++;
        }
    }

    return cnt;
}

bool acceptable_char(int ch)
{
    bool r;

    if(isalpha(ch) || isdigit(ch))// isalnum
        r=true;
    else
        r=false;

    return r;
}

bool ignore_ch(int ch)
{
    bool r;

    if(ch=='\0' || ch=='\n' || ch==EOF)
        r=true;
    else
        r=false;

    return r;
}
