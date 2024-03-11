# MiniLang Parser
This is the assignment for the my compiler construction course in which we have to create a parser for a custom language which goes by the name MiniLang.

Steps to run:
- bash script.sh
- ./assign "print(x);"

Make sure you provide the source code as a command line argument string for the program to work.

### Grammar Rules: 
- program → <statement> | END_OF_FILE
- statement → <assignment> | <conditional> | <printStatement>
- assignment → = <expression> ; 
- conditional → if ( <expression> ) { <program> } | if ( <expression> ) { <program> } else { <program> }
- printStatement> → PRINT <expression> ;
- expression → <term> + <term> | <term> - <term> | <term>
- term → <factor> * <factor> | <factor> / <factor> | <factor>
- factor → INTEGER | IDENTIFIER | ( <expression> )
