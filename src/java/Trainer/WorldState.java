import java.io.Serializable;

public class WorldState implements SoccerTypes, Serializable
{
    private ServerParams SP;

    private int time;
    private int playMode;
    private String[] teamNames;
    private int[] scores;

    private PlayerObject[] players;
    private BallObject ball;

    private int ourSide, oppSide;
    
    public double[] lastPlayerState;
    public int numStates;
    public int lastPlayerAction;
    public boolean newPlayerMessageHeard;
    public int messageSayer;//A keeper

    public WorldState( ServerParams SP )
    {
	this( SP, SIDE_ILLEGAL );
    }

    public WorldState( ServerParams SP, int side )
    {
	this.SP = SP;
	setPlayMode( PM_Null );
	setSide( side );

	scores = new int[ NUM_TEAMS ];

	teamNames = new String[ NUM_TEAMS ];
	teamNames[ SIDE_LEFT ]  = new String();
	teamNames[ SIDE_RIGHT ] = new String();

	players = new PlayerObject[ MAX_PLAYERS ];
	for ( int i = 0; i < MAX_PLAYERS; i++ ) {
	    players[ i ] = new PlayerObject();
	    // Set initial position of player on sideline
	    int s = ( i < MAX_PLAYERS_TEAM ) ? -3 : 3;
	    players[ i ].setPosition( new VecPosition( s * ( i % MAX_PLAYERS_TEAM + 1 ), 
						       SIDELINES_Y ) );
	}
	ball = new BallObject();

    }

    public WorldState copy( WorldState ws )
    {
	SP = ws.SP;
	updateTime( ws.getTime() );
	setPlayMode( ws.getPlayMode() );
	setSide( ws.getOurSide() );

	for ( int i = 0; i < NUM_TEAMS; i++ ) {
	    setScore( i, ws.getScore( i ) );
	    setTeamName( i, ws.getTeamName( i ) );
	}

	for ( int i = 0; i < MAX_PLAYERS; i++ ) {
	    players[ i ].copy( ws.players[ i ] );
	}
	ball.copy( ws.ball );

	return this;
    }

    public void updateTime( int time )
    {
	this.time = time;
    }

    public int getTime()
    {
	return time;
    }

    public void setPlayMode( int playMode )
    {
	this.playMode = playMode;
    }

    public int getPlayMode()
    {
	return playMode;
    }

    public String getPlayModeString()
    {
	return Utils.getPlayModeString( getPlayMode() );
    }

    public void setSide( int side )
    {
	if ( side == SIDE_LEFT ) {
	    ourSide = SIDE_LEFT;
	    oppSide = SIDE_RIGHT;
	}
	else if ( side == SIDE_RIGHT ) {
	    ourSide = SIDE_RIGHT;
	    oppSide = SIDE_LEFT;
	}
	else {
	    ourSide = SIDE_ILLEGAL;
	    oppSide = SIDE_ILLEGAL;
	}
    }

    public int getOurSide()
    {
	return ourSide;
    }

    public int getOppSide()
    {
	return oppSide;
    }

    public int getSideFromTeamName( String teamName )
    {
	if ( teamName.equals( teamNames[ SIDE_LEFT ] ) )
	    return SIDE_LEFT;
	if ( teamName.equals( teamNames[ SIDE_RIGHT ] ) )
	    return SIDE_RIGHT;
	return SIDE_ILLEGAL;
    }

    public void setSideFromTeamName( String teamName )
    {
	setSide( getSideFromTeamName( teamName ) );
    }

    public void setTeamName( int side, String teamName )
    {
	teamNames[ side ] = teamName;
    }

    public void setLeftTeamName( String teamName )
    {
	setTeamName( SIDE_LEFT, teamName );
    }

    public void setRightTeamName( String teamName )
    {
	setTeamName( SIDE_RIGHT, teamName );
    }

    public void setOurTeamName( String teamName )
    {
	setTeamName( getOurSide(), teamName );
    }

    public void setOppTeamName( String teamName )
    {
	setTeamName( getOppSide(), teamName );
    }

    public String getTeamName( int side )
    {
	return teamNames[ side ];
    }

    public String getLeftTeamName()
    {
	return getTeamName( SIDE_LEFT );
    }

    public String getRightTeamName()
    {
	return getTeamName( SIDE_RIGHT );
    }

