import java.io.Serializable;

public class PlayerObject extends DynamicObject implements Serializable
{
    private boolean isGoalie;
    private int heteroPlayerType;
    private double angBody;
    private double angNeck;

    public void copy( PlayerObject obj )
    {
	super.copy( obj );
	setIsGoalie( obj.getIsGoalie() );
	setHeteroPlayerType( obj.getHeteroPlayerType() );
	setBodyAngle( obj.getBodyAngle() );
	setNeckAngle( obj.getNeckAngle() );
    }

    public void setIsGoalie( boolean isGoalie )
    {
	this.isGoalie = isGoalie;
    }

    public boolean getIsGoalie()
    {
	return isGoalie;
    }

    public void setHeteroPlayerType( int index )
    {
	heteroPlayerType = index;
    }

    public int getHeteroPlayerType()
    {
	return heteroPlayerType;
    }

    public void setBodyAngle( double angBody )
    {
	this.angBody = angBody;
    }

    public double getBodyAngle()
    {
	return angBody;
    }

    public void setNeckAngle( double angNeck )
    {
	this.angNeck = angNeck;
    }

    public double getNeckAngle()
    {
	return angNeck;
    }
}
