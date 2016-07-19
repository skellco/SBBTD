import java.io.*;

public class GameLog implements SoccerTypes
{
    final int REC_VERSION_NONE = 0;
    final int REC_VERSION_OLD = 1;
    final int REC_VERSION_2 = 2;
    final int REC_VERSION_3 = 3;

    final int NO_INFO = 0;
    final int SHOW_MODE = 1;
    final int MSG_MODE = 2;
    final int DRAW_MODE = 3;
    final int BLANK_MODE = 4;
    final int PM_MODE = 5;
    final int TEAM_MODE = 6;
    final int PT_MODE = 7;
    final int PARAM_MODE = 8;
    final int PPARAM_MODE = 9;

    private WorldState WS;
    private InputStream logFile;
    private int logVersion;
    private ServerParams SP;

    private BallT ballT;
    private PlayerParamsT playerParamsT;
    private PlayerT playerT;
    private PlayerTypeT playerTypeT;
    private ServerParamsT serverParamsT; 
    private ShortShowinfoT2 shortShowinfoT2;
    private TeamT[] teams;

    public GameLog( String filename )
    {
	try {
	    logFile = new FileInputStream( filename );
	}
	catch( FileNotFoundException e ) {
	    System.err.println( "GameLog: File not found: " + filename );
	    System.exit( 1 );
	}

	logVersion = REC_VERSION_NONE;
	readVersion();

	ballT = new BallT();
	playerParamsT = new PlayerParamsT();
	playerT = new PlayerT();
	playerTypeT = new PlayerTypeT();
	serverParamsT = new ServerParamsT(); 
	shortShowinfoT2 = new ShortShowinfoT2();
	teams = new TeamT[ 2 ];
	for ( int i = 0; i < 2; i++ ) {
	    teams[ i ] = new TeamT();
	}
    }

    public void setWorldState( WorldState WS )
    {
	this.WS = WS;
    }

    public void setServerParams( ServerParams SP )
    {
	this.SP = SP;
    }

    public static int s2i( short s )
    {
	return (int) s;
    }

    private int l2i( long l )
    {
	return (int) Math.round( l2d( l ) );
    }

    public static double l2d( long l )
    {
	return l / 65536.0;
    }

    private void readVersion()
    {
	byte[] buffer = new byte[128];
	
	try {
	    logFile.read( buffer, 0, 4 );
	}
	catch ( IOException e ) {
	    System.err.println( "GameLog: Unable to read version number" );
	    System.exit( 1 );
	}

	String s = new String( buffer, 0, 4 );
	if ( s.substring( 0, 3 ).equals( "ULG" ) ) {
	    logVersion = (int) buffer[ 3 ];
	}
	else {
	    logVersion = REC_VERSION_OLD;
	}
    }

    public void readPlayMode()
    {
	byte pmode = Struct.readByte( logFile );
	updatePlayMode( pmode );
    }

    public void readTeams()
    {
	for ( int i = 0; i < 2; i++ ) {
	    teams[ i ].read( logFile );
	}
	updateTeams( teams );
    }

    public void readShowInfo()
    {
	shortShowinfoT2.read( logFile );
	updateShowInfo( shortShowinfoT2 );
    }

    public void readMessage()
    {
	short board = Struct.readShort( logFile, false );
	short len = Struct.readShort( logFile, false );
	byte[] msg = new byte[ 2048 ];
	try {
	    logFile.read( msg, 0, s2i( len ) );
	}
	catch ( IOException e ) {
	    System.err.println( e );
	    return;
	}
	updateMessage( board, msg, len );
    }

    public void readServerParams()
    {
	serverParamsT.read( logFile );
	updateServerParams( serverParamsT );
    }

    public void readPlayerParams()
    {
	playerParamsT.read( logFile );
	updatePlayerParams( playerParamsT );
    }

    public void readHeteroPlayerType()
    {
	playerTypeT.read( logFile );
	// Kludge to eat up extra byte in struct
	Struct.readShort( logFile, false );	
	updateHeteroPlayerType( playerTypeT );
    }

    public boolean readNext()
    {
	if ( logVersion == REC_VERSION_3 ) {
	    short mode = Struct.readShort( logFile, false );
	    if ( mode == 0 )
		mode = Struct.readShort( logFile, false );

	    switch ( s2i( mode ) ) {
	    case PM_MODE:
		readPlayMode();
		break;
	    case TEAM_MODE:
		readTeams();
		break;
	    case SHOW_MODE:
		readShowInfo();
		break;
	    case MSG_MODE:
		readMessage();
		break;
	    case PARAM_MODE:
		readServerParams();
		break;
	    case PPARAM_MODE:
		readPlayerParams();
		break;
	    case PT_MODE:
		readHeteroPlayerType();
		break;
	    default:
		if ( s2i( mode ) != WS.getTime() )
		    System.err.println( "GameLog: Invalid mode: " + 
					s2i( mode ) );
		return false;
	    }
	}
	else { // logVersion != REC_VERSION_3
	    System.err.println( "GameLog: Bad log version: " + logVersion );
	    return false;
	}
	
	return true;
    }

