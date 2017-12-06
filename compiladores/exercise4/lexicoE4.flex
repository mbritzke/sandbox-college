%%

%byaccj


%{
  private Parser yyparser;

  public Yylex(java.io.Reader r, Parser yyparser) {
    this(r);
    this.yyparser = yyparser;
    yyline = 1;
  }


  public int getLine() {
      return yyline;
  }

%}


NL  = \n | \r | \r\n

%%

"$TRACE_ON"  { yyparser.setDebug(true);  }
"$TRACE_OFF" { yyparser.setDebug(false); }
"$MOSTRA_TS" { yyparser.listarTS(); }


"void"      { return Parser.VOID; }
"int"       { return Parser.INT; }
"double"    { return Parser.DOUBLE; }
"boolean"   { return Parser.BOOLEAN; }
"funct"     { return Parser.FUNCT; }
"if"        { return Parser.IF; }
"else"      { return Parser.ELSE; }
"while"     { return Parser.WHILE; }
"true"      { return Parser.TRUE; }
"false"     { return Parser.FALSE; }
"&&"        { return Parser.AND; }
"||"        { return Parser.OR; }
"=="        { return Parser.EQ; }


[a-zA-Z][a-zA-z0-9_]*   { yyparser.yylval = new ParserVal(yytext());
                          return Parser.IDENT; }

[0-9]+      { yyparser.yylval = new ParserVal(Integer.parseInt(yytext()));
              return Parser.NUM; }


"=" |
"<" |
">" |
"+" |
"-" |
"*" |
"/" |
"," |
";" |
"(" |
")" |
"{" |
"}"     { return (int) yycharat(0); }

[ \t]+ { }
{NL}+  {yyline++;}

.    { System.err.println("Error: unexpected character '"+yytext()+"' na linha "+yyline); return YYEOF; }
