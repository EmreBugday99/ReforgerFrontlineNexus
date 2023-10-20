void OnCaptureStateChangeMethod(FN_CaptureState captureState);
typedef func OnCaptureStateChangeMethod;
typedef ScriptInvokerBase<OnCaptureStateChangeMethod> OnCaptureStateChange;

class FN_CaptureState
{
	protected Faction m_CapturingFaction; // Must not be null at all times!
	protected int m_iCapturePoints;
	protected ref OnCaptureStateChange m_OnCapturePointChange;

	//------------------------------------------------------------------------------------------------
	Faction GetFaction()
	{
		return m_CapturingFaction;
	}

	//------------------------------------------------------------------------------------------------
	int GetCapturePoints()
	{
		return m_iCapturePoints;
	}

	//------------------------------------------------------------------------------------------------
	OnCaptureStateChange GetOnCaptureStateChange()
	{
		if (!m_OnCapturePointChange)
			m_OnCapturePointChange = new OnCaptureStateChange();

		return m_OnCapturePointChange;
	}

	//------------------------------------------------------------------------------------------------
	void AddCapturePoints(int amount)
	{
		if (amount <= 0)
		{
			Print("[FrontlineNexus] Attempting to add a negative value as Capture Point! " + this, LogLevel.ERROR);
			return;
		}

		m_iCapturePoints += amount;

		if (m_OnCapturePointChange)
			m_OnCapturePointChange.Invoke(this);

		Print("New Capture Point For " + GetFaction().GetFactionName() + ": " + GetCapturePoints());
	}

	//------------------------------------------------------------------------------------------------
	void RemoveCapturePoints(int amount)
	{
		if (amount <= 0)
		{
			Print("[FrontlineNexus] Attempting to add a negative value as Capture Point! " + this, LogLevel.ERROR);
			return;
		}

		m_iCapturePoints -= amount;

		if (m_OnCapturePointChange)
			m_OnCapturePointChange.Invoke(this);
		
		Print("New Capture Point For " + GetFaction().GetFactionName() + ": " + GetCapturePoints());
	}

	//------------------------------------------------------------------------------------------------
	void SetCapturePoints(int amount)
	{
		m_iCapturePoints = amount;

		if (m_OnCapturePointChange)
			m_OnCapturePointChange.Invoke(this);

		Print("New Capture Point For " + GetFaction().GetFactionName() + ": " + GetCapturePoints());
	}

	//------------------------------------------------------------------------------------------------
	void FN_CaptureState(notnull Faction capturingFaction)
	{
		m_CapturingFaction = capturingFaction;
	}
}