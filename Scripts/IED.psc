Scriptname IED Hidden

Int Function GetScriptVersion() global
	return 2
EndFunction

Int Function GetVersion() native global

Function AddActorBlock(Actor akActor, string asPlugin) native global
Function RemoveActorBlock(Actor akActor, string asPlugin) native global

Function Evaluate(Actor akActor) native global
Function EvaluateAll() native global

Function Reset(Actor akActor) native global
Function ResetAll() native global


;; Custom item control functions
;
;  asPlugin = your plugin name
;
;  asPlugin must be loaded or the call will fail
;  Items registered to plugins removed mid-save are automatically purged

Bool Function CreateItemActor(Actor akActor, string asPlugin, string asName, bool abIsFemale, Form akForm, bool abIsInventoryForm, string asNode) native global
Bool Function CreateItemNPC(ActorBase akActorBase, string asPlugin, string asName, bool abIsFemale, Form akForm, bool abIsInventoryForm, string asNode) native global
Bool Function CreateItemRace(Race akRace, string asPlugin, string asName, bool abIsFemale, Form akForm, bool abIsInventoryForm, string asNode) native global

Bool Function DeleteItemActor(Actor akActor, string asPlugin, string asName) native global
Bool Function DeleteItemNPC(ActorBase akActorBase, string asPlugin, string asName) native global
Bool Function DeleteItemRace(Race akRace, string asPlugin, string asName) native global

Bool Function DeleteAllActor(Actor akActor, string asPlugin) native global
Bool Function DeleteAllNPC(ActorBase akActorBase, string asPlugin) native global
Bool Function DeleteAllRace(Race akRace, string asPlugin) native global

Bool Function DeleteAll(string asPlugin) native global

Bool Function SetItemEnabledActor(Actor akActor, string asPlugin, string asName, bool abIsFemale, bool abSwitch) native global
Bool Function SetItemEnabledNPC(ActorBase akActorBase, string asPlugin, string asName, bool abIsFemale, bool abSwitch) native global
Bool Function SetItemEnabledRace(Race akRace, string asPlugin, string asName, bool abIsFemale, bool abSwitch) native global

Bool Function SetItemFormActor(Actor akActor, string asPlugin, string asName, bool abIsFemale, Form akForm) native global
Bool Function SetItemFormNPC(ActorBase akActorBase, string asPlugin, string asName, bool abIsFemale, Form akForm) native global
Bool Function SetItemFormRace(Race akRace, string asPlugin, string asName, bool abIsFemale, Form akForm) native global

; Only takes effect if the entry is flagged as an inventory item
Bool Function SetItemModelSwapFormActor(Actor akActor, string asPlugin, string asName, bool abIsFemale, Form akForm) native global
Bool Function SetItemModelSwapFormNPC(ActorBase akActorBase, string asPlugin, string asName, bool abIsFemale, Form akForm) native global
Bool Function SetItemModelSwapFormRace(Race akRace, string asPlugin, string asName, bool abIsFemale, Form akForm) native global

Bool Function ClearItemModelSwapFormActor(Actor akActor, string asPlugin, string asName, bool abIsFemale) native global
Bool Function ClearItemModelSwapFormNPC(ActorBase akActorBase, string asPlugin, string asName, bool abIsFemale) native global
Bool Function ClearItemModelSwapFormRace(Race akRace, string asPlugin, string asName, bool abIsFemale) native global

; Marks this as an inventory item, 3D won't load unless the actor carries it
Bool Function SetItemInventoryActor(Actor akActor, string asPlugin, string asName, bool abIsFemale, bool abIsInventoryForm) native global
Bool Function SetItemInventoryNPC(ActorBase akActorBase, string asPlugin, string asName, bool abIsFemale, bool abIsInventoryForm) native global
Bool Function SetItemInventoryRace(Race akRace, string asPlugin, string asName, bool abIsFemale, bool abIsInventoryForm) native global

Bool Function SetItemEquipmentModeActor(Actor akActor, string asPlugin, string asName, bool abIsFemale, bool abEquipmentMode, bool abIgnoreRaceEquipTypes, bool abDisableIfEquipped) native global
Bool Function SetItemEquipmentModeNPC(ActorBase akActorBase, string asPlugin, string asName, bool abIsFemale, bool abEquipmentMode, bool abIgnoreRaceEquipTypes, bool abDisableIfEquipped) native global
Bool Function SetItemEquipmentModeRace(Race akRace, string asPlugin, string asName, bool abIsFemale, bool abEquipmentMode, bool abIgnoreRaceEquipTypes, bool abDisableIfEquipped) native global

Bool Function SetItemLeftWeaponActor(Actor akActor, string asPlugin, string asName, bool abIsFemale, bool abIsLeftWeapon) native global
Bool Function SetItemLeftWeaponNPC(ActorBase akActorBase, string asPlugin, string asName, bool abIsFemale, bool abIsLeftWeapon) native global
Bool Function SetItemLeftWeaponRace(Race akRace, string asPlugin, string asName, bool abIsFemale, bool abIsLeftWeapon) native global