    public String getOurTeamName()
    {
	return getTeamName( getOurSide() );
    }

    public String getOppTeamName()
    {
	return getTeamName( getOppSide() );
    }

    public void setScore( int side, int score )
    {
	scores[ side ] = score;
    }

    public void setLeftScore( int score )
    {
	setScore( SIDE_LEFT, score );
    }

    public void setRightScore( int score )
    {
	setScore( SIDE_RIGHT, score );
    }
    
    public void setOurScore( int score )
    {
	setScore( getOurSide(), score );
    }

    public void setOppScore( int score )
    {
	setScore( getOppSide(), score );
    }

    public int getScore( int side )
    {
	return scores[ side ];
    }

    public int getLeftScore()
    {
	return getScore( SIDE_LEFT );
    }

    public int getRightScore()
    {
	return getScore( SIDE_RIGHT );
    }

    public int getOurScore()
    {
	return getScore( getOurSide() );
    }

    public int getOppScore()
    {
	return getScore( getOppSide() );
    }

    public int getGoalDifference()
    {
	return getOurScore() - getOppScore();
    }

    public boolean isTeammate( int id )
    {
	return Utils.isPlayer( id ) &&
	    Utils.getSideFromID( id ) == getOurSide();
    }

    public boolean isOpponent( int id )
    {
	return Utils.isPlayer( id ) &&
	    Utils.getSideFromID( id ) == getOppSide();
    }

    public int getTeammateID( int unum )
    {
	return Utils.getPlayerID( getOurSide(), unum );
    }

    public int getOpponentID( int unum )
    {
	return Utils.getPlayerID( getOppSide(), unum );
    }

    public boolean isOnSidelines( int id )
    {
	return getPlayerPosition( id ).getY() == SIDELINES_Y;
    }
    
    public void updateBall( VecPosition pos, VecPosition vel )
    {
	ball.setTimeLastSeen( getTime() );
	ball.setPosition( pos );
	ball.setVelocity( vel );
    }

    public void updatePlayer( int id, 
			      VecPosition pos, VecPosition vel, 
			      double angBody, double angNeck )
    {
	PlayerObject player = players[ id ];

	player.setTimeLastSeen( getTime() );
	player.setPosition( pos );
	player.setVelocity( vel );
	player.setBodyAngle( angBody );
	player.setNeckAngle( angNeck );
    }    

    public VecPosition getBallPosition()
    {
	return ball.getPosition();
    }

    public int getTimeSinceSeenBall()
    {
	return getTime() - ball.getTimeLastSeen();
    }

    public VecPosition getPlayerPosition( int id )
    {
	return players[ id ].getPosition();
    }

    public int getTimeSinceSeenPlayer( int id )
    {
	return getTime() - players[ id ].getTimeLastSeen();
    }

    public VecPosition getPosition( int id )
    {
	if ( Utils.isBall( id ) ) 
	    return getBallPosition();
	if ( Utils.isPlayer( id ) )
	    return getPlayerPosition( id );
	if ( Utils.isLeftGoal( id ) )
	    return new VecPosition( -PITCH_LENGTH / 2, 0 );
	if ( Utils.isRightGoal( id ) )
	    return new VecPosition(  PITCH_LENGTH / 2, 0 );
	return null;
    }

    public VecPosition getBallVelocity()
    {
	return ball.getVelocity();
    }

    public VecPosition getPlayerVelocity( int id )
    {
	return players[ id ].getVelocity();
    }

    public VecPosition getVelocity( int id )
    {
	if ( Utils.isBall( id ) ) 
	    return getBallVelocity();
	if ( Utils.isPlayer( id ) )
	    return getPlayerVelocity( id );
	if ( Utils.isGoal( id ) )
	    return new VecPosition();
	return null;
    }

    public double getPlayerBodyAngle( int id )
    {
	return players[ id ].getBodyAngle();
    }

    public double getPlayerNeckAngle( int id )
    {
	return players[ id ].getNeckAngle();
    }

    public int getHeteroPlayerType( int id )
    {
	return players[ id ].getHeteroPlayerType();
    }

    public boolean isBallKickableBy( int id )
    {
	return getBallPosition().getDistanceTo( getPlayerPosition( id ) ) <
	    SP.kickable_margin + SP.player_size + SP.ball_size;
    }

}
