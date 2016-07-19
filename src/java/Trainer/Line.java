import java.util.*;

public class Line
{
    final static double EPSILON = 0.00001;
    double a, b, c;

    public Line( double a, double b, double c )
    {
	this.a = a;
	this.b = b;
	this.c = c;
    }

    public double getA()
    {
	return a;
    }

    public double getB()
    {
	return b;
    }

    public double getC()
    {
	return c;
    }

    public VecPosition getIntersection( LineSegment ls )
    {
	return ls.getIntersection( this );
    }

    public VecPosition getIntersection( Line line )
    {
	double x, y;

	if ( ( a / b ) == ( line.getA() / line.getB() ) ) {
	    return null; 
	}

	if ( a == 0 ) {
	    x = -c / b;
	    y = line.getYGivenX( x );  
	}                   
	else if ( line.getA() == 0 ) {
	    x = -line.getC() / line.getB();
	    y = getYGivenX( x );
	}
	else {
	    x = ( a * line.getC() - line.getA() * c ) /
		( line.getA() * b - a * line.getB() );
	    y = getYGivenX( x );
	}

	return new VecPosition( x, y );	
    }

    public Line getTangentLine( VecPosition pos )
    {
	return new Line( b, -a, a * pos.getX() - b * pos.getY() );
    }

    public VecPosition getPointOnLineClosestTo( VecPosition pos )
    {
	Line l2 = getTangentLine( pos );
	return getIntersection( l2 );
    }

    public double getDistanceToPoint( VecPosition pos )
    {                                                   
	return pos.getDistanceTo( getPointOnLineClosestTo( pos ) );
    }            

    public double getYGivenX( double x )
    {
	if ( a == 0 ) {
	    System.err.println( "getYGivenX(): Cannot calculate Y coordinate" );
	    return 0;
	}

	return -( b * x + c ) / a;
    }

    public double getXGivenY( double y )
    {
	if ( b == 0 ) {
	    System.err.println( "getXGivenY(): Cannot calculate X coordinate" );
	    return 0;
	}

	return -( a * y + c ) / b;
    }

    public static Line makeLineFromTwoPoints( VecPosition pos1, VecPosition pos2 )
    {
	double dA, dB, dC;
	double dTemp = pos2.getX() - pos1.getX();

	if ( Math.abs( dTemp ) < EPSILON ) {                             
	    dA = 0.0;                                             
	    dB = 1.0;
	}
	else {
	    dA = 1.0;
	    dB = -( pos2.getY() - pos1.getY() ) / dTemp;
	}
	dC = -dA * pos2.getY() - dB * pos2.getX();

	return new Line( dA, dB, dC );
    }

    public String toString()
    {
	String retVal;

	if ( a == 0 ) {
	    retVal = "x = " + ( -c / b );
	}
	else {
	    retVal = "y = ";
	    if ( b != 0 )
		retVal += ( -b / a ) + "x ";
	    if ( c > 0 ) {
		retVal += "- " + Math.abs( c / a );
	    }
	    else if ( c < 0 ) {
		retVal += "+ " + Math.abs( c / a );
	    }
	}

	return retVal;
    }

    // Testing purposes
    public static void main( String[] args )
    {
	VecPosition p1 = new VecPosition( -10, -10 );
	VecPosition p2 = new VecPosition(  10,  10 );
	Rectangle rect = new Rectangle( p1, p2 );
	Line line = Line.makeLineFromTwoPoints( p1, p2 );
					
	Iterator i = rect.sideIterator();
	while ( i.hasNext() ) {
	    LineSegment ls = (LineSegment) i.next();
	    Line l = ls.getLine();
	    System.out.println( l.getIntersection( line ) );
	}
    }
}
