class Time implements SoccerTypes, Comparable
{
    private int m_time;
    private int m_stopped;

    public Time()
    {
	this( -1 );
    }

    public Time( int time )
    {
	this( time, 0 );
    }

    public Time( int time, int stopped )
    {
	m_time = time;
	m_stopped = stopped;
    }

    public void updateTime( int time )
    {
	if ( m_time == time ) {
	    m_stopped++;
	}
	else {
	    m_time = time;
	    m_stopped = 0;
	}
    }

    public void setTimeStopped( int stopped )
    {
	m_stopped = stopped;
    }

    public int getTime()
    {
	return m_time;
    }

    public int getTimeStopped()
    {
	return m_stopped;
    }

    public int getTimeDifference( Time t )
    {
	if ( getTime() < t.getTime() )
	    return getTime() - t.getTime() - t.getTimeStopped();
	else if( getTime() == t.getTime() )
	    return getTimeStopped() - t.getTimeStopped();
	else
	    return getTime() - t.getTime();
    }

    public boolean isStopped()
    {
	return m_stopped > 0;
    }

    public Time getTimeAddedWith( int cycles )
    {
	int time = getTime();
	int stopped = getTimeStopped();

	if ( cycles > 0 ) {
	    if ( stopped > 0 )
		stopped += cycles;
	    else
		time += cycles;   
	}
	else {                    
	    if ( stopped > 0 && stopped >= cycles )
		stopped += cycles;            
	    else if ( stopped > 0 ) {
		stopped = 0;        
		cycles += stopped;
		time += cycles;
	    }
	    else      
		time += cycles;
	    if ( time < 0 )
		time = 0; 
	}
	return new Time( time, stopped );
    }

    public void addToTime( int cycles )
    {
	Time t = getTimeAddedWith( cycles );
	m_time = t.getTime();
	m_stopped = t.getTimeStopped();
    }

    public String toString()
    {
	return "(" + getTime() + "," + getTimeStopped() + ")";
    }

    public Time add( int i )
    {
	return getTimeAddedWith( i );
    }

    public Time add( Time t )
    {
	return new Time( getTime() + t.getTime(), t.getTimeStopped() );
    }

    public Time subtract( int i )
    {
	return getTimeAddedWith( -i );
    }

    public int subtract( Time t )
    {
	return getTimeDifference( t );
    }
    
    public boolean equals( int i )
    {
	return equals( new Time( i, 0 ) );
    }

    public boolean equals( Object o )
    {
	return compareTo( o ) == 0;
    }

    public int compareTo( int i )
    {
	return compareTo( new Time( i, 0 ) );
    }

    public int compareTo( Object o )
    {
	if ( o instanceof Time ) {
	    Time t = (Time) o;
	    return getTimeDifference( t );
	}
	return UnknownInt;
    }
}
