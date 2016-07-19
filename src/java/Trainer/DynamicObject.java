import java.io.Serializable;

public abstract class DynamicObject implements Serializable
{
    protected VecPosition pos;
    protected VecPosition vel;
    protected int timeLastSeen;
    
    public DynamicObject()
    {
	pos = new VecPosition();
	vel = new VecPosition();
	timeLastSeen = -1;
    }

    public void copy( DynamicObject obj )
    {
	setPosition( obj.pos );
	setVelocity( obj.vel );
	setTimeLastSeen( obj.timeLastSeen );
    }

    public void setPosition( VecPosition pos )
    {
	this.pos.copy( pos );
    }

    public VecPosition getPosition()
    {
	return new VecPosition( pos );
    }

    public void setVelocity( VecPosition vel )
    {
	this.vel.copy( vel );
    }

    public VecPosition getVelocity()
    {
	return new VecPosition( vel );
    }

    public int getTimeLastSeen()
    {
	return timeLastSeen;
    }

    public void setTimeLastSeen( int time )
    {
	timeLastSeen = time;
    }
}
