import java.awt.Color;

public class FieldRect extends FieldShape
{
    private Color color;
    private boolean filled;
    private VecPosition topLeft;
    private VecPosition bottomRight;

    public FieldRect( Color color, boolean filled,
		      VecPosition topLeft, VecPosition bottomRight,
		      int depth )
    {
        this.color = color;
        this.filled = filled;
        this.topLeft = topLeft;
        this.bottomRight = bottomRight;
	this.depth = depth;
    }

    public void draw( FieldCanvas fc )
    {
	fc.drawRect( topLeft, bottomRight, color, filled );
    }
}
