public abstract class FieldShape implements Comparable
{
    protected int depth;

    public int compareTo( Object obj )
    {
	return depth - ( (FieldShape) obj ).depth;
    }

    public abstract void draw( FieldCanvas fc );
} 
