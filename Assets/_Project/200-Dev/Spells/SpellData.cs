using System;
using System.Collections.Generic;
using System.Linq;
using _Project._200_Dev.Entities.Player.Stats;
using _Project._200_Dev.ParrelSync.IL_Weaving;
using _Project._200_Dev.Tools;
using Sirenix.OdinInspector;
using UnityEditor;
using UnityEngine;

namespace _Project._200_Dev.Spells
{
    public enum SpellInstantiationType
    {
        NetworkObject,
        ServerOnly,
        None
    }
    
    [CreateAssetMenu(fileName = "New SpellData", menuName = "Spells/Data/Default", order = -10)]
    public class SpellData : ScriptableObject, IScriptableObjectSerializeReference
    {        
        public const int CharacterSpellsCount = 4;

        // Ugly but working
        // Find a way to get rid of magic string
        [BoxGroup("Caster"), Required, SerializeField, 
         ValueDropdown("@ICastResult.AllResultTypesAsString"), OnValueChanged(nameof(CheckCasterValidity))]
        private string resultTypeSelection;
        
        [BoxGroup("Caster"), OnValueChanged(nameof(CheckCasterValidity))] 
        public SpellCaster requiredCaster;

        [BoxGroup("Caster")] public bool overrideCasterPreview;
        [BoxGroup("Caster"), ShowIf(nameof(overrideCasterPreview))] public GameObject overridePreviewPrefab;
        
        [BoxGroup("Spell"), OnValueChanged("UpdateHash")] public string spellId;
        [BoxGroup("Spell"), DisableIf("@true")] public int spellIdHash;
        [Space(15)]
        [BoxGroup("Spell")] public SpellInstantiationType instantiationType;
        [BoxGroup("Spell"), InlineEditor, ShowIf(nameof(IsInstantiated))] public Spell spellPrefab;
        [Space(15)]
        [BoxGroup("Spell"), Tooltip("If true, this spell won't have any aim phase")] public bool isInstant;
        [BoxGroup("Spell")] public float cooldown;
        [BoxGroup("Spell")] public float channelingTime;
        
        [Space(15)]
        
        [BoxGroup("Spell"), EnumToggleButtons] public CastingFlags castingFlags;
        [BoxGroup("Spell")] public float castAnimationDuration;

        [BoxGroup("Description")] public string spellName;
        [BoxGroup("Description")] public Sprite spellIcon;
        [BoxGroup("Description")] public SpellDescription spellDescription;

        [Space(40)]
        
        // Effects applied on a defined target when spell asks for it
        [SerializeReference, PropertyOrder(999), OnValueChanged(nameof(CopyEffects), InvokeOnInitialize = true)] public Effect[] effects;
        
        // Effects applied on caster when spell is cast
        [SerializeReference, PropertyOrder(1000)] public Effect[] onCasterEffects;

        public Type RequiredResultType
        {
            get
            {
                Debug.Log($"Result type selection : {resultTypeSelection}");
                return resultTypeSelection != null ? ICastResult.GetTypeFromName(resultTypeSelection) : null;
            }
        }

        void Awake()
        {
            UpdateHash();
        }

        private Effect[] CopyEffects() => spellDescription.effects = effects;
        
        private void UpdateHash()
        {
            if (spellId == null) return;
            
            spellIdHash = spellId.GetHashCode();
        }

        private static List<SpellData> _spellsCache;

        public static SpellData GetSpell(int spellIdHash)
        {
            if (_spellsCache == null || _spellsCache.Count == 0)
                PopulateSpellsCache();
            
            // _spellsCache can't be null here
            return _spellsCache!.FirstOrDefault(spell => spell.spellIdHash == spellIdHash);
        }
        
        private static void PopulateSpellsCache()
        {
            _spellsCache = new List<SpellData>();
                
            var spellDataTypes = AppDomain.CurrentDomain.GetAssemblies()
                .SelectMany(domainAssembly => domainAssembly.GetTypes())
                .Where(type => typeof(SpellData).IsAssignableFrom(type));

            foreach (var type in spellDataTypes)
            {
                var spells = SOScriptableObjectReferencesCache.GetScriptableObjects(type);

                _spellsCache.AddRange(spells.Cast<SpellData>());
            }
        }
        
        #if UNITY_EDITOR

        [PropertySpace, Button(ButtonSizes.Large, ButtonStyle.Box, ButtonHeight = 30, Name = "Force Link To Prefab")]
        private void ForceLinkToPrefab()
        {
            if (spellPrefab == null) return;
            
            GameObject contentsRoot = spellPrefab.gameObject;

            spellPrefab.Data = this;

            PrefabUtility.SavePrefabAsset(contentsRoot);
            
            Debug.Log($"<color=lime>Linked</color>");
        }
        #endif

        #region Utilities
        
        private void CheckCasterValidity()
        {
            if (requiredCaster == null) return;

            if (resultTypeSelection == null)
            {
                Debug.LogError("You need to define <b>resultTypeSelection</b> before defining a caster!");
                
                requiredCaster = null;
                return;
            }

            if (RequiredResultType != requiredCaster.CastResultType)
            {
                Debug.LogError($"Selected caster <color=red>{requiredCaster.gameObject.name}</color> has a different CastResultType " +
                               $"than the one required by this spell! Required: <color=red>{RequiredResultType}</color>, Caster: " +
                               $"<color=red>{requiredCaster.CastResultType}</color>");
                
                requiredCaster = null;
                return;
            }
            
            if(requiredCaster.SpellDataType != GetType())
            {
                Debug.LogError($"Selected caster <color=red>{requiredCaster.gameObject.name}</color> only works " +
                               $"with SpellData of type <b>{requiredCaster.SpellDataType}</b>. Type of this asset is <b>{GetType()}</b>");
                
                requiredCaster = null;
            }
        }
        
        private bool IsInstantiated()
        {
            return instantiationType != SpellInstantiationType.None;
        }
        
        #if UNITY_EDITOR
        
        [Button]
        [Tooltip("Sometimes Github don't detect the changes made on the ScriptableObject, so we need to force the write on the disk")]
        [ParrelSyncIgnore]
        private void ForceSaveOnDisk()
        {
            EditorUtility.SetDirty(this);
            AssetDatabase.SaveAssets();
            AssetDatabase.Refresh();
        }
        
        #endif
        
        #endregion
    }
}