    public void updateServerParams( ServerParamsT sp )
    {
    }

    public void updatePlayerParams( PlayerParamsT pp )
    {
    }

    public void updateHeteroPlayerType( PlayerTypeT pt )
    {
    }

    public void updatePlayMode( byte pm )
    {
	WS.setPlayMode( pm );
    }

    public void updateTeams( TeamT[] teams )
    {
	WS.setLeftTeamName(  new String( teams[ SIDE_LEFT  ].name ).trim() );
	WS.setRightTeamName( new String( teams[ SIDE_RIGHT ].name ).trim() );
	WS.setLeftScore(  s2i( teams[ SIDE_LEFT  ].score ) );
	WS.setRightScore( s2i( teams[ SIDE_RIGHT ].score ) );
    }

    public void updateShowInfo( ShortShowinfoT2 si )
    {
	WS.updateTime( s2i( si.time ) );

	// Get ball info
	BallT b = si.ball;
	VecPosition bpos = new VecPosition( l2d( b.x ), l2d( b.y ) );
	VecPosition bvel = new VecPosition( l2d( b.deltax ), l2d( b.deltay ) );

// 	if ( WS.getSide() == SIDE_RIGHT ) {
// 	    bpos = bpos.negate();
// 	    bvel = bvel.negate();
// 	}
	
	WS.updateBall( bpos, bvel );

	// Get player info
	for ( int i = 0; i < MAX_PLAYERS; i++ ) {
	    PlayerT p = si.pos[ i ];
	    VecPosition ppos = new VecPosition( l2d( p.x ), l2d( p.y ) );
	    VecPosition pvel = new VecPosition( l2d( p.deltax ), l2d( p.deltay ) );
	    double angBody = Math.toDegrees( l2d( p.body_angle ) );
	    double angNeck = Math.toDegrees( l2d( p.head_angle ) );

// 	    if ( WS.getSide() == SIDE_RIGHT ) {
// 		ppos = ppos.negate();
// 		pvel = pvel.negate();
// 		angBody = Utils.normalizeAngle( angBody + 180 );
// 		angNeck = Utils.normalizeAngle( angNeck + 180 );
// 	    }

	    WS.updatePlayer( i, ppos, pvel, angBody, angNeck );
	}
    }

    public void updateMessage( short board, byte[] msg, short len )
    {
	/* Ignore */
    }

    // Testing purposes
    ///////////////////////////////////////////////////////////////////////
//     private static void processCycle( WorldState ws )
//     {
// 	if ( ws.getBallPosition().getX() < 0 ) {
// 	    cycles++;
// 	    p1 += ws.getPlayerPosition( ws.getTeammateID( 1 ) ).getY() * -Utils.sign( ws.getBallPosition().getY() ); 
// 	    p23 += ws.getPlayerPosition( ws.getTeammateID( 2 ) ).getDistanceTo( ws.getPlayerPosition( ws.getTeammateID( 3 ) ) );
// 	}
//     }

//     public static void main( String[] args ) 
//     {
// 	ServerParams sp = new ServerParams();
// 	WorldState ws = new WorldState( sp, SIDE_LEFT );
// 	GameLog gl = new GameLog( args[ 0 ] );
// 	int lastTime = 0;
// 	gl.setWorldState( ws );
// 	while ( ws.getPlayMode() != PM_TimeOver ) {
//             if ( !gl.readNext() )
//                 break;

//             if ( ws.getTime() > lastTime ) {
// 		processCycle( ws );

// 		if ( cycles >= 500 )
// 		    break;

//                 if ( ws.getTime() % 100 == 0 )
//                     System.out.print( "." );
//                 lastTime = ws.getTime();
//             }
//         }
// 	p1 /= cycles;
// 	p23 /= cycles;
// 	if ( p1 > 3 ) {
// 	    System.out.println( "SamplePattern2" );
// 	}
// 	else if ( p23 < 5 ) {
// 	    System.out.println( "SamplePattern3" );
// 	}
// 	else {
// 	    System.out.println( "SamplePattern1" );
// 	}
//     }
}
