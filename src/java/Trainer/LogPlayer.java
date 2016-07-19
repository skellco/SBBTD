import java.util.*;
import java.io.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.border.*;

public class LogPlayer implements SoccerTypes,
				  ActionListener
{
    private static final int MAX_CYCLES = 6002;
    private static final int NORMAL_DELAY = 90;
    private static final int FAST_DELAY = 30;
    private static final String imgDir = "pngs/";

    private JFrame frame;
    private JLabel timeLabel;
    private JTextField goTextField;
    private JPanel listPanel;
    private Vector worldStates;
    private GameLog gl;
    private Monitor monitor;
    private Vector drawLogs;
    private Map logInfo;
    private Map checkBoxes;
    private JComboBox logList;
    private String rcgLog;
    private String stateLog;

    private int cycle;
    private int cycleInc;
    private javax.swing.Timer timer;

    public LogPlayer( String rcgLog, String stateLog, Vector drawLogs )
    {
	this.rcgLog = rcgLog;
	this.stateLog = stateLog;
	this.drawLogs = drawLogs;
	this.logInfo = new HashMap();
	this.worldStates = new Vector();
	this.cycle = 1;
	if ( rcgLog != null )
	    this.gl = new GameLog( rcgLog );
    }

    private void createControls()
    {
        JPanel controls = new JPanel();
        controls.setLayout( new BoxLayout( controls, BoxLayout.X_AXIS ) );

        ImageIcon fastreverseIcon = new ImageIcon( imgDir + "fastreverse.png" );
        ImageIcon reverseIcon = new ImageIcon( imgDir + "reverse.png" );
        ImageIcon stepreverseIcon = new ImageIcon( imgDir + "stepreverse.png" );
        ImageIcon stopIcon = new ImageIcon( imgDir + "stop.png" );
        ImageIcon stepIcon = new ImageIcon( imgDir + "step.png" );
        ImageIcon playIcon = new ImageIcon( imgDir + "play.png" );
        ImageIcon fastforwardIcon = new ImageIcon( imgDir + "fastforward.png" );

	Dimension buttonSize = new Dimension( 25, 24 );

        JButton fastreverseButton = new JButton( fastreverseIcon );
        fastreverseButton.setActionCommand( "Fast Reverse" );
        fastreverseButton.addActionListener( this );	
	fastreverseButton.setPreferredSize( buttonSize );

        JButton reverseButton = new JButton( reverseIcon );
        reverseButton.setActionCommand( "Reverse" );
        reverseButton.addActionListener( this );	
	reverseButton.setPreferredSize( buttonSize );

        JButton stepreverseButton = new JButton( stepreverseIcon );
        stepreverseButton.setActionCommand( "Step Reverse" );
        stepreverseButton.addActionListener( this );	
	stepreverseButton.setPreferredSize( buttonSize );

        JButton stopButton = new JButton( stopIcon );
        stopButton.setActionCommand( "Stop" );
        stopButton.addActionListener( this );	
	stopButton.setPreferredSize( buttonSize );

        JButton stepButton = new JButton( stepIcon );
        stepButton.setActionCommand( "Step" );
        stepButton.addActionListener( this );	
	stepButton.setPreferredSize( buttonSize );

        JButton playButton = new JButton( playIcon );
        playButton.setActionCommand( "Play" );
        playButton.addActionListener( this );	
	playButton.setPreferredSize( buttonSize );

        JButton fastforwardButton = new JButton( fastforwardIcon );
        fastforwardButton.setActionCommand( "Fast Forward" );
        fastforwardButton.addActionListener( this );	
	fastforwardButton.setPreferredSize( buttonSize );

	controls.add( fastreverseButton );
	controls.add( reverseButton );
	controls.add( stepreverseButton );
	controls.add( stopButton );
	controls.add( stepButton );
	controls.add( playButton );
	controls.add( fastforwardButton );
        
	frame.getContentPane().add( controls );
    }

    private void initializeFrame()
    {
        JFrame.setDefaultLookAndFeelDecorated( false );

        frame = new JFrame( "JARCS Log Player" );
        frame.setDefaultCloseOperation( JFrame.EXIT_ON_CLOSE );

	frame.getContentPane().setLayout( new BoxLayout( frame.getContentPane(), 
							 BoxLayout.Y_AXIS ) );

	JPanel timePanel = new JPanel();
	timePanel.setBackground( Color.white );
	timeLabel = new JLabel( "time" );
	timePanel.add( timeLabel );
	frame.getContentPane().add( timePanel );

	JPanel filePanel = new JPanel();
	JLabel fileLabel;
	if ( rcgLog != null )
	    fileLabel = new JLabel( Utils.stripPath( rcgLog ) );
	else
	    fileLabel = new JLabel( Utils.stripPath( stateLog ) );
	filePanel.add( fileLabel );
	frame.getContentPane().add( filePanel );	

	createControls();

	JPanel goPanel = new JPanel();
	goPanel.setLayout( new BoxLayout( goPanel, 
					  BoxLayout.X_AXIS ) );
	goTextField = new JTextField( 6 );
	goTextField.setFont( new Font( "Monospaced", Font.BOLD, 14 ) );
	goTextField.setHorizontalAlignment( JTextField.RIGHT );
	goPanel.add( goTextField );
	JButton goButton = new JButton( "Jump" );
	goButton.addActionListener( this );
	goPanel.add( goButton );
	frame.getContentPane().add( goPanel );	



	JPanel westPanel = new JPanel();
	westPanel.setLayout( new BoxLayout( westPanel, BoxLayout.Y_AXIS ) );

	if ( logInfo != null ) {
	// Log File Panel
	JPanel logFilePanel = new JPanel( new BorderLayout() );
	Border logFileBorder = BorderFactory.createEtchedBorder();
	TitledBorder logFileTitledBorder = 
	    BorderFactory.createTitledBorder( logFileBorder,
					      "Log File");
	logFileTitledBorder.setTitleJustification( TitledBorder.RIGHT );
	logFilePanel.setBorder( logFileTitledBorder );

	SortedSet keys = new TreeSet( logInfo.keySet() );
	String[] logFilenames = 
	    (String[]) keys.toArray( new String[ 0 ] );
	Arrays.sort( logFilenames );
	logList = new JComboBox( logFilenames );
	logList.addActionListener( this );
	logFilePanel.add( logList );
	westPanel.add( logFilePanel );

	// Log Data Panel
	checkBoxes = new HashMap();
	for ( int i = 0; i < logFilenames.length; i++ ) {
	    LogDrawInfo ldi = (LogDrawInfo) logInfo.get( logFilenames[ i ] );
	    String[] idents = ldi.getIdents();
	    Arrays.sort( idents );
	    Vector boxes = new Vector();
	    for ( int j = 0; j < idents.length; j++ ) {
		JCheckBox box = new JCheckBox( idents[ j ] );
		box.addActionListener( this );
		boxes.add( box );
	    }
	    checkBoxes.put( logFilenames[ i ], boxes );
	}
	JPanel logDataPanel = new JPanel( new BorderLayout() );
	Border logDataBorder = BorderFactory.createEtchedBorder();
	TitledBorder logDataTitledBorder = 
	    BorderFactory.createTitledBorder( logDataBorder,
					      "Log Data");
	logDataTitledBorder.setTitleJustification( TitledBorder.RIGHT );
	logDataPanel.setBorder( logDataTitledBorder );

  	listPanel = new JPanel();
	listPanel.setLayout( new BoxLayout( listPanel, BoxLayout.Y_AXIS ) );
	updateCheckBoxes();

	JScrollPane listScroller = new JScrollPane( listPanel );
	listScroller.setPreferredSize(new Dimension(100, 200));
	logDataPanel.add( listScroller );
	westPanel.add( logDataPanel );

	JPanel buttonPanel = new JPanel();
	buttonPanel.setLayout( new BoxLayout( buttonPanel, 
					      BoxLayout.X_AXIS ) );
	JButton allButton = new JButton( "All" );
	allButton.addActionListener( this );
	buttonPanel.add( allButton );
 	JButton noneButton = new JButton( "None" );
	noneButton.addActionListener( this );
	buttonPanel.add( noneButton );

	westPanel.add( buttonPanel );

        frame.getContentPane().add( westPanel, BorderLayout.WEST );
	}

        frame.pack();
        frame.setVisible( true );
    }

    public void selectAll( boolean val )
    {
	Component[] comps = listPanel.getComponents();
	for ( int i = 0; i < comps.length; i++ ) {
	    JCheckBox box = (JCheckBox) comps[ i ];
	    box.setSelected( val );
	}
    }

    public void updateCheckBoxes()
    {
	listPanel.removeAll();
	String selected = (String) logList.getSelectedItem();
	Vector boxes = (Vector) checkBoxes.get( selected );
	if ( boxes != null ) {
	    Iterator i = boxes.iterator();
	    while ( i.hasNext() ) {
		listPanel.add( (JCheckBox) i.next() );
	    }
	    Dimension d = frame.getSize();
	    int x = (int) d.getWidth();
	    int y = (int) d.getHeight();
	    frame.setSize( x + 1, y + 1 );
	    frame.setSize( x, y );
	    frame.repaint();
	}
    }

    public void actionPerformed( ActionEvent e ) 
    {
	String cmd = e.getActionCommand();

	if ( cmd.equals( "Fast Reverse" ) ) {
	    timer.setDelay( FAST_DELAY );
	    cycleInc = -1;
	    timer.start();
	}
	else if ( cmd.equals( "Reverse" ) ) {
	    timer.setDelay( NORMAL_DELAY );
	    cycleInc = -1;
	    timer.start();
	}
	else if ( cmd.equals( "Step Reverse" ) ) {
	    cycleInc = -1;
	    timer.stop();
	    step();
	}
	else if ( cmd.equals( "Stop" ) ) {
	    timer.stop();
	}
	else if ( cmd.equals( "Step" ) ) {
	    cycleInc = 1;
	    timer.stop();
	    step();	    
	}
	else if ( cmd.equals( "Play" ) ) {
	    timer.setDelay( NORMAL_DELAY );
	    cycleInc = 1;
	    timer.start();
	}
	else if ( cmd.equals( "Fast Forward" ) ) {
	    timer.setDelay( FAST_DELAY );
	    cycleInc = 1;
	    timer.start();
	}
	else if ( cmd.equals( "Jump" ) ) {
	    try { 
		cycle = Integer.parseInt( goTextField.getText() );
	    }
	    catch ( NumberFormatException nfe ) {
		goTextField.setText( "Error" );
	    }
	}
	else if ( cmd.equals( "All" ) ) {
	    selectAll( true );
	} 
	else if ( cmd.equals( "None" ) ) {
	    selectAll( false );
	}
	else {
            updateCheckBoxes();
	}
	
	updateCycle();
    }

    private void parseDrawLog( String filename )
    {
	try {
	    BufferedReader br =
		new BufferedReader( new FileReader( filename ) );
	    LogDrawLexer lexer = new LogDrawLexer( br );
	    LogDrawParser parser = new LogDrawParser( lexer );
	    LogDrawInfo info = new LogDrawInfo( MAX_CYCLES );
	    logInfo.put( Utils.stripPath( filename ), info );
            parser.info = info;
	    System.out.print( "Parsing log: " + filename );
	    parser.file();
	    System.out.println( "...done" );
	}
	catch ( Exception e ) {
	    System.err.println( "Error parsing " + filename + ": " + e );
	}
    }

    private void parseGameLog()
    {
	if ( rcgLog != null ) {
	    System.out.print( "Parsing game log" );

	    int lastTime = -1;
	    WorldState ws = new WorldState( null );
	    gl.setWorldState( ws );
	    while ( ws.getPlayMode() != PM_TimeOver ) {
		if ( !gl.readNext() )
		    break;
		
		if ( ws.getTime() > lastTime ) {
		    worldStates.add( ws );
		    ws = ( new WorldState( null ) ).copy( ws );
		    gl.setWorldState( ws );
		    
		    if ( ws.getTime() % 100 == 0 )
			System.out.print( "." );
		    lastTime = ws.getTime();
		}
	    }
	}
	else { // use stateLog instead
	    System.out.print( "Parsing state log" );

	    try {
		ObjectInputStream ois = 
		    new ObjectInputStream( new FileInputStream( stateLog ) );
		LinkedList ll = (LinkedList) ois.readObject();
		Iterator i = ll.iterator();
		int cyc = 0;
		while ( i.hasNext() ) {
		    if ( cyc++ % 100 == 0 )
			System.out.print( "." );
		    worldStates.add( i.next() );
		}
	    }
	    catch ( Exception e ) {
		System.err.println( "Couldn't parse state log: " + e );
		System.exit( 1 );
	    }
	}
	
	System.out.println( "...done" );
    }

    public Vector getShapes( int cycle )
    {
	Vector shapes = new Vector();

	LogDrawInfo ldi =
	    (LogDrawInfo) logInfo.get( logList.getSelectedItem() );

	Component[] comps = listPanel.getComponents();

	if ( ldi != null ) {
	    for ( int i = 0; i < comps.length; i++ ) {
		JCheckBox box = (JCheckBox) comps[ i ];
		if ( box.isSelected() ) {
		    Vector newShapes = ldi.getInfo( cycle, box.getText() );
		    if ( newShapes != null )
			shapes.addAll( newShapes );
		}
	    }
	}

	// gjk
	shapes.add( new FieldRect( Color.WHITE, false,
				   new VecPosition( -10, -10 ),
				   new VecPosition(  10,  10 ), 0 ) );

	return shapes;
    }

    public WorldState getCycle( int cycle )
    {
	return (WorldState) worldStates.elementAt( cycle );
    }

    public void step()
    {
	updateCycle();
	cycle += cycleInc;
    }

    private void updateCycle()
    {
	if ( cycle >= worldStates.size() ) {
	    cycle = worldStates.size() - 1;
	    timer.stop();
	}
	else if ( cycle < 1 ) {
	    cycle = 1;
	    timer.stop();
	}
	timeLabel.setText( "time: " + cycle );
	monitor.update( getCycle( cycle ), 
			getShapes( cycle ) );
    }
    
    public void mainLoop()
    {
 	Iterator iter = drawLogs.iterator();
 	while ( iter.hasNext() ) {
 	    parseDrawLog( (String) iter.next() );
 	}

	parseGameLog();
	initializeFrame();

	// gjk
	MonitorParams mp = new MonitorParams();
	mp.show_middle_line = false;
	mp.show_center_circle = false;

	monitor = new Monitor( mp );	

	timer = new javax.swing.Timer( NORMAL_DELAY, new ActionListener() 
	    {
		public void actionPerformed( ActionEvent e ) 
		{
		    step();
		}    
	    } );
    }

    private static void printUsageAndExit()
    {
	System.out.println( "Usage: java LogPlayer [options] <gamelog>\n" );
	System.out.println( "  -d <drawlog>    Add LogDraw file" );
	System.out.println();
	System.exit( 0 );
    }

    public static void main( String[] args )
    {
	String rcgLog = null;
	String stateLog = null;
	Vector drawLogs = new Vector();

	for ( int i = 0; i < args.length; ) {
	    if ( args[ i ].charAt( 0 ) != '-' ) {
		rcgLog = args[ i ];
		i++;
	    }
	    else if ( args[ i ].equals( "-d" ) ) {
		drawLogs.add( args[ i+1 ] );
		i += 2;
	    }
	    else if ( args[ i ].equals( "-s" ) ) {
		stateLog = args[ i+1 ];
		i += 2;
	    }
	    else {
		System.out.println( "Unknown option: " + args[ i ] );
		printUsageAndExit();
	    }
	}
	if ( rcgLog == null && stateLog == null )
	    printUsageAndExit();

	LogPlayer lp = new LogPlayer( rcgLog, stateLog, drawLogs );
	lp.mainLoop();
    }
}


