import java.io.Serializable;

public class BallObject extends DynamicObject implements Serializable
{
    public String toString()
    {
	return "(ball pos:" + getPosition() +
	    " vel: " + getVelocity() + ")";
    }
}
