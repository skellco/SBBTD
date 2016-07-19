public class CoachCommandHandler
{
    protected Connection C;
    protected WorldState WS;

    public CoachCommandHandler( Connection c )
    {
	this( c, null );
    }

    public CoachCommandHandler( Connection c, WorldState ws )
    {
	C = c;
	setWorldState( ws );
    }

    public void setWorldState( WorldState ws )
    {
	WS = ws;
    }

    public void init( String teamName, double version )
    {
	String cmd = "(init " + teamName + " (version " +
	    version + "))";
	C.send( cmd );
    }

    public void say( String message )
    {
	String cmd = "(say " + message + ")";
	C.send( cmd );
    }

    public void changePlayerType( int id, int playerType )
    {
	if ( !WS.isTeammate( id ) ) {
	    System.err.println( "changePlayerType: Object " + id + 
				" is not a teammate" );
	    return;
	}
	    
	int unum = Utils.getUnumFromID( id );
	String cmd = "(change_player_type " +
	    unum + " " + playerType + ")";
	C.send( cmd );
    }

    public void eye( boolean mode )
    {
	String cmd = "(eye " + ( mode ? "on" : "off" ) + ")";
	C.send( cmd );
    }

    public void teamNames()
    {
	String cmd = "(team_names)";
	C.send( cmd );
    }

    public void done()
    {
	String cmd = "(done)";
	C.send( cmd );
    }
}








