// $ANTLR 2.7.2: "LogDrawParser.g" -> "LogDrawParser.java"$

import antlr.TokenBuffer;
import antlr.TokenStreamException;
import antlr.TokenStreamIOException;
import antlr.ANTLRException;
import antlr.LLkParser;
import antlr.Token;
import antlr.TokenStream;
import antlr.RecognitionException;
import antlr.NoViableAltException;
import antlr.MismatchedTokenException;
import antlr.SemanticException;
import antlr.ParserSharedInputState;
import antlr.collections.impl.BitSet;

  import java.io.*;
  import java.util.*;
  import java.awt.Color;

public class LogDrawParser extends antlr.LLkParser       implements LogDrawTokenTypes
 {

  public LogDrawInfo info;
  public int cycle;
  Color color;
  boolean filled;
  int depth;
  String ident;

protected LogDrawParser(TokenBuffer tokenBuf, int k) {
  super(tokenBuf,k);
  tokenNames = _tokenNames;
}

public LogDrawParser(TokenBuffer tokenBuf) {
  this(tokenBuf,3);
}

protected LogDrawParser(TokenStream lexer, int k) {
  super(lexer,k);
  tokenNames = _tokenNames;
}

public LogDrawParser(TokenStream lexer) {
  this(lexer,3);
}

public LogDrawParser(ParserSharedInputState state) {
  super(state,3);
  tokenNames = _tokenNames;
}

	public final void file() throws RecognitionException, TokenStreamException {
		
		
		try {      // for error handling
			{
			_loop3:
			do {
				if ((LA(1)==INT)) {
					info();
				}
				else {
					break _loop3;
				}
				
			} while (true);
			}
			match(Token.EOF_TYPE);
		}
		catch (RecognitionException ex) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_0);
		}
	}
	
	public final void info() throws RecognitionException, TokenStreamException {
		
		Token  c = null;
		Token  s = null;
		
		try {      // for error handling
			c = LT(1);
			match(INT);
			cycle = Integer.parseInt( c.getText() );
			match(COLON);
			s = LT(1);
			match(STRING);
			ident = Utils.unescape( s.getText() );
			{
			int _cnt6=0;
			_loop6:
			do {
				if ((_tokenSet_1.member(LA(1)))) {
					shape();
				}
				else {
					if ( _cnt6>=1 ) { break _loop6; } else {throw new NoViableAltException(LT(1), getFilename());}
				}
				
				_cnt6++;
			} while (true);
			}
		}
		catch (RecognitionException ex) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_2);
		}
	}
	
	public final void shape() throws RecognitionException, TokenStreamException {
		
		
		try {      // for error handling
			{
			switch ( LA(1)) {
			case LITERAL_CIRC:
			{
				circle();
				break;
			}
			case LITERAL_LINE:
			{
				line();
				break;
			}
			case LITERAL_RECT:
			{
				rectangle();
				break;
			}
			case LITERAL_TEXT:
			{
				text();
				break;
			}
			default:
			{
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
			}
		}
		catch (RecognitionException ex) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_3);
		}
	}
	
	public final void circle() throws RecognitionException, TokenStreamException {
		
		Token  f = null;
		Token  d = null;
		
		try {      // for error handling
			match(LITERAL_CIRC);
			{
			switch ( LA(1)) {
			case LITERAL_FILL:
			{
				f = LT(1);
				match(LITERAL_FILL);
				break;
			}
			case INT:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
			}
			filled = ( f != null );
			d = LT(1);
			match(INT);
			depth = Integer.parseInt( d.getText() );
			rgbColor();
			{
			int _cnt12=0;
			_loop12:
			do {
				if ((LA(1)==LPAREN)) {
					circleCoord();
				}
				else {
					if ( _cnt12>=1 ) { break _loop12; } else {throw new NoViableAltException(LT(1), getFilename());}
				}
				
				_cnt12++;
			} while (true);
			}
		}
		catch (RecognitionException ex) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_3);
		}
	}
	
	public final void line() throws RecognitionException, TokenStreamException {
		
		Token  d = null;
		
		try {      // for error handling
			match(LITERAL_LINE);
			d = LT(1);
			match(INT);
			depth = Integer.parseInt( d.getText() );
			rgbColor();
			{
			int _cnt17=0;
			_loop17:
			do {
				if ((LA(1)==LPAREN)) {
					lineCoord();
				}
				else {
					if ( _cnt17>=1 ) { break _loop17; } else {throw new NoViableAltException(LT(1), getFilename());}
				}
				
				_cnt17++;
			} while (true);
			}
		}
		catch (RecognitionException ex) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_3);
		}
	}
	
	public final void rectangle() throws RecognitionException, TokenStreamException {
		
		Token  f = null;
		Token  d = null;
		
		try {      // for error handling
			match(LITERAL_RECT);
			{
			switch ( LA(1)) {
			case LITERAL_FILL:
			{
				f = LT(1);
				match(LITERAL_FILL);
				break;
			}
			case INT:
			{
				break;
			}
			default:
			{
				throw new NoViableAltException(LT(1), getFilename());
			}
			}
			}
			filled = ( f != null );
			d = LT(1);
			match(INT);
			depth = Integer.parseInt( d.getText() );
			rgbColor();
			{
			int _cnt22=0;
			_loop22:
			do {
				if ((LA(1)==LPAREN)) {
					rectCoord();
				}
				else {
					if ( _cnt22>=1 ) { break _loop22; } else {throw new NoViableAltException(LT(1), getFilename());}
				}
				
				_cnt22++;
			} while (true);
			}
		}
		catch (RecognitionException ex) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_3);
		}
	}
	
	public final void text() throws RecognitionException, TokenStreamException {
		
		Token  d = null;
		
		try {      // for error handling
			match(LITERAL_TEXT);
			d = LT(1);
			match(INT);
			depth = Integer.parseInt( d.getText() );
			rgbColor();
			{
			int _cnt26=0;
			_loop26:
			do {
				if ((LA(1)==LPAREN)) {
					textCoord();
				}
				else {
					if ( _cnt26>=1 ) { break _loop26; } else {throw new NoViableAltException(LT(1), getFilename());}
				}
				
				_cnt26++;
			} while (true);
			}
		}
		catch (RecognitionException ex) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_3);
		}
	}
	
	public final void rgbColor() throws RecognitionException, TokenStreamException {
		
		Token  r = null;
		Token  g = null;
		Token  b = null;
		
		try {      // for error handling
			match(LBRACKET);
			r = LT(1);
			match(REAL);
			match(COMMA);
			g = LT(1);
			match(REAL);
			match(COMMA);
			b = LT(1);
			match(REAL);
			match(RBRACKET);
			color = new Color( Float.parseFloat( r.getText() ),
				 	       Float.parseFloat( g.getText() ),
					       Float.parseFloat( b.getText() ) );
		}
		catch (RecognitionException ex) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_4);
		}
	}
	
	public final void circleCoord() throws RecognitionException, TokenStreamException {
		
		Token  x = null;
		Token  y = null;
		Token  r = null;
		
		try {      // for error handling
			match(LPAREN);
			x = LT(1);
			match(REAL);
			match(COMMA);
			y = LT(1);
			match(REAL);
			match(RPAREN);
			r = LT(1);
			match(REAL);
			match(SEMI);
			info.add( cycle, ident, new FieldCircle( color, filled,
			new VecPosition( 
							      Double.parseDouble( x.getText() ), 
							      Double.parseDouble( y.getText() ) ), 
						 	      Double.parseDouble( r.getText() ),
			depth ) );
		}
		catch (RecognitionException ex) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_5);
		}
	}
	
	public final void lineCoord() throws RecognitionException, TokenStreamException {
		
		Token  x = null;
		Token  y = null;
		Token  xx = null;
		Token  yy = null;
		
		try {      // for error handling
			match(LPAREN);
			x = LT(1);
			match(REAL);
			match(COMMA);
			y = LT(1);
			match(REAL);
			match(RPAREN);
			match(LPAREN);
			xx = LT(1);
			match(REAL);
			match(COMMA);
			yy = LT(1);
			match(REAL);
			match(RPAREN);
			match(SEMI);
			info.add( cycle, ident, new FieldLine( color,
						            new VecPosition( 
						            Double.parseDouble( x.getText() ), 
							    Double.parseDouble( y.getText() ) ),
							    new VecPosition( 
						 	    Double.parseDouble( xx.getText() ), 
							    Double.parseDouble( yy.getText() ) ),
			depth ) );
		}
		catch (RecognitionException ex) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_5);
		}
	}
	
	public final void rectCoord() throws RecognitionException, TokenStreamException {
		
		Token  x = null;
		Token  y = null;
		Token  xx = null;
		Token  yy = null;
		
		try {      // for error handling
			match(LPAREN);
			x = LT(1);
			match(REAL);
			match(COMMA);
			y = LT(1);
			match(REAL);
			match(RPAREN);
			match(LPAREN);
			xx = LT(1);
			match(REAL);
			match(COMMA);
			yy = LT(1);
			match(REAL);
			match(RPAREN);
			match(SEMI);
			info.add( cycle, ident, new FieldRect( color, filled,
							      new VecPosition( 
							      Double.parseDouble( x.getText() ), 
							      Double.parseDouble( y.getText() ) ),
							      new VecPosition( 
						 	      Double.parseDouble( xx.getText() ), 
							      Double.parseDouble( yy.getText() ) ),
			depth ) );
		}
		catch (RecognitionException ex) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_5);
		}
	}
	
	public final void textCoord() throws RecognitionException, TokenStreamException {
		
		Token  x = null;
		Token  y = null;
		Token  s = null;
		
		try {      // for error handling
			match(LPAREN);
			x = LT(1);
			match(REAL);
			match(COMMA);
			y = LT(1);
			match(REAL);
			match(RPAREN);
			s = LT(1);
			match(STRING);
			match(SEMI);
			info.add( cycle, ident, new FieldText( color,
							      new VecPosition( 
							      Double.parseDouble( x.getText() ), 
							      Double.parseDouble( y.getText() ) ),
			Utils.unescape( s.getText() ),
			depth ) );
		}
		catch (RecognitionException ex) {
			reportError(ex);
			consume();
			consumeUntil(_tokenSet_5);
		}
	}
	
	
	public static final String[] _tokenNames = {
		"<0>",
		"EOF",
		"<2>",
		"NULL_TREE_LOOKAHEAD",
		"INT",
		"COLON",
		"STRING",
		"\"CIRC\"",
		"\"FILL\"",
		"LBRACKET",
		"REAL",
		"COMMA",
		"RBRACKET",
		"LPAREN",
		"RPAREN",
		"SEMI",
		"\"LINE\"",
		"\"RECT\"",
		"\"TEXT\"",
		"MINUS",
		"WS",
		"DIGIT",
		"NUMBER",
		"ESC",
		"CHAR",
		"KEYWORD"
	};
	
	private static final long[] mk_tokenSet_0() {
		long[] data = { 2L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_0 = new BitSet(mk_tokenSet_0());
	private static final long[] mk_tokenSet_1() {
		long[] data = { 458880L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_1 = new BitSet(mk_tokenSet_1());
	private static final long[] mk_tokenSet_2() {
		long[] data = { 18L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_2 = new BitSet(mk_tokenSet_2());
	private static final long[] mk_tokenSet_3() {
		long[] data = { 458898L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_3 = new BitSet(mk_tokenSet_3());
	private static final long[] mk_tokenSet_4() {
		long[] data = { 8192L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_4 = new BitSet(mk_tokenSet_4());
	private static final long[] mk_tokenSet_5() {
		long[] data = { 467090L, 0L};
		return data;
	}
	public static final BitSet _tokenSet_5 = new BitSet(mk_tokenSet_5());
	
	}
