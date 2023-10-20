[BaseContainerProps()]
class FN_CPHandler_WinnerPointReducer : FN_CapturePointHandler
{
	//------------------------------------------------------------------------------------------------
	override void OnCaptureTimerTick()
	{
		Faction winningFaction = m_CapturePoint.GetFactionWithHighestCharactersInside();
		Faction holdingFaction = m_CapturePoint.GetHoldingFaction();
		if (!winningFaction || !holdingFaction)
			return;

		if (winningFaction == holdingFaction)
			return;

		FN_CaptureState holdingFactionCaptureState = m_CapturePoint.GetCaptureStateByFaction(holdingFaction.GetFactionKey());
		if (!holdingFactionCaptureState)
			return;

		if (holdingFactionCaptureState.GetCapturePoints() > 0)
			holdingFactionCaptureState.RemoveCapturePoints(1); // Magic number! Lets improve this later on by making it data driven.
	}
}
