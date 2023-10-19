[EntityEditorProps(category: "GameScripted/FrontlineNexus", description: "")]
class FN_CapturePointEntityClass : BaseGameTriggerEntityClass
{
}

class FN_CapturePointEntity : BaseGameTriggerEntity
{
	protected ref array<SCR_ChimeraCharacter> m_aCharactersInsideZone = new array<SCR_ChimeraCharacter>();

	[Attribute()]
	protected ref FN_CapturePointManagerConfig m_CapturePointConfig;

	[Attribute()]
	protected int m_iCapturePointCap;

	protected ref array<ref FN_CaptureState> m_aCaptureStates = new array<ref FN_CaptureState>();

	//------------------------------------------------------------------------------------------------
	FN_CaptureState GetCaptureStateByFaction(FactionKey faction)
	{
		foreach (FN_CaptureState captureState : m_aCaptureStates)
		{
			if (captureState.GetFaction().GetFactionKey() == faction)
				return captureState;
		}
		
		return null;
	}

	//------------------------------------------------------------------------------------------------
	protected void CreateCaptureStateIfNotExistent(FactionKey factionKey)
	{
		if (factionKey == string.Empty)
			return;

		FN_CaptureState captureState = GetCaptureStateByFaction(factionKey);
		if (captureState)
			return;

		Faction faction = GetGame().GetFactionManager().GetFactionByKey(factionKey);
		if (!faction)
			return;

		captureState = new FN_CaptureState(faction);
		m_aCaptureStates.Insert(captureState);
	}

	//------------------------------------------------------------------------------------------------
	protected void OnCharacterEnteredZone(notnull SCR_ChimeraCharacter enteredCharacter)
	{
		m_aCharactersInsideZone.Insert(enteredCharacter);

		CreateCaptureStateIfNotExistent(enteredCharacter.GetFactionKey());
	}

	//------------------------------------------------------------------------------------------------
	protected void OnCharacterLeaveZone(notnull SCR_ChimeraCharacter leftCharacter)
	{
		m_aCharactersInsideZone.RemoveItem(leftCharacter);
	}

	//------------------------------------------------------------------------------------------------
	void OnCapturePointTimerTick()
	{
		int perCharacterPointGain = m_CapturePointConfig.GetPerCharacterPointGain();

		foreach (SCR_ChimeraCharacter character : m_aCharactersInsideZone)
		{
			if (!character)
				continue;

			string factionKey = character.GetFactionKey();
			if (factionKey == string.Empty)
				continue;

			FN_CaptureState captureState = GetCaptureStateByFaction(factionKey);
			if (!captureState)
				continue;

			captureState.AddCapturePoints(perCharacterPointGain);
		}
	}

	//------------------------------------------------------------------------------------------------
	override protected void OnActivate(IEntity ent)
	{
		SCR_ChimeraCharacter enteredCharacter = SCR_ChimeraCharacter.Cast(ent);
		if (!enteredCharacter)
		{
			Print("[FrontlineNexus] Non Character Entity Detected On Capture Zone! Check Trigger Filters! " + this, LogLevel.ERROR);
			return;
		}
		
		if (m_aCharactersInsideZone.Contains(enteredCharacter))
			return;

		OnCharacterEnteredZone(enteredCharacter);
	}

	//------------------------------------------------------------------------------------------------
	override protected void OnDeactivate(IEntity ent)
	{
		SCR_ChimeraCharacter leftCharacter = SCR_ChimeraCharacter.Cast(ent);
		if (!leftCharacter)
		{
			Print("[FrontlineNexus] Non Character Entity Detected On Capture Zone! Check Trigger Filters! " + this, LogLevel.ERROR);
			return;
		}
		
		if (!m_aCharactersInsideZone.Contains(leftCharacter))
			return;

		OnCharacterLeaveZone(leftCharacter);
	}

	//------------------------------------------------------------------------------------------------
	override protected void EOnInit(IEntity owner)
	{
		if (!m_CapturePointConfig)
		{
			Print("[FrontlineNexus] NULL Capture Point Config Detected! " + this, LogLevel.ERROR);
			return;
		}
		
		RplComponent rpl = RplComponent.Cast(FindComponent(RplComponent));
		if (!rpl)
			Print("[FrontlineNexus] Capture Point Entity is missing a Replication Component! " + this, LogLevel.ERROR);

		m_CapturePointConfig.AddCapturePoint(this);
	}
}