public class TrainerCommandHandler 
    extends CoachCommandHandler 
    implements SoccerTypes
{
    public TrainerCommandHandler( Connection c )
    {
	super( c );
    }

    public TrainerCommandHandler( Connection c, WorldState ws )
    {
	super( c, ws );
    }

    private String getObjectStringFromID( int id )
    {
	if ( Utils.isBall( id ) ) {
	    return "(ball)";
	}
	else if ( Utils.isPlayer( id ) ) {
	    String teamName = WS.getTeamName( Utils.getSideFromID( id ) );
	    int unum = Utils.getUnumFromID( id );
	    return "(player " + teamName + " " + unum + ")";
	}

	System.err.println( "getObjectStringFromID: Object " + id + 
			    " is not a ball or player" );
	return null;
    }

    public void changeMode( int playMode )
    {
	String cmd = "(change_mode " + PLAYMODE_STRINGS[ playMode ] + ")";
	C.send( cmd );
    }

    public void move( int id, VecPosition pos )
    {
	move( id, pos, null );
    }

    public void move( int id, VecPosition pos,
			VecPosition vel )
    {
	move( id, pos, 0, vel );
    }

    public void move( int id, VecPosition pos,
			double vDir )
    {
	move( id, pos, vDir, new VecPosition() );
    }    

    public void move( int id, VecPosition pos,
			double vDir, VecPosition vel )
    {
	String cmd = "(move " + getObjectStringFromID( id ) + " " +
	    pos.getX() + " " + pos.getY();
	if ( vel != null ) {
	    cmd += " " + vDir + " " + 
		vel.getX() + " " + vel.getY();
	}
	cmd += ")";
	C.send( cmd );
    }

    public void checkBall()
    {
	String cmd = "(check_ball)";
	C.send( cmd );
    }

    public void start()
    {
	String cmd = "(start)";
	C.send( cmd );
    }

    public void recover()
    {
	String cmd = "(recover)";
	C.send( cmd );
    }

    public void ear( boolean mode )
    {
	String cmd = "(ear " + ( mode ? "on" : "off" ) + ")";
	C.send( cmd );
    }

    public void init( double version )
    {
	init( "", version );
    }

    public void changePlayerType( int id, int playerType )
    {
	String teamName = WS.getTeamName( Utils.getSideFromID( id ) );
	int unum = Utils.getUnumFromID( id );
	String cmd = "(change_player_type " + teamName +
	    " " + unum + " " + playerType + ")";
	C.send( cmd );
    }    
}
