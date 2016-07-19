import java.util.Arrays;
import java.util.List;

public class RCSSParser implements SoccerTypes
{
    private WorldState WS;
    private ServerParams SP;

    public RCSSParser( WorldState WS, ServerParams SP )
    {
	this.WS = WS;
	this.SP = SP;
    }

    public boolean analyzeMessage( String msg ) 
    {
	if ( msg == null || msg.length() < 2 )
	    return false;

	
	switch ( msg.charAt( 1 ) ) {
	case 'c':
	    return analyzeCLangVersionMessage( msg );
	case 'o':
	    // ok
	    return true;
	case 's':
	    switch( msg.charAt( 3 ) ) {
	    case 'e':
		return analyzeSeeGlobalMessage( msg );
	    case 'r': 
		return analyzeServerParamMessage( msg );
	    default: 
		break;
	    }
	case 'i':     
	    return analyzeInitMessage( msg );
	case 'h':     
	    return analyzeHearMessage( msg );
	case 'p':    
	    if ( msg.charAt( 8 ) == 't' )
		return analyzePlayerTypeMessage( msg );
	    else
		return analyzePlayerParamMessage( msg );
	case 'e':     
	    System.err.println( msg );
	    break;
	case 't':  //think - don't need to do anything
	    break;
	default:
	    System.err.println( "Ignored message: " + msg ); 
	    return false;
	}
	return true;
    }

    public boolean analyzeCLangVersionMessage( String msg )
    {
	// ignore
	return true;
    }

    public boolean analyzeSeeGlobalMessage( String msg )
    {
	int pos, next;

	pos = 12;
	next = msg.indexOf( ' ', pos );
	int time = Integer.parseInt( msg.substring( pos, next ) );
	pos = next;
	WS.updateTime( time );
	
	double x, y, vx, vy, angBody, angNeck;
	while ( msg.charAt( pos ) != ')' ) {
	    pos += 3;
	    switch ( msg.charAt( pos ) ) {
	    case 'b':
		pos += 3;
		next = msg.indexOf( ' ', pos );
		x = Double.parseDouble( msg.substring( pos, next ) );
		pos = next + 1;
		next = msg.indexOf( ' ', pos );
		y = Double.parseDouble( msg.substring( pos, next ) );
		pos = next + 1;
		next = msg.indexOf( ' ', pos );
		vx = Double.parseDouble( msg.substring( pos, next ) );
		pos = next + 1;
		next = msg.indexOf( ')', pos );
		vy = Double.parseDouble( msg.substring( pos, next ) );
		pos = next + 1;
		WS.updateBall( new VecPosition( x, y ),
			       new VecPosition( vx, vy ) );
		break;
	    case 'p':
		pos += 3;
		next = msg.indexOf( '\"', pos );
		String team = msg.substring( pos, next );
		int side = WS.getSideFromTeamName( team );
		if ( side == SIDE_ILLEGAL ) {
		    if ( WS.getTeamName( SIDE_LEFT ).equals( "" ) ) {
			side = SIDE_LEFT;
		    }
		    else if ( WS.getTeamName( SIDE_RIGHT ).equals( "" ) ) {
			side = SIDE_RIGHT;
		    }
		    WS.setTeamName( side, team );
		}
		pos = next + 2;
		int space = msg.indexOf( ' ', pos );
		int paren = msg.indexOf( ')', pos );
		int unum;
		boolean goalie;
		if ( space != -1 && space < paren ) {
		    unum = Integer.parseInt( msg.substring( pos, space ) );
		    pos = msg.indexOf( ')', space ) + 2;
		    goalie = true;
		}
		else {
		    unum = Integer.parseInt( msg.substring( pos, paren ) );
		    pos = paren + 2;
		    goalie = false;
		}
		next = msg.indexOf( ' ', pos );
		x = Double.parseDouble( msg.substring( pos, next ) );
		pos = next + 1;
		next = msg.indexOf( ' ', pos );
		y = Double.parseDouble( msg.substring( pos, next ) );
		pos = next + 1;
		next = msg.indexOf( ' ', pos );
		vx = Double.parseDouble( msg.substring( pos, next ) );
		pos = next + 1;
		next = msg.indexOf( ' ', pos );
		vy = Double.parseDouble( msg.substring( pos, next ) );
		pos = next + 1;
		next = msg.indexOf( ' ', pos );
		angBody = Double.parseDouble( msg.substring( pos, next ) );
		pos = next + 1;
		next = msg.indexOf( ')', pos );
		angNeck = Double.parseDouble( msg.substring( pos, next ) );
		pos = next + 1;
		//gjk add isGoalie
		WS.updatePlayer( Utils.getPlayerID( side, unum ), new VecPosition( x, y ),
				 new VecPosition( vx, vy ),
				 angBody, angNeck );
		break;
	    case 'g':
		// ignore
		pos = msg.indexOf( ')', pos );
		pos = msg.indexOf( ')', pos + 1 );
		pos++;
		break;
	    default:
		System.err.println( "Error parsing see global message at: " +
				    msg.substring( pos, msg.length() ) );
		return false;
	    }
	}
	return true;
    }

