[BaseContainerProps()]
class FN_CPHandler_NeutralizeHolder : FN_CapturePointHandler
{
	//------------------------------------------------------------------------------------------------
	override void OnCaptureTimerTick()
	{
		Faction holdingFaction = m_CapturePoint.GetHoldingFaction();
		if (!holdingFaction)
			return;

		FN_CaptureState holdingFactionCaptureState = m_CapturePoint.GetCaptureStateByFaction(holdingFaction.GetFactionKey());
		if (!holdingFactionCaptureState)
			return;

		if (holdingFactionCaptureState.GetCapturePoints() == 0)
			m_CapturePoint.SetHoldingFaction(null);
	}
}