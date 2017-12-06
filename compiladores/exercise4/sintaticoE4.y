%{
  import java.io.*;
%}

%token VOID, INT, DOUBLE, BOOLEAN
%token IF, ELSE, WHILE, TRUE, FALSE, FUNCT
%token IDENT, NUM, AND, OR, EQ

%right '='
%nonassoc '<', '>', EQ
%left AND OR
%left '-' '+'
%left '*' '/'

%type <sval> IDENT
%type <ival> NUM
%type <obj> Tipo
%type <obj> E

%%


Prog : { currClass = ClasseID.VarGlobal; } Decl ListaFuncoes
     ;

Decl : Tipo {currentType = (TS_entry)$1; } 
       LId ';' Decl 
     |
     ;

Tipo : INT { $$ = Tp_INT; }
     | DOUBLE { $$ = Tp_DOUBLE; }
     | BOOLEAN { $$ = Tp_BOOLEAN; }
     ;

LId : LId ',' IDENT 
    | IDENT { TS_entry nodo = ts.pesquisa($1);
                            if (nodo != null) 
                              yyerror("(sem) variavel >" + $1 + "< jah declarada");
                          else ts.insert(new TS_entry($1, currentType, currClass)); 
                       }
    ;

ListaFuncoes : umaFuncao ListaFuncoes
             | umaFuncao
             ;

umaFuncao : FUNCT tipoOuVoid IDENT '(' ')' Bloco 
          | FUNCT tipoOuVoid IDENT '(' { currClass = ClasseID.VarLocal; } ListaParametros ')' Bloco
		  ;

tipoOuVoid : VOID
           | Tipo
           ;

ListaParametros : Tipo IDENT { TS_entry nodo = ts.pesquisa($2);
                            if (nodo != null) 
                              yyerror("(sem) variavel >" + $2 + "< jah declarada");
                          else ts.insert(new TS_entry($2, (TS_entry)$1, currClass)); 
                       }
                | Tipo IDENT { TS_entry nodo = ts.pesquisa($2);
                            if (nodo != null) 
                              yyerror("(sem) variavel >" + $2 + "< jah declarada");
                          else ts.insert(new TS_entry($2, (TS_entry)$1, currClass)); 
                       }
                  ',' ListaParametros
                ;

Bloco : { currClass = ClasseID.VarLocal; } '{' Decl LCmd '}' 

LCmd :  Cmd LCmd
     |
     ;

Cmd : Bloco
    | IF '(' E ')' Cmd
    | IF '(' E ')' Cmd ELSE Cmd
    | WHILE '(' E ')' Cmd
    |  E  ';'
    ;

E : E '+' E { $$ = validaTipo('+', (TS_entry)$1, (TS_entry)$3); }
  | E '-' E { $$ = validaTipo('-', (TS_entry)$1, (TS_entry)$3); }
  | E '*' E { $$ = validaTipo('*', (TS_entry)$1, (TS_entry)$3); }
  | E '/' E { $$ = validaTipo('/', (TS_entry)$1, (TS_entry)$3); }
  | E '>' E { $$ = validaTipo('>', (TS_entry)$1, (TS_entry)$3); }
  | E '<' E { $$ = validaTipo('<', (TS_entry)$1, (TS_entry)$3); }
  | E EQ E  { $$ = validaTipo(EQ, (TS_entry)$1, (TS_entry)$3); }
  | E AND E { $$ = validaTipo(AND, (TS_entry)$1, (TS_entry)$3); }
  | E OR E  { $$ = validaTipo(OR, (TS_entry)$1, (TS_entry)$3); }
  | NUM { $$ = Tp_INT; }
  | TRUE { $$ = Tp_BOOLEAN; }
  | FALSE { $$ = Tp_BOOLEAN; }
  | IDENT { TS_entry nodo = ts.pesquisa($1);
                    if (nodo == null) {
                       yyerror("(sem) var <" + $1 + "> nao declarada");
                       $$ = Tp_ERRO;
                       }
                    else
                        $$ = nodo.getTipo();
                  }
  | IDENT '(' ')' { $$ = $1; }
  | IDENT '(' listaExp ')' { $$ = $1; }
  | '(' E ')' { $$ = $2; }
  | E '=' E { $$ = validaTipo(ATRIB, (TS_entry)$1, (TS_entry)$3); }
  ;

 listaExp : E
          | E ',' listaExp
          ;

