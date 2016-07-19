import java.lang.reflect.Array;
import java.lang.reflect.Field;
import java.io.*;

abstract class Struct
{
    // Buffer and counter to handle
    // packed shorts
    private static int remaining = 0;
    private static byte[] buffer = new byte[ 4 ];

    private Field[] fields;

    public static int s2i( short s )
    {
	return (int) s;
    }

    public static int l2i( long l )
    {
	return (int) Math.round( l2d( l ) );
    }

    public static double l2d( long l )
    {
	return l / 65536.0;
    }

    public static byte readByte( InputStream inStream )
    {
	try {
	    inStream.read( buffer, 0, 1 );
	    byte b = buffer[ 0 ];
	    //if ( Character.isLetterOrDigit( (char) b ) ) 
	    //System.out.print( (char) b );
	    return b;
	}
	catch( IOException e ) {
	    System.err.println( e );
	    return -1;
	}
    }

    public static short readShort( InputStream inStream,
				   boolean even )
    {
	try {
	    short s;
	    if ( even ) {
		inStream.read( buffer, 0, 4 );
		s = (short) ( (int) buffer[ 0 ] << 8 & 0x0000FF00 |
			      (int) buffer[ 1 ] & 0x000000FF );
		remaining = 2;
		//System.out.println( buffer[ 1 ] + ", " +
		//		    buffer[ 0 ] );
	    }
	    else if ( remaining == 2 ) {
		s = (short) ( (int) buffer[ 2 ] << 8 & 0x0000FF00 |
			      (int) buffer[ 3 ] & 0x000000FF );
		remaining = 0;
		//System.out.println( buffer[ 3 ] + ", " +
		//		    buffer[ 2 ] );
	    }
	    else {
		inStream.read( buffer, 0, 2 );
		s = (short) ( (int) buffer[ 0 ] << 8 & 0x0000FF00 |
			      (int) buffer[ 1 ] & 0x000000FF );
		//System.out.println( buffer[ 1 ] + ", " +
		//		    buffer[ 0 ] );
	    }
	    //System.out.println( even + ", " + remaining );
	    //System.out.println( s );
	    //System.out.println( s2i( s ) );
	    return s;
	}
	catch( IOException e ) {
	    System.err.println( e );
	    return -1;
	}
    }

    public static long readLong( InputStream inStream )
    {
	try {
	    byte[] buffer = new byte[ 4 ];
	    inStream.read( buffer, 0, 4 );
	    long l = (long) ( (int) buffer[ 0 ] << 24 & 0xFF000000 |
			      (int) buffer[ 1 ] << 16 & 0x00FF0000 |
			      (int) buffer[ 2 ] << 8 & 0x0000FF00 |
			      (int) buffer[ 3 ] & 0x000000FF );
	    //System.out.println( buffer[ 3 ] + ", " +
	    //			buffer[ 2 ] + ", " +
	    //			buffer[ 1 ] + ", " +
	    //			buffer[ 0 ] );
	    //System.out.println( l );
	    //System.out.println( l2d(l) );
	    return l;
	}
	catch( IOException e ) {
	    System.err.println( e );
	    return -1;
	}
    }

    public static void readFields( Struct object, 
				   InputStream inStream )
    {
	boolean even = true;
	try {
// 	    byte[] buff = new byte[800];
// 	    inStream.read( buff, 0, 800 ); 
// 	    for ( int i = 0; i < 200; i++ ) {
// 		for ( int j = 3; j >=0; j-- ) {
// 		    System.out.print( buff[ 4*i+j ] + ", " );
// 		}
// 		System.out.println();
// 	    }
	    
	    Field[] fields = object.getFields();
	    for ( int i = 0; i < fields.length; i++ ) {
		Class type = fields[ i ].getType();		

		//System.out.println( fields[ i ] );
		if ( type.equals( byte.class ) ) {
		    byte b = readByte( inStream );
		    fields[ i ].setByte( object, b );
		}
		else if ( type.equals( short.class ) ) {
		    short s = readShort( inStream, even );
		    fields[ i ].setShort( object, s );
		    even = !even;
		}
		else if ( type.equals( long.class ) ) {
		    long l = readLong( inStream );
		    fields[ i ].setLong( object, l );
		    even = true;
		}
		else if ( type.isArray() ) {
		    Class componentType = type.getComponentType();
		    Object array = fields[ i ].get( object );
		    int length = Array.getLength( array );
		    for ( int j = 0; j < length; j++ ) {
			if ( componentType.equals( byte.class ) ) {
			    byte b = readByte( inStream );
			    Array.setByte( array, j, b );			    
			    even = false;
			}
			else if ( componentType.equals( short.class ) ) {
			    short s = readShort( inStream, even );
			    Array.setShort( array, j, s );
			    even = !even;
			}
			else if ( componentType.equals( long.class ) ) {
			    long l = readLong( inStream );
			    Array.setLong( array, j, l );
			    even = true;
			}
			else if ( !componentType.isPrimitive() ) {
			    readFields( (Struct) Array.get( array, j ), inStream );
			}
		    }
		}
		else if ( !type.isPrimitive() ) {
		    readFields( (Struct) fields[ i ].get( object ), inStream );
		}
		else {
		    System.err.println( "Struct: Unsupported field type: " +
					type );
		}
	    }
	    if ( !even ) {
	    	readShort( inStream, false );
	    }
	}
	catch ( IllegalAccessException e ) {
	    System.err.println( e );
	}
    }

    public void read( InputStream inStream )
    {
	readFields( this, inStream );
    }

    public Field[] getFields()
    {
	if ( fields == null )
	    fields = getClass().getFields();
	return fields;
    }

    public String toString()
    {
	String s = new String();

	try {
	    Field[] f = getFields();
	    for ( int i = 0; i < f.length; i++ ) {
		s += f[ i ] + " = " + f[ i ].get( this ) + "\n";
	    }
	}
	catch ( Exception e ) {
	    e.printStackTrace();
	}
	return s;
    }
}
