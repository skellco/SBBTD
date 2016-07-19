public class Coach implements SoccerTypes
{
    ServerParams SP;
    WorldState WS;
    CoachCommandHandler CMD;
    Connection C;
    RCSSParser parser;
    String teamName;
    boolean launchMonitor;
    int cycles;
    double p1, p23;

    public Coach( Connection c,
		  String teamName,
		  boolean launchMonitor )
    {
	SP = new ServerParams();
	WS = new WorldState( SP );
	C = c;
	CMD = new CoachCommandHandler( C, WS );
        parser = new RCSSParser( WS, SP );
	this.teamName = teamName;
	this.launchMonitor = launchMonitor;
	cycles = 0;
	p1 = 0;
	p23 = 0;
    }

    public void mainLoop() 
    {
	final double version = 9.0;
	String reply;

	CMD.init( teamName, version );
	reply = C.receive();
	parser.analyzeMessage( reply );
	if ( WS.getOurSide() == SIDE_ILLEGAL ) {
	    System.err.println( "Unable to init: " + reply );
	    System.exit( 1 );
	}

	CMD.eye( true );

	int time = 0;
	while ( true ) {
	    reply = C.receive();
	    //System.out.println( reply );
	    parser.analyzeMessage( reply );
	    if ( WS.getTime() > time ) {
		time = WS.getTime();
		processCycle();
		if ( SP.synch_mode == 1 )
		    CMD.done();
		if ( cycles >= 600 )
		    break;
	    }
	    if ( WS.getPlayMode() == PM_TimeOver )
		break;
	}

	p1 /= cycles;
	p23 /= cycles;
	if ( p1 > 3 ) {
	    sayPattern( "SamplePattern2" );
	}
	else if ( p23 < 5 ) {
	    sayPattern( "SamplePattern3" );
	}
	else {
	    sayPattern( "SamplePattern1" );
	}


	C.disconnect();
	System.out.println( "Shutting down coach." );
    }

    private void processCycle()
    {
	if ( WS.getBallPosition().getX() < 0 ) {
	    cycles++;
	    p1 += WS.getPlayerPosition( WS.getTeammateID( 1 ) ).getY() * -Utils.sign( WS.getBallPosition().getY() ); 
	    p23 += WS.getPlayerPosition( WS.getTeammateID( 2 ) ).getDistanceTo( WS.getPlayerPosition( WS.getTeammateID( 3 ) ) );
	}	
    }

    private void sayPattern( String pattern )
    {
	CMD.say( "(freeform (pattern-detected " + pattern + " ))" );
	System.out.println( "Detected Pattern: " + pattern );
    }

    public static void main( String[] args )
    {
	String hostName = "localhost";
	int coachPort = 6002;
	boolean launchMonitor = false;
	String teamName = "Coachable";

	try {
	    for ( int i = 0; i < args.length; i += 2 ) {
		if ( args[ i ].equals( "-host" ) ) {
		    hostName = args[ i + 1 ];
		}
		else if ( args[ i ].equals( "-port" ) ) {
		    coachPort = Integer.parseInt( args[ i + 1 ] );
		}
		else if ( args[ i ].equals( "-monitor" ) ) {
		    launchMonitor = args[ i + 1 ].equals( "1" );
		}
		else if ( args[ i ].equals( "-team" ) ) {
		    teamName = args[ i + 1 ];
		}
		else {
		    System.err.println( "Unknown option: " + args[ i ] );
		}
	    }
	}
	catch ( Exception e ) {
	    System.err.println( "Unable to parse commandline options: " + e );
	    System.exit( 1 );
	}

	Connection c = new Connection( hostName, coachPort );
	Coach coach = new Coach( c, teamName, launchMonitor );
	coach.mainLoop();
    }
}