Bool Function SetItemLoadChanceActor(Actor akActor, string asPlugin, string asName, bool abIsFemale, bool abEnable, float afPercentChance) native global
Bool Function SetItemLoadChanceNPC(ActorBase akActorBase, string asPlugin, string asName, bool abIsFemale, bool abEnable, float afPercentChance) native global
Bool Function SetItemLoadChanceRace(Race akRace, string asPlugin, string asName, bool abIsFemale, bool abEnable, float afPercentChance) native global

Bool Function SetItemCountRangeActor(Actor akActor, string asPlugin, string asName, bool abIsFemale, int aiMin, int aiMax) native global
Bool Function SetItemCountRangeNPC(ActorBase akActorBase, string asPlugin, string asName, bool abIsFemale, int aiMin, int aiMax) native global
Bool Function SetItemCountRangeRace(Race akRace, string asPlugin, string asName, bool abIsFemale, int aiMin, int aiMax) native global

; An attempt to set a managed node when in parent attachment mode will fail and return false
Bool Function SetItemNodeActor(Actor akActor, string asPlugin, string asName, bool abIsFemale, string asNode) native global
Bool Function SetItemNodeNPC(ActorBase akActorBase, string asPlugin, string asName, bool abIsFemale, string asNode) native global
Bool Function SetItemNodeRace(Race akRace, string asPlugin, string asName, bool abIsFemale, string asNode) native global

; aiAttachmentMode: 0 = reference, 1 = parent
;
; An attempt to set parent mode when a managed node is also set will fail and return false
Bool Function SetItemAttachmentModeActor(Actor akActor, string asPlugin, string asName, bool abIsFemale, int aiAttachmentMode, bool abSyncReference) native global
Bool Function SetItemAttachmentModeNPC(ActorBase akActorBase, string asPlugin, string asName, bool abIsFemale, int aiAttachmentMode, bool abSyncReference) native global
Bool Function SetItemAttachmentModeRace(Race akRace, string asPlugin, string asName, bool abIsFemale, int aiAttachmentMode, bool abSyncReference) native global

; arPosition: 3 element float array (X, Y, Z), clamped to +-5000
Bool Function SetItemPositionActor(Actor akActor, string asPlugin, string asName, bool abIsFemale, float[] arPosition) native global
Bool Function SetItemPositionNPC(ActorBase akActorBase, string asPlugin, string asName, bool abIsFemale, float[] arPosition) native global
Bool Function SetItemPositionRace(Race akRace, string asPlugin, string asName, bool abIsFemale, float[] arPosition) native global

; arRotation: 3 element float array (Pitch, Roll, Yaw) in degrees, clamped to +-360
Bool Function SetItemRotationActor(Actor akActor, string asPlugin, string asName, bool abIsFemale, float[] arRotation) native global
Bool Function SetItemRotationNPC(ActorBase akActorBase, string asPlugin, string asName, bool abIsFemale, float[] arRotation) native global
Bool Function SetItemRotationRace(Race akRace, string asPlugin, string asName, bool abIsFemale, float[] arRotation) native global

; clamped to 0.01 - 100
Bool Function SetItemScaleActor(Actor akActor, string asPlugin, string asName, bool abIsFemale, float afScale) native global
Bool Function SetItemScaleNPC(ActorBase akActorBase, string asPlugin, string asName, bool abIsFemale, float afScale) native global
Bool Function SetItemScaleRace(Race akRace, string asPlugin, string asName, bool abIsFemale, float afScale) native global

Bool Function ClearItemPositionActor(Actor akActor, string asPlugin, string asName, bool abIsFemale) native global
Bool Function ClearItemPositionNPC(ActorBase akActorBase, string asPlugin, string asName, bool abIsFemale) native global
Bool Function ClearItemPositionRace(Race akRace, string asPlugin, string asName, bool abIsFemale) native global

Bool Function ClearItemRotationActor(Actor akActor, string asPlugin, string asName, bool abIsFemale) native global
Bool Function ClearItemRotationNPC(ActorBase akActorBase, string asPlugin, string asName, bool abIsFemale) native global
Bool Function ClearItemRotationRace(Race akRace, string asPlugin, string asName, bool abIsFemale) native global

Bool Function ClearItemScaleActor(Actor akActor, string asPlugin, string asName, bool abIsFemale) native global
Bool Function ClearItemScaleNPC(ActorBase akActorBase, string asPlugin, string asName, bool abIsFemale) native global
Bool Function ClearItemScaleRace(Race akRace, string asPlugin, string asName, bool abIsFemale) native global

Bool Function ItemExistsActor(Actor akActor, string asPlugin, string asName) native global
Bool Function ItemExistsNPC(ActorBase akActorBase, string asPlugin, string asName) native global
Bool Function ItemExistsRace(Race akRace, string asPlugin, string asName) native global

Bool Function ItemEnabledActor(Actor akActor, string asPlugin, string asName, bool abIsFemale) native global
Bool Function ItemEnabledNPC(ActorBase akActorBase, string asPlugin, string asName, bool abIsFemale) native global
Bool Function ItemEnabledRace(Race akRace, string asPlugin, string asName, bool abIsFemale) native global