%%

  private Yylex lexer;

  private TabSimb ts;

  public static TS_entry Tp_INT =  new TS_entry("int", null, ClasseID.TipoBase);
  public static TS_entry Tp_DOUBLE = new TS_entry("double", null,  ClasseID.TipoBase);
  public static TS_entry Tp_BOOLEAN = new TS_entry("boolean", null,  ClasseID.TipoBase);
  public static TS_entry Tp_ERRO = new TS_entry("_erro_", null,  ClasseID.TipoBase);

  public static final int ARRAY = 1500;
  public static final int ATRIB = 1600;

  private String currEscopo;
  private ClasseID currClass;
  
  private TS_entry currentType;

  private int yylex () {
    int yyl_return = -1;
    try {
      yylval = new ParserVal(0);
      yyl_return = lexer.yylex();
    }
    catch (IOException e) {
      System.err.println("IO error :" + e.getMessage());
    }
    return yyl_return;
  }


  public void yyerror (String error) {
    System.err.printf("Erro (linha: %2d \tMensagem: %s)\n", lexer.getLine(), error);
    // System.err.println ("Error: " + error);
  }


  public Parser(Reader r) {
    lexer = new Yylex(r, this);
    ts = new TabSimb();
    ts.insert(Tp_ERRO);
    ts.insert(Tp_INT);
    ts.insert(Tp_DOUBLE);
    ts.insert(Tp_BOOLEAN);
  }

  public void setDebug(boolean debug) {
    yydebug = debug;
  }

  public void listarTS() { ts.listar();}

  static boolean interactive;

  public static void main(String args[]) throws IOException {
    System.out.println("\n\nVerificador semantico simples\n");


    Parser yyparser;
    if ( args.length > 0 ) {
      // parse a file
      yyparser = new Parser(new FileReader(args[0]));
    }
    else {
      // interactive mode
      System.out.println("[Quit with CTRL-D]");
      System.out.print("Programa de entrada:\n");
        yyparser = new Parser(new InputStreamReader(System.in));
    }

    yyparser.yyparse();

      yyparser.listarTS();

      System.out.print("\n\nFeito!\n");

  }

  TS_entry validaTipo(int operador, TS_entry A, TS_entry B) {

           switch ( operador ) {
                case ATRIB:
                      if ( (A == Tp_INT && B == Tp_INT)                        ||
                           ((A == Tp_DOUBLE && (B == Tp_INT || B == Tp_DOUBLE))) ||
                           (A == B))
                           return A;
                       else
                           yyerror("(sem) tipos incomp. para atribuicao: "+ A.getTipoStr() + " = "+B.getTipoStr());
                      break;

                case '+' :
                      if ( A == Tp_INT && B == Tp_INT)
                            return Tp_INT;
                      else if ( (A == Tp_DOUBLE && (B == Tp_INT || B == Tp_DOUBLE)) ||
                                              (B == Tp_DOUBLE && (A == Tp_INT || A == Tp_DOUBLE)) )
                           return Tp_DOUBLE;
                      else
                          yyerror("(sem) tipos incomp. para soma: "+ A.getTipoStr() + " + "+B.getTipoStr());
                      break;

                case '-' :
                      if ( A == Tp_INT && B == Tp_INT)
                            return Tp_INT;
                      else if ( (A == Tp_DOUBLE && (B == Tp_INT || B == Tp_DOUBLE)) ||
                                              (B == Tp_DOUBLE && (A == Tp_INT || A == Tp_DOUBLE)) )
                           return Tp_DOUBLE;
                      else
                          yyerror("(sem) tipos incomp. para subtração: "+ A.getTipoStr() + " + "+B.getTipoStr());
                      break;

                case '*' :
                      if ( A == Tp_INT && B == Tp_INT)
                            return Tp_INT;
                      else if ( (A == Tp_DOUBLE && (B == Tp_INT || B == Tp_DOUBLE)) ||
                                              (B == Tp_DOUBLE && (A == Tp_INT || A == Tp_DOUBLE)) )
                           return Tp_DOUBLE;
                      else
                          yyerror("(sem) tipos incomp. para multiplicação: "+ A.getTipoStr() + " + "+B.getTipoStr());
                      break;

                case '/' :
                      if ( A == Tp_INT && B == Tp_INT)
                            return Tp_INT;
                      else if ( (A == Tp_DOUBLE && (B == Tp_INT || B == Tp_DOUBLE)) ||
                                              (B == Tp_DOUBLE && (A == Tp_INT || A == Tp_DOUBLE)) )
                           return Tp_DOUBLE;
                      else
                          yyerror("(sem) tipos incomp. para divisão: "+ A.getTipoStr() + " + "+B.getTipoStr());
                      break;

               case '>' :
                       if ((A == Tp_INT || A == Tp_DOUBLE) && (B == Tp_INT || B == Tp_DOUBLE))
                           return Tp_BOOLEAN;
                        else
                          yyerror("(sem) tipos incomp. para op relacional: "+ A.getTipoStr() + " > "+B.getTipoStr());
                        break;

              case '<' :
                       if ((A == Tp_INT || A == Tp_DOUBLE) && (B == Tp_INT || B == Tp_DOUBLE))
                           return Tp_BOOLEAN;
                        else
                          yyerror("(sem) tipos incomp. para op relacional: "+ A.getTipoStr() + " > "+B.getTipoStr());
                        break;

               case AND:
                       if (A == Tp_BOOLEAN && B == Tp_BOOLEAN)
                           return Tp_BOOLEAN;
                        else
                          yyerror("(sem) tipos incomp. para op lógica: "+ A.getTipoStr() + " && "+B.getTipoStr());
                   break;
              case OR:
                       if (A == Tp_BOOLEAN && B == Tp_BOOLEAN)
                           return Tp_BOOLEAN;
                        else
                          yyerror("(sem) tipos incomp. para op lógica: "+ A.getTipoStr() + " || "+B.getTipoStr());
                   break;
              case EQ:
                       if ((A == Tp_BOOLEAN && B == Tp_BOOLEAN) || (A == Tp_INT && B == Tp_DOUBLE) || (A == Tp_INT && B == Tp_DOUBLE)
                            || (A == Tp_INT && B == Tp_INT) || (A == Tp_DOUBLE && B == Tp_DOUBLE))
                           return Tp_BOOLEAN;
                        else
                          yyerror("(sem) tipos incomp. para op igualdade: "+ A.getTipoStr() + " == "+B.getTipoStr());
                   break;
              }

              return Tp_ERRO;

       }
