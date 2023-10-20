[BaseContainerProps()]
class FN_CPHandler_WinnerPointIncreaser : FN_CapturePointHandler
{
	//------------------------------------------------------------------------------------------------
	override void OnCaptureTimerTick()
	{
		Faction winningFaction = m_CapturePoint.GetFactionWithHighestCharactersInside();
		Faction holdingFaction = m_CapturePoint.GetHoldingFaction();
		if (!winningFaction || holdingFaction) // If there is a holding faction, it first needs to be neutralized.
			return;

		FN_CaptureState winningCaptureState = m_CapturePoint.GetCaptureStateByFaction(winningFaction.GetFactionKey());
		if (!winningCaptureState)
			return;

		int capturePointCap = m_CapturePoint.GetCapturePointCap();
		int captureStatePoints = winningCaptureState.GetCapturePoints();

		if (captureStatePoints < capturePointCap)
			winningCaptureState.AddCapturePoints(1);
		else if (captureStatePoints == capturePointCap)
			m_CapturePoint.SetHoldingFaction(winningFaction);
	}
}