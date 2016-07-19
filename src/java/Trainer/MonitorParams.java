import java.awt.Color;
import java.lang.reflect.Field;

public class MonitorParams
{
    public String title = "JARCS Monitor";
    public int window_size_x = 600;
    public int window_size_y = 450;
    public int menu_size_y = 20;

    public double plane_origin_x = 0;
    public double plane_origin_y = 0;
    public double plane_size_x = 112;
    public double plane_size_y = 85;
    public double penalty_spot_radius = 0.15;
    public boolean show_penalty_spot = true;
    public boolean show_ball = true;
    public boolean show_players = true;
    public boolean show_sideline_players = true;
    public boolean show_center_circle = true;
    public boolean show_middle_line = true;
    public double corner_arc_radius = 1;
    public double ball_radius = 0.3;

    public Color color_ball = Color.white;
    public Color color_team_l = Color.yellow;
    public Color color_team_r = Color.cyan;
    public Color color_unum_l = new Color( 0.5f, 0f, 0.5f );
    public Color color_unum_r = Color.pink;
    public Color color_field = new Color( 0f, 0.6f, 0f );
    public Color color_player_outline = Color.black;
    public Color color_body_ang = Color.black;
    public Color color_neck_ang = Color.red;

    public int depth_ball = 60;
    public int depth_player_body = 20;
    public int depth_player_decorations = 40;
    public int depth_player_outline = 30;
    public int depth_player_unum = 50;

    public boolean setParam( String param, String value )
    {
	try {
	    Field field = getClass().getField( param );
	    Class type = field.getType();
	    if ( type.equals( double.class ) ) {
		field.setDouble( this, Double.parseDouble( value ) );
	    }
	    else if ( type.equals( int.class ) ) {
		field.setInt( this, Integer.parseInt( value ) );
	    }
	    else if ( type.equals( String.class ) ) {
		field.set( this, value.substring( 1, value.length() - 1 ) );
	    }
	    else {
		System.err.println( "Unsupported field type: " + type );
		return false;
	    }
	}
	catch ( Exception e ) {
	    System.err.println( e );
	    return false;
	}
	return true;
    }

    public Object getParam( String param )
    {
	try {
	    Field field = getClass().getField( param );
	    return field.get( this );
	}
	catch ( Exception e ) {
	    System.err.println( e );
	    return null;
	}
    }
}
