import java.util.*;
import java.text.*;

public class Utils implements SoccerTypes
{
    public final static double EPSILON = 1E-4;

    public static double cosDeg( double angDeg )
    {
	return Math.cos( Math.toRadians( angDeg ) );
    }

    public static double sinDeg( double angDeg )
    {
	return Math.sin( Math.toRadians( angDeg ) );
    }

    public static double tanDeg( double angDeg )
    {
	return Math.tan( Math.toRadians( angDeg ) );
    }

    public static double atanDeg( double x )
    {
	return Math.toDegrees( Math.atan( x ) );
    }

    public static double atan2Deg( double y, double x )
    {
	if ( Math.abs( x ) < EPSILON && Math.abs( y ) < EPSILON )
	    return 0;
	return Math.toDegrees( Math.atan2( y, x ) );
    }

    public static double acosDeg( double x )
    {
	if ( x >= 1 )
	    return 0;
	else if ( x <= -1 )
	    return 180;
	return Math.toDegrees( Math.acos( x ) );
    }

    public static double asinDeg( double x )
    {
	if ( x >= 1 )
	    return 90;
	else if ( x <= -1 )
	    return  -90;
	return Math.toDegrees( Math.asin( x ) );
    }

    public static double normalizeAngle( double angDeg )
    {
	while( angDeg > 180  ) 
	    angDeg -= 360;
	while( angDeg < -180 ) 
	    angDeg += 360;
	return angDeg;
    }

    public static int sign( double d )
    {
	return ( d > 0 ) ? 1 : -1;
    }

    public static boolean isAngInInterval( double angDeg, double angMin, double angMax )
    {
	if ( angDeg < 0 ) angDeg += 360;
	if ( angMin < 0 ) angMin += 360;
	if ( angMax < 0 ) angMax += 360;

	if ( angMin < angMax )
	    return angMin < angDeg && angDeg < angMax ;
	else                 
	    return !( angMax < angDeg && angDeg < angMin );
    }

    public static double getBisectorTwoAngles( double angMin, double angMax )
    {
	// separate sine and cosine part to circumvent boundary problem
	return normalizeAngle( atan2Deg( ( sinDeg( angMin ) + sinDeg( angMax ) ) / 2.0,
					 ( cosDeg( angMin ) + cosDeg( angMax ) ) / 2.0 ) );
    }

    public static boolean isLeftPlayer( int id )
    {
	return id >= ID_PLAYER_L_1 && id <= ID_PLAYER_L_11;
    }

    public static boolean isRightPlayer( int id )
    {
	return id >= ID_PLAYER_R_1 && id <= ID_PLAYER_R_11;
    }

    public static boolean isPlayer( int id )
    {
	return isLeftPlayer( id ) || isRightPlayer( id );
    }

    public static boolean isLeftGoal( int id )
    {
	return id == ID_GOAL_L;
    }

    public static boolean isRightGoal( int id )
    {
	return id == ID_GOAL_R;
    }

    public static boolean isGoal( int id )
    {
	return isLeftGoal( id ) || isRightGoal( id );
    }

    public static boolean isBall( int id )
    {
	return id == ID_BALL;
    }

    public static int getUnumFromID( int id )
    {
	if ( isLeftPlayer( id ) )
	    return id - ID_PLAYER_L_1 + 1;
	if ( isRightPlayer( id ) )
	    return id - ID_PLAYER_R_1 + 1;
	return 0;
    }

    public static int getSideFromID( int id )
    {
	if ( isLeftPlayer( id ) || isLeftGoal( id ) )
	    return SIDE_LEFT;
	if ( isRightPlayer( id ) || isRightGoal( id ) )
	    return SIDE_RIGHT;
	return SIDE_ILLEGAL;
    }

    public static int getBallID()
    {
	return ID_BALL;
    }

    public static int getLeftPlayerID( int unum )
    {
	return ID_PLAYER_L_1 + unum - 1;
    }

    public static int getRightPlayerID( int unum )
    {
	return ID_PLAYER_R_1 + unum - 1;
    }

    public static int getPlayerID( int side, int unum )
    {
	if ( side == SIDE_LEFT )
	    return getLeftPlayerID( unum );
	if ( side == SIDE_RIGHT )
	    return getRightPlayerID( unum );
	return ID_ILLEGAL;
    }

    public static String getPlayModeString( int pm )
    {
	return PLAYMODE_STRINGS[ pm ];
    }

    public static String stripPath( String filename )
    {
	String retVal = new String();
	StringTokenizer st =
	    new StringTokenizer( filename, "/" );
	while ( st.hasMoreTokens() ) {
	    retVal = st.nextToken();
	}
	return retVal;
    }

    public static String unescape( String string )
    {
	final StringBuffer result = new StringBuffer();

	final StringCharacterIterator iterator = 
	    new StringCharacterIterator( string );
	char c = iterator.current();
	
	boolean escaped = false;
	while ( c != StringCharacterIterator.DONE ) {
	    if ( escaped ) {
		switch ( c ) {
		case 'n': 
		    result.append( '\n' );
		    break;
		default:
		    result.append( c );
		    break;
		}
		escaped = false;
	    }
	    else { 
		switch ( c ) {
		case '\\':
		    escaped = true;
		    break;
		case '"':
		    // Don't print quotes
		    break;
		default:
		    result.append( c );
		    break;
		}
	    }
	    c = iterator.next();
	}
	
	return result.toString();
    }
}
