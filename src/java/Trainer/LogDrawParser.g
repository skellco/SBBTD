//
// LogDraw Parser Grammar
//
// Greg Kuhlmann
//

// Import the necessary classes
{
  import java.io.*;
  import java.util.*;
  import java.awt.Color;
}

class LogDrawParser extends Parser;
options {
  k = 3;
  exportVocab=LogDraw;
  defaultErrorHandler = true;
  buildAST = false;
}

{
  public LogDrawInfo info;
  public int cycle;
  Color color;
  boolean filled;
  int depth;
  String ident;
}

file
  : ( info )* EOF!
  ;

info
  : c:INT { cycle = Integer.parseInt( c.getText() ); }
    COLON s:STRING
    { ident = Utils.unescape( s.getText() ); }
    ( shape )+
  ;

shape
  : ( circle | line | rectangle | text )
  ;

circle
  : "CIRC" ( f:"FILL" )?
    { filled = ( f != null ); }
    d:INT { depth = Integer.parseInt( d.getText() ); }
    rgbColor ( circleCoord )+
  ;

rgbColor
  : LBRACKET r:REAL COMMA g:REAL COMMA b:REAL RBRACKET
  { color = new Color( Float.parseFloat( r.getText() ),
	 	       Float.parseFloat( g.getText() ),
  		       Float.parseFloat( b.getText() ) ); }
  ;

circleCoord
  : LPAREN x:REAL COMMA y:REAL RPAREN r:REAL SEMI
  { info.add( cycle, ident, new FieldCircle( color, filled,
                                      new VecPosition( 
				      Double.parseDouble( x.getText() ), 
				      Double.parseDouble( y.getText() ) ), 
			 	      Double.parseDouble( r.getText() ),
                      depth ) ); }
  ;

line
  : "LINE" 
    d:INT { depth = Integer.parseInt( d.getText() ); }
    rgbColor ( lineCoord )+
  ;

lineCoord
  : LPAREN x:REAL COMMA y:REAL RPAREN 
    LPAREN xx:REAL COMMA yy:REAL RPAREN SEMI
  { info.add( cycle, ident, new FieldLine( color,
			            new VecPosition( 
			            Double.parseDouble( x.getText() ), 
				    Double.parseDouble( y.getText() ) ),
				    new VecPosition( 
			 	    Double.parseDouble( xx.getText() ), 
				    Double.parseDouble( yy.getText() ) ),
                    depth ) ); }
  ;

rectangle
  : "RECT" ( f:"FILL" )?
    { filled = ( f != null ); }
    d:INT { depth = Integer.parseInt( d.getText() ); }
    rgbColor ( rectCoord )+
  ;

rectCoord
  : LPAREN x:REAL COMMA y:REAL RPAREN 
    LPAREN xx:REAL COMMA yy:REAL RPAREN SEMI
  { info.add( cycle, ident, new FieldRect( color, filled,
				      new VecPosition( 
				      Double.parseDouble( x.getText() ), 
				      Double.parseDouble( y.getText() ) ),
				      new VecPosition( 
			 	      Double.parseDouble( xx.getText() ), 
				      Double.parseDouble( yy.getText() ) ),
                      depth ) ); }
  ;

text
  : "TEXT" 
    d:INT { depth = Integer.parseInt( d.getText() ); }
    rgbColor ( textCoord )+
  ;

textCoord
  : LPAREN x:REAL COMMA y:REAL RPAREN s:STRING SEMI
  { info.add( cycle, ident, new FieldText( color,
				      new VecPosition( 
				      Double.parseDouble( x.getText() ), 
				      Double.parseDouble( y.getText() ) ),
                      Utils.unescape( s.getText() ),
                      depth ) ); }
  ;

class LogDrawLexer extends Lexer;

options {
  charVocabulary = '\0'..'\377';
  k = 2;
  testLiterals = false;
  caseSensitive = true;
  caseSensitiveLiterals = true;
  defaultErrorHandler = true;
}

//tokens { "CIRC"; "RECT"; "LINE"; "TEXT"; "FILL"; }

COLON			: ':' ;
SEMI			: ';' ;
LPAREN			: '(' ;
RPAREN			: ')' ;
LBRACKET		: '[' ;
RBRACKET		: ']' ;
COMMA			: ',' ;
protected MINUS		: '-' ;

// Whitespace -- ignored
WS
  : ( ' '
    | '\t'
    | '\f'
    | ( "\r\n"
      | '\r'  
      | '\n'  
      )
      { newline(); }
    )
    { $setType(Token.SKIP); }
  ;

protected
DIGIT : '0'..'9' ;

protected
INT : ( DIGIT )+ ;

protected
REAL : ( MINUS )? INT '.' INT ;

NUMBER : ( ( MINUS )? INT '.' ) => REAL { $setType(REAL); }
| INT { $setType(INT); }
;

protected ESC 
  : '\\' ( '\\' 
         | '"'
         | '\n' 
         )
  ;

STRING : '"' ( ESC | ~'"' )* '"' ;

protected
CHAR : ( 'A'..'Z' ) ;

KEYWORD
  options { testLiterals = true; }
  : ( CHAR )+ ;
