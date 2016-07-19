import java.io.Serializable;

public class VecPosition implements Serializable
{
    private double m_x;
    private double m_y;
 
    public VecPosition()
    {
	this( 0, 0 );
    }

    public VecPosition( double vx, double vy )
    {
	this( vx, vy, false );
    }

    public VecPosition( double vx, double vy, boolean polar )
    {
	if ( polar ) {
	    VecPosition p = getVecPositionFromPolar( vx, vy );
	    m_x = p.getX();
	    m_y = p.getY();
	}
	else { // cartesian
	    m_x = vx;
	    m_y = vy;
	}
    }

    public VecPosition( VecPosition p )
    {
	this( p.getX(), p.getY() );
    }

    public void copy( VecPosition p )
    {
	m_x = p.getX();
	m_y = p.getY();
    }

    public double getX()
    {
	return m_x;
    }

    public double getY()
    {
	return m_y;
    }

    public double getMagnitude()
    {
	return Math.sqrt( m_x * m_x + m_y * m_y );
    }

    public double getDirection()
    {
	return Utils.atan2Deg( m_y, m_x );
    }

    public double getDistanceTo( VecPosition p )
    {
	return subtract( p ).getMagnitude();
    }

    public boolean isInFrontOf( double d )
    {
	return m_x > d;
    }

    public boolean isInFrontOf( VecPosition p )
    {
	return m_x > p.m_x;
    }

    public boolean isBehind( double d )
    {
	return m_x < d;
    }

    public boolean isBehind( VecPosition p )
    {
	return m_x < p.m_x;
    }

    public boolean isLeftOf( double d )
    {
	return m_y < d;
    }

    public boolean isLeftOf( VecPosition p )
    {
	return m_y < p.m_y;
    }  
  
    public boolean isRightOf( double d )
    {
	return m_y > d;
    }

    public boolean isRightOf( VecPosition p )
    {
	return m_y > p.m_y;
    }  

    public boolean isBetweenX( VecPosition p1, VecPosition p2 )
    {
	return isBetweenX( p1.getX(), p2.getX() );
    }

    public boolean isBetweenX( double d1, double d2 )
    {
	return isInFrontOf( d1 ) && isBehind( d2 ) ||
	    m_x == d1 || m_x == d2;
    }

    public boolean isBetweenY( VecPosition p1, VecPosition p2 )
    {
	return isBetweenY( p1.getY(), p2.getY() );
    }
    
    public boolean isBetweenY( double d1, double d2 )
    {
	return isRightOf( d1 ) && isLeftOf( d2 ) ||
	    m_y == d1 || m_y == d2;
    }

    public VecPosition negate()
    {
	return new VecPosition( -m_x, -m_y );
    }

    public VecPosition add( double d )
    {
	return new VecPosition( m_x + d, m_y + d );
    }

    public VecPosition add( VecPosition p )
    {
	return new VecPosition( m_x + p.m_x, m_y + p.m_y );
    }

    public VecPosition subtract( double d )
    {
	return new VecPosition( m_x - d, m_y - d );
    }

    public VecPosition subtract( VecPosition p )
    {
	return new VecPosition( m_x - p.m_x, m_y - p.m_y );
    }

    public VecPosition multiply( double d )
    {
	return new VecPosition( m_x * d, m_y * d );
    }

    public VecPosition multiply( VecPosition p )
    {
	return new VecPosition( m_x * p.m_x, m_y * p.m_y );
    }

    public VecPosition divide( double d )
    {
	return new VecPosition( m_x / d, m_y / d );
    }

    public VecPosition divide( VecPosition p )
    {
	return new VecPosition( m_x / p.m_x, m_y / p.m_y );
    }

    public VecPosition withMagnitude( double d )
    {
	if ( getMagnitude() > Utils.EPSILON )
	    return multiply( d / getMagnitude() );
	return new VecPosition( getX(), getY() );
    }

    public VecPosition normalize()
    {
	return withMagnitude( 1.0 );
    }

    public VecPosition rotate( double angDeg )
    {
	double dMag = getMagnitude();
	double dNewDir = getDirection() + angDeg;
	return new VecPosition( dMag, dNewDir, true );
    }

    public VecPosition globalToRelative( VecPosition origin, double angDeg )
    {
	return subtract( origin ).rotate( -angDeg );
    }
    
    public VecPosition relativeToGlobal( VecPosition origin, double angDeg )
    {
	return rotate( angDeg ).add( origin );
    }

    public double dotProduct( VecPosition p )
    {
	VecPosition v = multiply( p );
	return v.getX() + v.getY();
    }

    public double getAngleBetweenPoints( VecPosition p1, VecPosition p2 )
    {
	VecPosition v1 = subtract( p1 ).normalize();
	VecPosition v2 = subtract( p2 ).normalize();
	return Math.abs( Utils.acosDeg( v1.dotProduct( v2 ) ) );
    }
    
    public String toString()
    {
	return "( " + m_x + ", " + m_y + " )";
    }
    
    public static VecPosition getVecPositionFromPolar( double mag, double angDeg )
    {
	return new VecPosition( mag * Utils.cosDeg( angDeg ), 
				mag * Utils.sinDeg( angDeg ) );
    }
}
