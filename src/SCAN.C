/****************************************************/
/* File: scan.c                                     */
/* The scanner implementation for the TINY compiler */
/****************************************************/

#include "GLOBALS.H"
#include "UTIL.H"
#include "SCAN.H"

/* states in scanner DFA */
typedef enum
   { START,INASSIGN,INCOMMENT,INNUM,INID,DONE }
   StateType;

/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN+1];

/* BUFLEN = length of the input buffer for
   source code lines */
#define BUFLEN 256

static char lineBuf[BUFLEN]; /* holds the current line */
static int linepos = 0; /* current position in LineBuf */
static int bufsize = 0; /* current size of buffer string */
static int EOF_flag = FALSE; /* corrects ungetNextChar behavior on EOF */

/* getNextChar fetches the next non-blank character
   from lineBuf, reading in a new line if lineBuf is
   exhausted */
static int getNextChar(void)
{ if (!(linepos < bufsize))
  { lineno++;
    if (fgets(lineBuf,BUFLEN-1,source))
    { if (EchoSource) fprintf(listing,"%4d: %s",lineno,lineBuf);
      bufsize = strlen(lineBuf);
      linepos = 0;
      return lineBuf[linepos++];
    }
    else
    { EOF_flag = TRUE;
      return EOF;
    }
  }
  else return lineBuf[linepos++];
}

/* ungetNextChar backtracks one character
   in lineBuf */
static void ungetNextChar(void)
{ if (!EOF_flag) linepos-- ;}

/* lookup table of reserved words */
static struct
    { char* str;
      TokenType tok;
    } reservedWords[MAXRESERVED]
   = {{"if",IF},{"then",THEN},{"else",ELSE},{"end",END},
      {"repeat",REPEAT},{"until",UNTIL},{"read",READ},
      {"write",WRITE}};

/* lookup an identifier to see if it is a reserved word */
/* uses linear search */
static TokenType reservedLookup (char * s)
{ int i;
  for (i=0;i<MAXRESERVED;i++)
    if (!strcmp(s,reservedWords[i].str))
      return reservedWords[i].tok;
  return ID;
}

/****************************************/
/* the primary function of the scanner  */
/****************************************/
/* function getToken returns the 
 * next token in source file
 */
TokenType getToken(void)
{  /* index for storing into tokenString */
   int tokenStringIndex = 0;
   /* holds current token to be returned */
   TokenType currentToken;
   /* current state - always begins at START */
   StateType state = START;
   /* flag to indicate save to tokenString */
   int save;
   while (state != DONE)
   { int c = getNextChar();
     save = TRUE;
     switch (state)
     { case START:
         if (isdigit(c)){
            state = INNUM;
            break;
         }else if(c=='{'){  //?????
            state = INCOMMENT;
            save=FALSE;
            break;
         }else if(c==' ' ||c=='\t' ||c=='\n' ){  //state????
            save = FALSE;
            state = START;
            break;
         }else if(c==':'){   //????
            state = INASSIGN;
            break;
         }else if(c>='a'&& c<='z' || c>='A' && c<='Z'){  //????
            state=INID;
            break;
         }else if(c=='='){
            state = DONE;
            currentToken = EQ;
            break;
         }else if(c=='<'){
            state = DONE;
            currentToken = LT;
            break;
         }else if(c=='+'){
            state = DONE;
            currentToken = PLUS;
            break;
         }else if(c=='-'){
            state = DONE;
            currentToken = MINUS;
            break;
         }else if(c=='*'){
            state = DONE;
            currentToken = TIMES;
            break;
         }else if(c=='/'){
            state = DONE;
            currentToken = OVER;
            break;
         }else if(c=='('){
            state = DONE;
            currentToken = LPAREN;
            break;
         }else if(c==')'){
            state = DONE;
            currentToken = RPAREN;
            break;
         } else if(c==';'){
            state = DONE;
            currentToken = SEMI;
            break;
         }else if(c==EOF){
            state = DONE;
            currentToken = ENDFILE;
            break;
         }else{
            state = DONE;
            currentToken = ERROR;
            break;
         }        /* ???????§Õ???? //??????????§Õ???????:?????/tab/???§³?{?????????????*/
        break;
       case INCOMMENT: //???
        if(c=='}'){  //??????
            state= START;
            save=FALSE;
            //currentToken = INCOMMENT;
            break;
        }else{   //???????state????
            state = INCOMMENT;
            save= FALSE;
            break;
        }      /* ???????§Õ???? ??????????§Õ?????????}????EOF?????¦Ä??????????????????*/
        break;
       case INASSIGN: //???
           if(c=='='){
                state= DONE;
                currentToken=ASSIGN;
                break;
            }else{
                state = DONE;
                currentToken = ERROR;
                break;
               /* ???????§Õ???? ??????????§Õ????=?????????????????*/
            }
        break;
       case INNUM:  //????
         if (!isdigit(c)){ /* backup in the input */
           ungetNextChar();
           save = FALSE;
           state = DONE;
           currentToken = NUM;
           break;
         }else{
            state = INNUM;
            break;
         }
         break;

       case INID:  //???
            if(c>='a'&& c<='z' || c>='A' && c<='Z'){
                state = INID;
            }else{  //??????? ???? ???????
                ungetNextChar();
                save=FALSE;
                state=DONE;
                currentToken=ID;
                break;
            }        /* ???????§Õ???? /??????????§Õ?????????????????????DONE???????????ID*/
        break;

       case DONE:
       default: /* should never happen */
         fprintf(listing,"Scanner Bug: state= %d\n",state);
         state = DONE;
         currentToken = ERROR;
         break;
     }
     if ((save) && (tokenStringIndex <= MAXTOKENLEN))
       tokenString[tokenStringIndex++] = (char) c;
     if (state == DONE)
     { tokenString[tokenStringIndex] = '\0';
       if (currentToken == ID)
         currentToken = reservedLookup(tokenString);
     }
   }
   if (TraceScan) {
     fprintf(listing,"\t%d: ",lineno);
     printToken(currentToken,tokenString);
   }
   return currentToken;
} /* end getToken */

