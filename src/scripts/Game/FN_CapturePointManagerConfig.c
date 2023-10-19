[BaseContainerProps(configRoot: true)]
class FN_CapturePointManagerConfig
{
	[Attribute()]
	protected int m_iCapturePointTickTimer;

	[Attribute()]
	protected int m_iPerCharacterPointGain;

	protected bool m_bIsManagerInitialized;

	protected ref array<FN_CapturePointEntity> m_aCapturePoints = new array<FN_CapturePointEntity>();

	//------------------------------------------------------------------------------------------------
	int GetPerCharacterPointGain()
	{
		return m_iPerCharacterPointGain;
	}

	//------------------------------------------------------------------------------------------------
	void AddCapturePoint(notnull FN_CapturePointEntity capturePoint)
	{
		RplComponent rpl = RplComponent.Cast(capturePoint.FindComponent(RplComponent));
		if (!rpl || rpl.IsProxy())
			return;

		if (!m_bIsManagerInitialized)
			InitializeManager();

		if (m_aCapturePoints.Contains(capturePoint))
		{
			Print("[FrontlineNexus] Double Capture Point Addition Detected! " + this, LogLevel.ERROR);
			return;
		}

		m_aCapturePoints.Insert(capturePoint);
	}

	//------------------------------------------------------------------------------------------------
	protected void CapturePointTick()
	{
		foreach (FN_CapturePointEntity capturePoint : m_aCapturePoints)
		{
			capturePoint.OnCapturePointTimerTick();
		}
	}

	//------------------------------------------------------------------------------------------------
	protected void InitializeManager()
	{
		m_bIsManagerInitialized = true;

		// For optimization reasons.
		if (m_iCapturePointTickTimer <= 1)
			Print("[FrontlineNexus] Capture Point Tick Timer must be higher than 1!", LogLevel.ERROR);

		GetGame().GetCallqueue().CallLater(CapturePointTick, m_iCapturePointTickTimer * 1000, true);
	}
}
