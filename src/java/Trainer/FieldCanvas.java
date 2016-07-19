import java.awt.*;
import java.awt.event.*;
import java.awt.geom.*;
import javax.swing.*;
import java.awt.image.*;
import java.io.*;
import javax.imageio.*;
import java.util.*;

public class FieldCanvas extends JPanel implements SoccerTypes 
{
    final double TOTAL_X = PITCH_LENGTH + 10;
    final double TOTAL_Y = PITCH_WIDTH + 14;

    private JFrame frame;
    private Monitor monitor;

    private Image offscreenImage;
    private Graphics2D offscreenBuffer;

    private Dimension canvasSize;

    public FieldCanvas( JFrame frame, Monitor monitor ) 
    {
	super();

	this.frame = frame;
	this.monitor = monitor;
    }

    public void updateCanvasSize()
    {
        Dimension tmpDim = getSize();
	
	if ( canvasSize == null ||
	     !canvasSize.equals( tmpDim ) ) {
	    canvasSize = tmpDim;
	    offscreenImage = frame.createImage( canvasSize.width, 
						canvasSize.height );
	    offscreenBuffer = (Graphics2D) offscreenImage.getGraphics();
	}
    }

    public void drawField( MonitorParams mp )
    {
	double hlen = PITCH_LENGTH * 0.5;
	double hwid = PITCH_WIDTH * 0.5;
	double pen_x = hlen - PENALTY_AREA_LENGTH;
	double pen_y = PENALTY_AREA_WIDTH * 0.5;
	double goal_x = hlen - GOAL_AREA_LENGTH;
	double goal_y = GOAL_AREA_WIDTH * 0.5;
	double garea_x = hlen + GOAL_DEPTH;
	double garea_y = GOAL_WIDTH * 0.5 + GOAL_POST_RADIUS;

	offscreenBuffer.setBackground( mp.color_field );
	offscreenBuffer.clearRect( 0, 0, canvasSize.width, canvasSize.height );

	// left goal
	drawRect( new VecPosition( -hlen, -garea_y ),
		  new VecPosition( -garea_x, garea_y ), Color.BLACK, true );

	// right goal
	drawRect( new VecPosition( hlen, -garea_y ),
		  new VecPosition( garea_x, garea_y ), Color.BLACK, true );

	// penalty points
	drawCircle( new VecPosition( -hlen + PENALTY_SPOT_DIST, 0 ),
		    mp.penalty_spot_radius, Color.WHITE, true);
	drawCircle( new VecPosition(  hlen - PENALTY_SPOT_DIST, 0 ),
		    mp.penalty_spot_radius, Color.WHITE, true);

	// field lines
	drawRect( new VecPosition( -hlen, -hwid ),
		  new VecPosition( hlen, hwid ), Color.WHITE, false );

	// middle line
	if ( mp.show_middle_line ) {
	    drawLine( new VecPosition( 0, -hwid ),
		      new VecPosition( 0, hwid ), Color.WHITE );
	}

	// left penalty area
	drawLine( new VecPosition( -hlen, -pen_y ),
		  new VecPosition( -pen_x, -pen_y ), Color.WHITE );
	drawLine( new VecPosition( -pen_x, -pen_y ),
		  new VecPosition( -pen_x, pen_y ), Color.WHITE );
	drawLine( new VecPosition( -pen_x, pen_y ),
		  new VecPosition( -hlen, pen_y ), Color.WHITE );
	
	// left goal area
	drawLine( new VecPosition( -hlen, -goal_y ),
		  new VecPosition( -goal_x, -goal_y ), Color.WHITE );
	drawLine( new VecPosition( -goal_x, -goal_y ),
		  new VecPosition( -goal_x, goal_y ), Color.WHITE );
	drawLine( new VecPosition( -goal_x, goal_y ),
		  new VecPosition( -hlen, goal_y ), Color.WHITE );	

	// right penalty area
	drawLine( new VecPosition( hlen, -pen_y ),
		  new VecPosition( pen_x, -pen_y ), Color.WHITE );
	drawLine( new VecPosition( pen_x, -pen_y ),
		  new VecPosition( pen_x, pen_y ), Color.WHITE );
	drawLine( new VecPosition( pen_x, pen_y ),
		  new VecPosition( hlen, pen_y ), Color.WHITE );
	
	// right goal area
	drawLine( new VecPosition( hlen, -goal_y ),
		  new VecPosition( goal_x, -goal_y ), Color.WHITE );
	drawLine( new VecPosition( goal_x, -goal_y ),
		  new VecPosition( goal_x, goal_y ), Color.WHITE );
	drawLine( new VecPosition( goal_x, goal_y ),
		  new VecPosition( hlen, goal_y ), Color.WHITE );

	// center circle
	if ( mp.show_center_circle ) {
	    drawCircle( new VecPosition( 0, 0 ), 
			CENTER_CIRCLE_RADIUS, Color.WHITE, false );
	}

	// edge circlearcs
	drawArc( new VecPosition( -hlen, -hwid ),
		 mp.corner_arc_radius, 270, 90, Color.WHITE );
	drawArc( new VecPosition( hlen, -hwid ),
		 mp.corner_arc_radius, 180, 90, Color.WHITE );
	drawArc( new VecPosition( hlen, hwid ),
		 mp.corner_arc_radius, 90, 90, Color.WHITE );
	drawArc( new VecPosition( -hlen, hwid ),
		 mp.corner_arc_radius, 0, 90, Color.WHITE );

	// goal posts
// 	drawCircle( new VecPosition( hlen - GOAL_POST_RADIUS, garea_y ), 
// 		    GOAL_POST_RADIUS, Color.YELLOW, true );
// 	drawCircle( new VecPosition( hlen - GOAL_POST_RADIUS, -garea_y ), 
// 		    GOAL_POST_RADIUS, Color.YELLOW, true );
// 	drawCircle( new VecPosition( -hlen + GOAL_POST_RADIUS, garea_y ), 
// 		    GOAL_POST_RADIUS, Color.YELLOW, true );
// 	drawCircle( new VecPosition( -hlen + GOAL_POST_RADIUS, -garea_y ), 
// 		    GOAL_POST_RADIUS, Color.YELLOW, true );
    }


