import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.util.*;

public class Monitor implements SoccerTypes
{
    private MonitorParams MP;
    private ServerParams SP;
    private MonitorListener listener;
    private FieldCanvas field;
    private JFrame frame;

    public Monitor()
    {
	this( null );
    }

    public Monitor( MonitorParams MP )
    {
	this( MP, null );
    }

    public Monitor( MonitorParams MP, ServerParams SP )
    {
	this( MP, SP, null );
    }

    public Monitor( MonitorParams MP, ServerParams SP,
		    MonitorListener listener )
    {
	if ( MP == null )
	    this.MP = new MonitorParams();
	else
	    this.MP = MP;

	if ( SP == null )
	    this.SP = new ServerParams();
	else
	    this.SP = SP;

	if ( listener == null ) 
	    this.listener = new MonitorListener( this );
	else
	    this.listener = listener;

	initializeFrame();
    }

    private void createMenuBar()
    {
        JMenuBar menuBar = new JMenuBar();
        menuBar.setOpaque( true );

	createFileMenu( menuBar );

        frame.setJMenuBar(menuBar);
    }

    private void createFileMenu( JMenuBar menuBar )
    {
	// file menu
	JMenu menu = new JMenu( "File" );
	menu.setMnemonic( KeyEvent.VK_F );
	menu.getAccessibleContext().setAccessibleDescription( "The File Menu" );
	menuBar.add( menu );

	// file->quit
	JMenuItem menuItem = new JMenuItem( "Quit", KeyEvent.VK_Q );
	menuItem.setAccelerator( KeyStroke.getKeyStroke( KeyEvent.VK_Q, 
							 ActionEvent.ALT_MASK ) );
	menuItem.getAccessibleContext().setAccessibleDescription( "Quit the program" );
	menuItem.addActionListener( listener );
	menu.add( menuItem );
    }

    private void initializeFrame()
    {
	// disable eye candy around the window
        JFrame.setDefaultLookAndFeelDecorated( false );

        // create and set up the window
        frame = new JFrame( MP.title );
        frame.setDefaultCloseOperation( JFrame.EXIT_ON_CLOSE );

	// create menu
	createMenuBar();

	// create field canvas
	field = new FieldCanvas( frame, this );
	field.setPreferredSize( new Dimension( MP.window_size_x, MP.window_size_y ) );
	field.addMouseListener( listener );
	field.addMouseMotionListener( listener );
	frame.getContentPane().add( field );
	field.setVisible( true );

        // display the main frame
        frame.pack();
	frame.addWindowStateListener( listener );
        frame.setVisible( true );

	update( null );
    }

    private Vector getBallShape( WorldState ws )
    {
	Vector shapes = new Vector();

	double radius = 1.3;
	shapes.add( new FieldCircle( MP.color_ball, true,
				     ws.getBallPosition(),
				     MP.ball_radius, MP.depth_ball ) );
	shapes.add( new FieldCircle( MP.color_ball, false,
				     ws.getBallPosition(),
				     radius, MP.depth_ball ) );
	return shapes;
    }

    private Vector getPlayerShape( WorldState ws, int id )
    {
	Vector shapes = new Vector();
	
	if ( !ws.isOnSidelines( id ) || MP.show_sideline_players ) { 
	    VecPosition pos = ws.getPlayerPosition( id );
	    double radius = 1.1;
	    double inner = 0.3;
	    Color body_color = 
		Utils.isLeftPlayer( id ) ? MP.color_team_l : MP.color_team_r;
	    Color unum_color = 
		Utils.isLeftPlayer( id ) ? MP.color_unum_l : MP.color_unum_r;
	    shapes.add( new FieldCircle( body_color, true, 
					 pos, radius,
					 MP.depth_player_body ) );
	    shapes.add( new FieldCircle( MP.color_player_outline, false, 
					 pos, radius,
					 MP.depth_player_outline ) );
	    shapes.add( new FieldCircle( MP.color_player_outline, false, 
					 pos, inner,
					 MP.depth_player_outline ) );
	    VecPosition bodyVector = 
		new VecPosition( radius, ws.getPlayerBodyAngle( id ), true );
	    VecPosition neckVector = 
		new VecPosition( radius, ws.getPlayerNeckAngle( id ) + 
				 ws.getPlayerBodyAngle( id ), true );
	    shapes.add( new FieldLine( MP.color_body_ang,
				       pos, pos.add( bodyVector ),
				       MP.depth_player_decorations ) );
	    shapes.add( new FieldLine( MP.color_neck_ang,
				       pos, pos.add( neckVector ),
				       MP.depth_player_decorations ) );
	    shapes.add( new FieldText( unum_color, pos, 
				       Utils.getUnumFromID( id ) + "",
				       MP.depth_player_unum ) );
	}
	
	return shapes;
    }

    private Vector getWorldStateShapes( WorldState ws )
    {
	Vector shapes = new Vector();

	if ( ws != null ) {
	    if ( MP.show_ball ) {
		shapes.addAll( getBallShape( ws ) );
	    }

	    if ( MP.show_players ) {
		for ( int i = 0; i < MAX_PLAYERS; i++ ) {
		    shapes.addAll( getPlayerShape( ws, i ) );
		}
	    }
	}

	return shapes;
    }

    public void update( WorldState ws )
    {
	update( ws, null );
    }

    public void update( WorldState ws, Vector shapes )
    {
	Vector toSend = getWorldStateShapes( ws );
	if ( shapes != null )
	    toSend.addAll( shapes );
	Collections.sort( toSend );

	field.updateCanvasSize();
	field.drawField( MP );
	field.drawShapes( toSend );
	repaint();
    }

    public void repaint()
    {
	field.repaint();
    }

    public void quit()
    {
	System.exit( 0 );
    }

    // Testing purposes
    public static void main( String[] args ) 
    {
	Monitor monitor = new Monitor();
	while ( true ) {
	    try { Thread.sleep( 100 ); } catch ( Exception e ) {}
	    monitor.update( null );
	}
    }
}

