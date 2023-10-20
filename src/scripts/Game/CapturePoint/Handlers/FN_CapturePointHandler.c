[BaseContainerProps()]
class FN_CapturePointHandler
{
	protected FN_CapturePointEntity m_CapturePoint;

	void OnCaptureTimerTick();
	void OnCharacterEnterCaptureZone(SCR_ChimeraCharacter enteredCharacter);
	void OnCharacterLeaveCaptureZone(SCR_ChimeraCharacter leftCharacter);
	void OnCharacterDeathInsideCaptureZone(SCR_ChimeraCharacter deadCharacter);
	void OnNewCaptureStateCreated(FN_CaptureState newCaptureState);
	protected void OnInitialize(FN_CapturePointEntity owningCapturePoint);

	void Initialize(notnull FN_CapturePointEntity owningCapturePoint)
	{
		m_CapturePoint = owningCapturePoint;
		OnInitialize(m_CapturePoint);
	}
}