    public void drawShapes( Vector shapes )
    {
	Iterator i = shapes.iterator();
	while ( i.hasNext() ) {
	    FieldShape fs = ( (FieldShape) i.next() );
	    fs.draw( this );
	}
    }

    public Color getColorFromHex( String hex )
    {
	return new Color( Integer.decode( "0x"+hex ).intValue() );
    }
    
    public void drawRect( VecPosition fieldPos1, VecPosition fieldPos2, 
			  Color color, boolean filled )
    {
	// Get canvas coordinates of first point
	VecPosition canvasPos1 = fieldToCanvas( fieldPos1 );
	int x1 = (int) Math.round( canvasPos1.getX() );
	int y1 = (int) Math.round( canvasPos1.getY() );

	// Get canvas coordinates of second point
	VecPosition canvasPos2 = fieldToCanvas( fieldPos2 );
	int x2 = (int) Math.round( canvasPos2.getX() );
	int y2 = (int) Math.round( canvasPos2.getY() );

	int x = Math.min( x1, x2 );
	int y = Math.min( y1, y2 );
	int w = Math.abs( x1 - x2 );
	int h = Math.abs( y1 - y2 );

	offscreenBuffer.setPaint( color );
	if ( filled ) {
	    offscreenBuffer.fillRect( x, y, w, h );
	}
	else {
	    offscreenBuffer.drawRect( x, y, w, h );
	}
    }

    public void drawText( VecPosition fieldPos, String text,
			  Color color )
    {
	VecPosition canvasPos = fieldToCanvas( fieldPos );
        int x = (int) Math.round( canvasPos.getX() );
        int y = (int) Math.round( canvasPos.getY() );
	offscreenBuffer.setFont( new Font( "Arial", Font.BOLD, 16 ) );
	offscreenBuffer.setPaint( color );
	offscreenBuffer.drawString( text, x, y );
    }

    public void drawLine( VecPosition fieldPos1, 
			  VecPosition fieldPos2, Color color )
    {
	// Get canvas coordinates of first point
	VecPosition canvasPos1 = fieldToCanvas( fieldPos1 );
	int x1 = (int) Math.round( canvasPos1.getX() );
	int y1 = (int) Math.round( canvasPos1.getY() );

	// Get canvas coordinates of second point
	VecPosition canvasPos2 = fieldToCanvas( fieldPos2 );
	int x2 = (int) Math.round( canvasPos2.getX() );
	int y2 = (int) Math.round( canvasPos2.getY() );

	offscreenBuffer.setPaint( color );
	offscreenBuffer.drawLine( x1, y1, x2, y2 );
    }

    public void drawArc( VecPosition fieldPos, double radius,
			 int startAng, int arcAng, Color color )
    {
	VecPosition canvasPos = fieldToCanvas( fieldPos );
	int x = (int) Math.round( canvasPos.getX() );
	int y = (int) Math.round( canvasPos.getY() );
	int r = (int) Math.round( fieldToCanvas( radius ) );

	offscreenBuffer.setPaint( color );
	offscreenBuffer.drawArc( x - r, y - r, 2 * r, 2 * r,
				 startAng, arcAng );
    }
	    
    public void drawCircle( VecPosition fieldPos, double radius,
			    Color color, boolean filled )
    {
	// Get player's canvas coordinates
	VecPosition canvasPos = fieldToCanvas( fieldPos );
	int x = (int) Math.round( canvasPos.getX() );
	int y = (int) Math.round( canvasPos.getY() );
	int r = (int) Math.round( fieldToCanvas( radius ) );

	offscreenBuffer.setPaint( color );
	if ( filled ) {
	    offscreenBuffer.fillOval( x - r, y - r,
				      2 * r, 2 * r );
	}
	else {
	    offscreenBuffer.drawOval( x - r, y - r,
				      2 * r, 2 * r );
	}
    }

    public void paint( Graphics g ) 
    {
	if ( offscreenImage != null )
	    g.drawImage( offscreenImage, 0, 0, this );
    }

    private VecPosition canvasToField( VecPosition canvasPos )
    {
	double x = canvasPos.getX() * TOTAL_X / canvasSize.width - TOTAL_X / 2;
	double y = canvasPos.getY() * TOTAL_Y / canvasSize.height - TOTAL_Y / 2;
	return new VecPosition( x, y );
    }

    private VecPosition fieldToCanvas( VecPosition fieldPos )
    {
	double x = fieldPos.getX() * canvasSize.width / TOTAL_X + canvasSize.width / 2;
	double y = fieldPos.getY() * canvasSize.height / TOTAL_Y + canvasSize.height / 2;
	return new VecPosition( x, y );
    }

    private double canvasToField( double canvasDistance )
    {
	return canvasDistance * TOTAL_X / canvasSize.width;
    }

    private double fieldToCanvas( double fieldDistance )
    {
	return fieldDistance * canvasSize.width / TOTAL_X;
    }

}