    public boolean analyzeServerParamMessage( String msg )
    {
	int pos, next;

	pos = 14;
	while ( msg.charAt( pos ) != ')' ) {
	    pos++;
	    next = msg.indexOf( ' ', pos );
	    String param = msg.substring( pos, next );
	    pos = next + 1;
	    next = msg.indexOf( ')', pos );
	    String value = msg.substring( pos, next );
	    pos = next + 1;
	    SP.setParam( param, value );
	   // System.out.println( "SP: " + param + " = " + 
	   // 			SP.getParam( param ) );
	}
	return true;
    }
    
    public boolean analyzeInitMessage( String msg )
    {
	if ( msg.charAt( 6 ) == 'l' )
	    WS.setSide( SIDE_LEFT );
	else if ( msg.charAt( 6 ) == 'r' )
	    WS.setSide( SIDE_RIGHT );
	else {
	    WS.setSide( SIDE_ILLEGAL );
	    return false;
	}
	return true;
    }

    public boolean analyzeHearMessage( String msg )
    {
	int pos, next;

	pos = 6;

	/*
		A typical message resembles
			(hear referee 214 ****)
		or
			(hear (p keeper_4) 22 ****)

		msg.charAt(6) is used to determine the source (player/referee)		
	*/

        Boolean referee = Arrays.asList(msg.split(" ")).contains("referee");
        Boolean player = Arrays.asList(msg.split(" ")).contains("(p");

        if (referee && player)
          System.err.println( "::::::::Error parsing hear message: " + msg );
           
	if (player) {
	//switch ( msg.charAt( pos ) ) {
	//case '(': //This is a message from a player
		int i, j, k;
  
	  	double[] tempReceivedState = new double[100];
	  	int numStates;
		int tempReceivedAction;
	  	double tempReceivedReward;
	
	  	String buffer;
		char[] msgArray;
		int startIndex, endIndex;
	
		msgArray = msg.toCharArray(); 
	
		numStates = 0;
		   
  		i = 0;
  		while(i < msg.length())
		{
			if(msgArray[i] == '*')
				break;
	  		i++;
		}
		
  		if(i == msg.length())
		  	return true;//This was not a state-action message!	
			
		i++;
		if(msgArray[i] != 'k')
		  	return true;//This was not a state-action message!	
		i++;
		WS.messageSayer	= msgArray[i] - '0';
		
  		while(i < msg.length())
		{
			if(msgArray[i] == '*')
				break;
	  		i++;
		}
		
  		if(i == msg.length())
		  	return true;//This was not a state-action message!	
		
		i++;
	
	  	startIndex = i;
		while(i < msg.length())
		{
			if(msgArray[i] == '*')
				break;
				
			if(msgArray[i] == ' ')
			{
				endIndex = i;
				buffer = msg.substring(startIndex, endIndex);
				try
				{
					tempReceivedState[numStates] = (new Double(buffer)).doubleValue();
				}
				catch(Exception e)
				{
					System.out.println("Exception caught. Message: " + msg);
					return true;//This was not a state-action message
				}
				numStates++;
				startIndex = i + 1;			
			}
			i++;
		}
		
		if(i == msg.length())
	  		return true;//This was not a state-action message
			  
		i++;
		startIndex = i;
		while(i < msg.length())
		{
			if(msgArray[i] == ' ')
				break;
				
			i++;
		}
		
		endIndex = i;
		buffer = msg.substring(startIndex, endIndex);
		try
		{
		tempReceivedAction = (new Integer(buffer)).intValue();
		}
		catch(Exception e)
		{
			System.out.println("Exception caught. Message:  " + msg);
			return true;//This was not a state-action message
		}
		
		//Update world model
	    	WS.lastPlayerState = tempReceivedState;
		WS.numStates = numStates;
		WS.lastPlayerAction = tempReceivedAction;
		
		WS.newPlayerMessageHeard = true;
	}
	//    	break;
		
	//case 'r':
        else if (referee) {
	    	pos = msg.indexOf( ' ', pos ) + 1;
	    	next = msg.indexOf( ')', pos );
	    	String pmode = msg.substring( pos, next );
	    	WS.setPlayMode( getPlayModeFromString( pmode ) );
	    	//break;
		//default:
        }
        else {
	    	System.err.println( "++++Error parsing hear message: " + msg );
	    	return false;
	}
	
		return true;
    }

    public int getPlayModeFromString( String s )
    {
	for ( int i = 0; i < PM_MAX; i++ ) {
	    if ( s.equals( PLAYMODE_STRINGS[ i ] ) )
		return i;
	}
	return PM_Null;
    }

    public boolean analyzePlayerTypeMessage( String msg )
    {
	// ignore
	return true;
    }

    public boolean analyzePlayerParamMessage( String msg )
    {
	int pos, next;

	pos = 14;
	while ( msg.charAt( pos ) != ')' ) {
	    pos ++;
	    next = msg.indexOf( ' ', pos );
	    String param = msg.substring( pos, next );
	    pos = next + 1;
	    next = msg.indexOf( ')', pos );
	    String value = msg.substring( pos, next );
	    pos = next + 1;
	    SP.setParam( param, value );
	    //System.out.println( "PP: " + param + " = " + 
	    //			SP.getParam( param ) );
	}
	return true;
    }
}
