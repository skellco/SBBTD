public class LineSegment
{
    VecPosition p1, p2;

    public LineSegment()
    {
	p1 = new VecPosition();
	p2 = new VecPosition();
    }

    public LineSegment( VecPosition p1, VecPosition p2 )
    {
	setPoints( p1, p2 );
    }

    public void setPoints( VecPosition p1, VecPosition p2 )
    {
	this.p1 = p1;
	this.p2 = p2;
    }

    public double getLength()
    {
	return p1.getDistanceTo( p2 );
    }
    
    public VecPosition getMidpoint()
    {
	return p1.add( p2 ).divide( 2 );
    }

    public Rectangle getBoundingRectangle()
    {
	return new Rectangle( p1, p2 );
    }

    public Line getLine()
    {
	return Line.makeLineFromTwoPoints( p1, p2 );
    }

    public VecPosition getIntersection( Line l )
    {
	Line line = getLine();
	if ( line == null )
	    return null;
	VecPosition intersect = line.getIntersection( l );
	if ( intersect == null ||
	     !getBoundingRectangle().isInside( intersect ) )
	    return null;
	return intersect;
    }
    
    public VecPosition getIntersection( LineSegment ls )
    {
	Line line = ls.getLine();
	if ( line == null )
	    return null;
	VecPosition intersect = getIntersection( line );
	if ( intersect == null ||
	     !ls.getBoundingRectangle().isInside( intersect ) )
	    return null;
	return intersect;
    }
}
