#ifndef POLICY_AGENT
#define POLICY_AGENT
 
#define MAX_STATE_VARS         64
#define MAX_ACTIONS            10

class PolicyAgent
{
 
 private:
  
  int m_numFeatures; /* number of state features <= MAX_STATE_VARS */
  int m_numActions;  /* number of possible actions <= MAX_ACTIONS */

 protected:

  int getNumFeatures() { return m_numFeatures; }
  int getNumActions()  { return m_numActions;  }
 
 public:
        int m_id;
	virtual int bestAction(double state[]) = 0;
	virtual int selectAction(double state[], double episodeTime) = 0;
	
	PolicyAgent(int numFeatures, int numActions, int id)
	{
		m_numFeatures = numFeatures;
		m_numActions = numActions;
                m_id = id;
	}
	
};

#endif

