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

	[Attribute()]
	protected ref array<ref FN_CapturePointHandler> m_aHandlers;

	protected Faction m_HoldingFaction;

	//------------------------------------------------------------------------------------------------
	int GetCapturePointCap()
	{
		return m_iCapturePointCap;
	}

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
	Faction GetHoldingFaction()
	{
		return m_HoldingFaction;
	}

	//------------------------------------------------------------------------------------------------
	Faction GetFactionWithHighestCharactersInside()
	{
		Faction highestCharacterFaction;
		int characterCountOfFaction;

		foreach (FN_CaptureState captureState : m_aCaptureStates)
		{
			Faction captureStateFaction = captureState.GetFaction();

			int amountOfCharacters = GetAmountOfCharactersInsideBasedOnFaction(captureStateFaction.GetFactionKey());
			if (amountOfCharacters > characterCountOfFaction)
			{
				highestCharacterFaction = captureStateFaction;
				characterCountOfFaction = amountOfCharacters;
			}
			else if (amountOfCharacters == characterCountOfFaction) // If both factions have same amount of characters, it's considered even.
				highestCharacterFaction = null;
		}

		return highestCharacterFaction;
	}

	//------------------------------------------------------------------------------------------------
	int GetAmountOfCharactersInsideBasedOnFaction(FactionKey factionKey)
	{
		if (SCR_StringHelper.IsEmptyOrWhiteSpace(factionKey))
			return 0;

		int amountOfCharacters;
		foreach (SCR_ChimeraCharacter character : m_aCharactersInsideZone)
		{
			if (!character)
				continue;

			if (character.GetFactionKey() == factionKey)
				amountOfCharacters++;
		}

		return amountOfCharacters;
	}

	//------------------------------------------------------------------------------------------------
	void SetHoldingFaction(Faction holdingFaction)
	{
		m_HoldingFaction = holdingFaction;
		Print("New Faction: " + holdingFaction);
	}

	//------------------------------------------------------------------------------------------------
	protected void CreateCaptureStateIfNotExistent(FactionKey factionKey)
	{
		if (SCR_StringHelper.IsEmptyOrWhiteSpace(factionKey))
			return;

		FN_CaptureState captureState = GetCaptureStateByFaction(factionKey);
		if (captureState)
			return;

		Faction faction = GetGame().GetFactionManager().GetFactionByKey(factionKey);
		if (!faction)
			return;

		captureState = new FN_CaptureState(faction);
		m_aCaptureStates.Insert(captureState);

		foreach (FN_CapturePointHandler handler : m_aHandlers)
		{
			handler.OnNewCaptureStateCreated(captureState);
		}
	}

	//------------------------------------------------------------------------------------------------
	void OnCharacterDeadWithinZone(SCR_CharacterControllerComponent characterController, IEntity killerEntity, notnull Instigator killer)
	{
		SCR_ChimeraCharacter character = characterController.GetCharacter();
		if (!character)
			return;

		m_aCharactersInsideZone.RemoveItem(characterController.GetCharacter());

		foreach (FN_CapturePointHandler handler : m_aHandlers)
		{
			handler.OnCharacterDeathInsideCaptureZone(character);
		}
	}

	//------------------------------------------------------------------------------------------------
	protected void OnCharacterEnteredZone(notnull SCR_ChimeraCharacter enteredCharacter)
	{
		SCR_CharacterControllerComponent characterController = SCR_CharacterControllerComponent.Cast(enteredCharacter.GetCharacterController());
		if (!characterController)
			return;

		characterController.GetOnPlayerDeathWithParam().Insert(OnCharacterDeadWithinZone);
		CreateCaptureStateIfNotExistent(enteredCharacter.GetFactionKey());
		m_aCharactersInsideZone.Insert(enteredCharacter);

		foreach (FN_CapturePointHandler handler : m_aHandlers)
		{
			handler.OnCharacterEnterCaptureZone(enteredCharacter);
		}
	}

	//------------------------------------------------------------------------------------------------
	protected void OnCharacterLeaveZone(notnull SCR_ChimeraCharacter leftCharacter)
	{
		m_aCharactersInsideZone.RemoveItem(leftCharacter);

		SCR_CharacterControllerComponent characterController = SCR_CharacterControllerComponent.Cast(leftCharacter.GetCharacterController());
		if (!characterController)
			return;

		characterController.GetOnPlayerDeathWithParam().Remove(OnCharacterDeadWithinZone);

		foreach (FN_CapturePointHandler handler : m_aHandlers)
		{
			handler.OnCharacterLeaveCaptureZone(leftCharacter);
		}
	}

	protected void OptimizeMemoryUtilization()
	{
		for (int i = m_aCharactersInsideZone.Count() - 1; i >= 0; i--)
		{
			SCR_ChimeraCharacter character = m_aCharactersInsideZone[i];
			if (!character) // If the character was deleted in between the previous tick, remove it from the array.
			{
				m_aCharactersInsideZone.RemoveOrdered(i);
				continue;
			}
		}

		m_aCharactersInsideZone.Compact();
	}

	//------------------------------------------------------------------------------------------------
	void OnCapturePointTimerTick()
	{
		OptimizeMemoryUtilization();

		foreach (FN_CapturePointHandler handler : m_aHandlers)
		{
			handler.OnCaptureTimerTick();
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

		foreach (FN_CapturePointHandler handler : m_aHandlers)
		{
			handler.Initialize(this);
		}
	}
}
