import java.util.*;

public class Rectangle implements Region
{
    VecPosition topLeft, bottomRight;

    public Rectangle()
    {
	topLeft = new VecPosition();
	bottomRight = new VecPosition();
    }

    public Rectangle( double width, double length )
    {
	this( width, length, new VecPosition( 0, 0 ) );
    }

    public Rectangle( double width, double length, VecPosition center )
    {
	this( center.subtract( new VecPosition( length / 2, width / 2 ) ),
	      center.add(      new VecPosition( length / 2, width / 2 ) ) );
    }

    public Rectangle( VecPosition p1, VecPosition p2 )
    {
	setPoints( p1, p2 );
    }

    public void setPoints( VecPosition p1, VecPosition p2 )
    {
	double xTop =    ( p1.isInFrontOf( p2 ) ) ? p1.getX() : p2.getX();
	double xBottom = ( p1.isBehind   ( p2 ) ) ? p1.getX() : p2.getX();
	double yLeft =   ( p1.isLeftOf   ( p2 ) ) ? p1.getY() : p2.getY();
	double yRight =  ( p1.isRightOf  ( p2 ) ) ? p1.getY() : p2.getY();
	topLeft     = new VecPosition( xTop,    yLeft );
	bottomRight = new VecPosition( xBottom, yRight );
    }

    public VecPosition getTopLeft()
    {
	return topLeft;
    }

    public VecPosition getBottomRight()
    {
	return bottomRight;
    }

    public VecPosition getTopRight()
    {
	return new VecPosition( getTopX(), getRightY() );
    }

    public VecPosition getBottomLeft()
    {
	return new VecPosition( getBottomX(), getLeftY() );
    }

    public double getTopX()
    {
	return topLeft.getX();
    }

    public double getBottomX()
    {
	return bottomRight.getX();
    }

    public double getLeftY()
    {
	return topLeft.getY();
    }

    public double getRightY()
    {
	return bottomRight.getY();
    }

    public LineSegment getTopSide()
    {
	return new LineSegment( getTopLeft(), getTopRight() );
    }

    public LineSegment getBottomSide()
    {
	return new LineSegment( getBottomLeft(), getBottomRight() );
    }

    public LineSegment getLeftSide()
    {
	return new LineSegment( getTopLeft(), getBottomLeft() );
    }

    public LineSegment getRightSide()
    {
	return new LineSegment( getBottomRight(), getTopRight() );
    }

    public Iterator cornerIterator()
    {
	Vector v = new Vector();
	v.add( getTopLeft() );
	v.add( getTopRight() );
	v.add( getBottomRight() );
	v.add( getBottomLeft() );
	return v.iterator();
    }

    public Iterator sideIterator()
    {
	Vector v = new Vector();
	v.add( getTopSide() );
	v.add( getRightSide() );
	v.add( getBottomSide() );
	v.add( getLeftSide() );
	return v.iterator();
    }

    public boolean isInside( VecPosition p )
    {
	return p.isBetweenX( bottomRight, topLeft ) &&
	    p.isBetweenY( topLeft, bottomRight );
    }

    public VecPosition getCenter()
    {
	return bottomRight.add( topLeft ).divide( 2 );
    }
 
    public double getWidth()
    {
	return bottomRight.subtract( topLeft ).getY();
    }
 
    public double getLength()
    {
	return topLeft.subtract( bottomRight ).getX();
    }

    public Rectangle add( double d )
    {
	return add( new VecPosition( d, d ) );
    }

    public Rectangle add( VecPosition v )
    {
	return new Rectangle( getWidth(), getLength(), getCenter().add( v ) );
    }

    public Rectangle subtract( double d )
    {
	return subtract( new VecPosition( d, d ) );
    }

    public Rectangle subtract( VecPosition v )
    {
	return new Rectangle( getWidth(), getLength(), getCenter().subtract( v ) );
    }

    public String toString()
    {
	return "[" + topLeft + ", " + bottomRight + "]";
    }
}
