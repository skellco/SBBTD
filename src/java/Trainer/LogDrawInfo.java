import java.io.*;
import java.util.*;

public class LogDrawInfo
{
    private Map[] frames;
    private Set idents;

    public LogDrawInfo( int numCycles )
    {
	frames = new Map[ numCycles ];
	for ( int i = 0; i < numCycles; i++ ) {
	    frames[ i ] = new HashMap();
	}
	idents = new HashSet();
    }

    public void add( int cycle, String ident, FieldShape fs )
    {
	idents.add( ident );
	Vector info = getInfo( cycle, ident );
	if ( info == null ) {
	    info = new Vector();
	}
	info.add( fs );
	frames[ cycle ].put( ident, info );
    }

    public String[] getIdents()
    {
	return (String[]) idents.toArray( new String[ 0 ] );
    }

    public Vector getInfo( int cycle, String ident )
    {
	return (Vector) frames[ cycle ].get( ident );
    }

    // Testing purposes
    public static void main( String args[] )
    {
	try {
	    BufferedReader br =
		new BufferedReader( new FileReader( args[ 0 ] ) );
	    LogDrawLexer lexer = new LogDrawLexer( br );
	    LogDrawParser parser = new LogDrawParser( lexer );
	    LogDrawInfo info = new LogDrawInfo( 6002 );
            parser.info = info;
	    parser.file();
	    String[] idents = info.getIdents();
	    for ( int i = 0; i <= parser.cycle; i++ ) {
		System.out.println( "CYCLE: " + i );
		for ( int j = 0; j < idents.length; j++ ) {
		    System.out.println( "Ident: " + idents[ j ] );
		    Vector shapes = info.getInfo( i, idents[ j ] );
		    System.out.println( shapes );
		}
	    }
	}
	catch( Exception e ) {
	    e.printStackTrace();
	    System.exit( 1 );
	}
    }
}
