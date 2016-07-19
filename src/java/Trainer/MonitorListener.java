import java.util.*;
import java.awt.event.*;

public class MonitorListener implements ActionListener,
					ItemListener,
					MouseListener,
					MouseMotionListener,
					WindowStateListener
{
    private Monitor monitor;

    public MonitorListener( Monitor monitor )
    {
	this.monitor = monitor;
    }

    public void windowStateChanged( WindowEvent e )
    {
	monitor.repaint();
    }

    public void actionPerformed( ActionEvent e ) 
    {
	String cmd = e.getActionCommand();

	if ( cmd.equals( "Quit" ) ) {
	    monitor.quit();
	}
    }

    public void itemStateChanged( ItemEvent e ) 
    {
    }


    public void mouseClicked( MouseEvent e ) 
    {
	int x = e.getX();
	int y = e.getY();
    }

    public void mouseEntered( MouseEvent e ) 
    {
    }

    public void mouseExited( MouseEvent e ) 
    {
    }

    public void mousePressed( MouseEvent e ) 
    {
	if ( e.getButton() == MouseEvent.BUTTON3 ) {

	    int x = e.getX();
	    int y = e.getY();
	}
    }

    public void mouseReleased( MouseEvent e ) 
    {
	int x = e.getX();
	int y = e.getY();
    }

    public void mouseDragged( MouseEvent e ) 
    {
	    int x = e.getX();
	    int y = e.getY();
    }

    public void mouseMoved( MouseEvent e ) 
    {
	int x = e.getX();
	int y = e.getY();
    }
}
